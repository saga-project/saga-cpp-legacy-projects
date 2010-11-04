//  Copyright (c) 2009 Miklos Erdelyi
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "Reducer.hpp"

namespace mapreduce {

// Comparator performing per-bit comparison of raw data regardless of the
// underlying object type.
template <class T>
int RawBytesComparator<T>::Compare(const uint8* data1, int size1,
  const uint8* data2, int size2) {
  int size = std::min(size1, size2);
  return memcmp(reinterpret_cast<const void*>(data1),
    reinterpret_cast<const void*>(data2), size);
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
  // Deserialize current key/value.
  SerializationHandler<KeyIn>::Deserialize(input_->current_key(), &key_);
  SerializationHandler<ValueIn>::Deserialize(input_->current_value(), &value_);
  // Read next pair.
  exhausted_ = !input_->NextRecord();
  // Check if it has the same as the current key.
  if (exhausted_) {
    same_as_previous_key_ = false;
  } else {
    // Read key into buffer.
    key_buffer_.clear();
    const uint8* key_buffer;
    int key_size;
    input_->get_key_buffer(&key_buffer, &key_size);
    if (static_cast<int>(previous_key_.size()) != key_size) {
      same_as_previous_key_ = false;
    } else {
      same_as_previous_key_ = 0 == memcmp(reinterpret_cast<const void*>(key_buffer),
        reinterpret_cast<const void*>(string_as_array(&previous_key_)),
        key_size);
    }
    // Save key.
    previous_key_.clear();
    previous_key_.append(reinterpret_cast<const char*>(key_buffer), key_size);
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
      if (!same_as_previous_key_) {
        stop_ = true;
      }
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
