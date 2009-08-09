//  Copyright (c) 2009 Miklos Erdelyi
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef MAPREDUCE_INPUT_SEQUENCEFILEINPUTFORMAT_HPP_
#define MAPREDUCE_INPUT_SEQUENCEFILEINPUTFORMAT_HPP_

#include "input_output.hpp"
#include "io/SequenceFile.hpp"
#include "FileInputFormat.hpp"
#include "factory_impl.hpp"

using mapreduce::io::SequenceFileReader;

namespace mapreduce {

// RawRecordReader implementation for SequenceFiles.
class SequenceFileRecordReader : public RawRecordReader {
 public:
  SequenceFileRecordReader(const saga::url& path, long offset = 0) {
    Setup(path, offset);
  }
  SequenceFileRecordReader() {}

  // Initialize with an InputChunk.
  void Initialize(InputChunk* input_chunk) {
    FileInputChunk* chunk = dynamic_cast<FileInputChunk*>(input_chunk);
    if (NULL == chunk) {
      throw saga::exception("Wrong InputChunk implementation given to "\
        "SequenceFileRecordReader");
    }
    long offset = chunk->start_offset();
    Setup(chunk->path(), offset);
  }
  // Retrieve next record.
  bool NextRecord() {
    // Clear buffers.
    key_buffer_.clear();
    value_buffer_.clear();
    return reader_->ReadRaw(key_holder_.get(), value_holder_.get());
  }
  // Get inputstream for current key.
  ZeroCopyInputStream* current_key() {
    key_input_.reset(new ArrayInputStream(string_as_array(&key_buffer_),
      key_buffer_.size()));
    return key_input_.get();
  }
  // Get inputstream for current value.
  ZeroCopyInputStream* current_value() {
    value_input_.reset(new ArrayInputStream(string_as_array(&value_buffer_),
      value_buffer_.size()));
    return value_input_.get();
  }
  // Close the record reader.
  void Close() {
    reader_->Close();
  }
 private:
  // Create SequenceFileReader and setup buffers.
  void Setup(const saga::url& path, long offset = 0) {
    reader_.reset(new SequenceFileReader(path, offset));
    key_holder_.reset(new StringOutputStream(&key_buffer_));
    value_holder_.reset(new StringOutputStream(&value_buffer_));
  }
  boost::scoped_ptr<SequenceFileReader> reader_;
  boost::scoped_ptr<StringOutputStream> key_holder_;
  boost::scoped_ptr<StringOutputStream> value_holder_;
  // FIXME: these should be resettable ArrayInputStreams to avoid frequent
  // malloc/free.
  boost::scoped_ptr<ArrayInputStream> key_input_;
  boost::scoped_ptr<ArrayInputStream> value_input_;
  std::string key_buffer_;
  std::string value_buffer_;
};

// Input format for SequenceFiles.
class SequenceFileInputFormat : public FileInputFormat {
 public:
  RawRecordReader* GetRecordReader(InputChunk* chunk, JobDescription* job) {
    SequenceFileRecordReader* reader = new SequenceFileRecordReader();
    reader->Initialize(chunk);
    return reader;
  }
};

}   // namespace mapreduce

// Register input format.
//REGISTER_INPUTFORMAT(SequenceFile, mapreduce::SequenceFileInputFormat, 2);

#endif  // MAPREDUCE_INPUT_SEQUENCEFILEINPUTFORMAT_HPP_
