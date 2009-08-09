//  Copyright (c) 2009 Miklos Erdelyi
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "Reducer.hpp"

namespace mapreduce {

// Comparator performing per-bit comparison of raw data regardless of the
// underlying object type.
template <class T>
int RawBytesComparator<T>::Compare(uint8* data1, int size1, uint8* data2,
  int size2) {
  int size = std::min(size1, size2);
  return memcmp(reinterpret_cast<const void*>(data1),
    reinterpret_cast<const void*>(data2), size);
}

// Helper function for consuming data from input and writing it into a string.
inline static void CopyStreamIntoString(ZeroCopyInputStream* input, std::string& output) {
  const void* buffer;
  int size;
  int total_size = 0;
  // Copy stream's content into output.
  while (input->Next(&buffer, &size)) {
    output.append(reinterpret_cast<const char*>(buffer), size);
    total_size += size;
  }
}

template <typename KeyIn, typename ValueIn, typename KeyOut, typename ValueOut>
void ReducerContext<KeyIn, ValueIn, KeyOut, ValueOut>::Initialize(
  RawRecordReader* input, RawComparator<KeyIn>* comparator,
  RecordWriter<KeyOut, ValueOut>* writer) {
  comparator_ = comparator;
  writer_ = writer;
  input_ = input;
  exhausted_ = !input_->NextRecord();
  if (!exhausted_) {
    CopyStreamIntoString(input_->current_key(), key_buffer_);
  }
  same_as_previous_key_ = false;
}

template <typename KeyIn, typename ValueIn, typename KeyOut, typename ValueOut>
bool ReducerContext<KeyIn, ValueIn, KeyOut, ValueOut>::NextKey() {
  while (same_as_previous_key_ && !exhausted_) {
    NextPair();
  }
  if (!exhausted_) {
    // Deserialize key/value.
    NextPair();
    return true;
  } else {
    return false;
  }
}

template <typename KeyIn, typename ValueIn, typename KeyOut, typename ValueOut>
bool ReducerContext<KeyIn, ValueIn, KeyOut, ValueOut>::NextPair() {
  if (exhausted_) {
    // Shouldn't have been called.
    return false;
  }
  //std::cerr<<"nextpair ";
  // Deserialize current key/value.
  SerializationHandler<KeyIn>::Deserialize(input_->current_key(), &key_);
  SerializationHandler<ValueIn>::Deserialize(input_->current_value(), &value_);
  // Read next pair.
  exhausted_ = !input_->NextRecord();
  // Check if it has the same as the current key.
  if (exhausted_) {
    same_as_previous_key_ = false;
  } else {
    //LOG_DEBUG << "compareee ";
    // TODO(miklos): should be done with pointer exchange to avoid copying.
    previous_key_ = key_buffer_;
    // Read key into buffer.
    key_buffer_.clear();
    CopyStreamIntoString(input_->current_key(), key_buffer_);
    // Compare it with previous key.
    same_as_previous_key_ = 0 == comparator_->Compare(
      reinterpret_cast<uint8*>(string_as_array(&previous_key_)),
      previous_key_.size(),
      reinterpret_cast<uint8*>(string_as_array(&key_buffer_)),
      key_buffer_.size());
      LOG_DEBUG << "prev " << previous_key_ << " n " << key_buffer_ << " cm" << same_as_previous_key_;
  }
  return true;
}


//
// Iterator implementation.
//
template <typename KeyIn, typename ValueIn, typename KeyOut, typename ValueOut>
bool ReducerContext<KeyIn, ValueIn, KeyOut, ValueOut>::Next() {
  if (iterator_begin_ || !stop_) {
    if (iterator_begin_) {
      // Key/value has been deserialized by NextKey().
      iterator_begin_ = false;
    } else {
      NextPair();
      if (!same_as_previous_key_) {
        stop_ = true;
      }
    }
    return true;
  } else {
    return false;
  }
}

}   // namespace mapreduce
