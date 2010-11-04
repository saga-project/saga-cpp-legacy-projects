//  Copyright (c) 2009 Miklos Erdelyi
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef MAPREDUCE_INPUT_TEXTINPUTFORMAT_HPP_
#define MAPREDUCE_INPUT_TEXTINPUTFORMAT_HPP_

#include "../input_output.hpp"
#include "../io/saga_file_adaptors.hpp"
#include "../../utils/logging.hpp"
#include "FileInputFormat.hpp"
#include "LineReader.hpp"

using google::protobuf::io::CopyingInputStreamAdaptor;
using google::protobuf::io::ArrayInputStream;
using google::protobuf::io::CopyingInputStream;

namespace mapreduce {

// Record reader for getting <int, string> pairs from text files such that
// the key is the offset in the file, while the value is a line from the file.
class TextRecordReader : public RawRecordReader {
 public:
  TextRecordReader() : current_key_(NULL), current_value_(NULL) {}
  // RawRecordReader implementation.
  void Initialize(InputChunk* chunk);
  bool NextRecord();
  ZeroCopyInputStream* current_key() { return current_key_.get(); }
  ZeroCopyInputStream* current_value() { return current_value_.get(); }
  void Close() {}
 private:
  DECLARE_LOGGER(TextRecordReader);
  boost::scoped_ptr<CopyingInputStreamAdaptor> input_stream_;
  boost::scoped_ptr<LineReader> reader_;
  std::string line_;
  int file_offset_;
  int end_offset_;
  std::string key_buffer_;
  std::string value_buffer_;
  boost::scoped_ptr<ArrayInputStream> current_key_;
  boost::scoped_ptr<ArrayInputStream> current_value_;
};

// Input format for processing lines of text files.
class TextInputFormat : public FileInputFormat {
 public:
  // Create a record reader to be used by the Mapper/Combiner/Reducer function.
  RawRecordReader* GetRecordReader(InputChunk* chunk, JobDescription* job) {
    TextRecordReader* reader = new TextRecordReader();
    reader->Initialize(chunk);
    return reader;
  }
};

} // namespace mapreduce

// Register input format.
REGISTER_INPUTFORMAT(Text, mapreduce::TextInputFormat, 1);

#endif  // MAPREDUCE_INPUT_TEXTINPUTFORMAT_HPP_
