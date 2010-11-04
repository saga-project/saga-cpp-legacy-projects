//  Copyright (c) 2009 Miklos Erdelyi
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef MAPREDUCE_WORKER_MERGINGRAWRECORDREADER_
#define MAPREDUCE_WORKER_MERGINGRAWRECORDREADER_

#include <string>
#include <vector>
#include <boost/scoped_ptr.hpp>
#include "serialization.hpp"
#include "input_output.hpp"

namespace mapreduce { namespace worker {

// Class for wrapping record readers. Takes over the ownership of the given
// RawRecordReader object.
class ReaderWrapper {
 public:
  ReaderWrapper(RawRecordReader* reader) : exhausted_(false) {
    reader_.reset(reader);
    Next();
  }
  bool exhausted() const { return exhausted_; }
  bool Next() {
    if (!reader_->NextRecord()) {
      exhausted_ = true;
      return false;
    } else {
      return true;
    }
  }
  inline ZeroCopyInputStream* current_key() {
    return reader_->current_key();
  }
  inline void get_key_buffer(const uint8** buffer, int* size) {
    reader_->get_key_buffer(buffer, size);
  }
  inline ZeroCopyInputStream* current_value() {
    return reader_->current_value();
  }
  RawRecordReader* reader() { return reader_.get(); }
 private:
  boost::scoped_ptr<RawRecordReader> reader_;
  bool exhausted_;  // Whether this reader has beeen exhausted.
};

// Class for comparing the underlying keys in ReaderWrappers. Simply obtains
// the buffers for the underlying keys and applies the comparator specified as
// the template parameter for comparison.
template <class Comparator>
class ReaderWrapperComparator {
 public:
  ReaderWrapperComparator() {}
  bool operator()(ReaderWrapper* a, ReaderWrapper* b) {
    const uint8* a_buffer;
    const uint8* b_buffer;
    int a_size, b_size;
    a->get_key_buffer(&a_buffer, &a_size);
    b->get_key_buffer(&b_buffer, &b_size);
    return compare_.Compare(a_buffer, a_size, b_buffer, b_size) > 0;
  }
 private:
  Comparator compare_;
};

// Class for reading from several record readers backed by data in ascending
// key order. The records output by this reader will also have an ascending
// order of keys.
template <class Key, class Comparator>
class MergingRawRecordReader : public RawRecordReader {
 public:
  MergingRawRecordReader(const std::vector<RawRecordReader*>& readers)
    : top_(NULL) {
    Setup(readers);
  }
  void Initialize(InputChunk* input_chunk) {}
  // Retrieve next record.
  bool NextRecord() {
    // Advance previous top reader and maintain heap property.
    if (NULL != top_) {
      // Remove top reader from heap.
      std::pop_heap(reader_heap_.begin(), reader_heap_.end(),
        ReaderWrapperComparator<Comparator>());
      reader_heap_.pop_back();
      // Advance top reader.
      top_->Next();
      // Put it back into the heap.
      reader_heap_.push_back(top_);
      // Maintain heap property.
      std::push_heap(reader_heap_.begin(), reader_heap_.end(),
        ReaderWrapperComparator<Comparator>());
    }
    // Note reader on top of the heap.
    top_ = reader_heap_.front();
    if (top_->exhausted()) {   // All readers are exhausted.
      return false;
    } else {
      return true;
    }
  }
  // Get inputstream for current key.
  ZeroCopyInputStream* current_key() {
    return (top_ != NULL) ? top_->current_key() : NULL;
  }
  // Get inmutable buffer for current key.
  void get_key_buffer(const uint8** buffer, int* size) {
    top_->get_key_buffer(buffer, size);
  }
  // Get inputstream for current value.
  ZeroCopyInputStream* current_value() {
    return (top_ != NULL) ? top_->current_value() : NULL;
  }
  // Close the record readers.
  void Close() {
    std::vector<ReaderWrapper*>::const_iterator reader_it =
      reader_heap_.begin();
    while (reader_it != reader_heap_.end()) {
      (*reader_it)->reader()->Close();
      delete *reader_it;
      ++reader_it;
    }
    reader_heap_.clear();
  }
 private:
  void Setup(const std::vector<RawRecordReader*>& readers) {
    // Create reader heap.
    std::vector<RawRecordReader*>::const_iterator reader_it = readers.begin();
    while (reader_it != readers.end()) {
      reader_heap_.push_back(new ReaderWrapper(*reader_it));
      ++reader_it;
    }
    // Heapify.
    std::make_heap(reader_heap_.begin(), reader_heap_.end(),
      ReaderWrapperComparator<Comparator>());
  }

  DECLARE_LOGGER(MergingRawRecordReader);
  std::vector<ReaderWrapper*> reader_heap_;
  ReaderWrapper* top_;  // The reader having the top-most key.
};

}   // namespace worker
}   // namespace mapreduce

#endif  // MAPREDUCE_WORKER_MERGINGRAWRECORDREADER_
