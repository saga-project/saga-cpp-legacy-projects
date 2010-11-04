//  Copyright (c) 2009 Miklos Erdelyi
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "TextInputFormat.hpp"

using google::protobuf::io::ArrayInputStream;
using google::protobuf::io::CopyingInputStream;
using google::protobuf::io::CopyingInputStreamAdaptor;

using mapreduce::io::SagaFileInputStream;

namespace mapreduce {

void TextRecordReader::Initialize(InputChunk* chunk) {
  // Open file.
  FileInputChunk* file_chunk = dynamic_cast<FileInputChunk*>(chunk);
  saga::url file_url(file_chunk->path());
  SagaFileInputStream* file_input = new SagaFileInputStream(file_url,
    saga::filesystem::Read);
  // Seek to the beginning of the chunk.
  file_offset_ = file_chunk->start_offset();
  end_offset_ = file_offset_ + file_chunk->GetLength();
  // Adjust offset if necessary.
  bool skip_first_line = false;
  if (file_offset_ > 0) {
    skip_first_line = true;
    --file_offset_;
  }
  file_input->Skip(file_offset_);
  CopyingInputStreamAdaptor* adaptor = new CopyingInputStreamAdaptor(
    file_input);
  // The adaptor should free the CopyingStream.
  adaptor->SetOwnsCopyingStream(true);
  input_stream_.reset(adaptor);
  reader_.reset(new LineReader(input_stream_.get()));
  if (skip_first_line) {
    file_offset_ += reader_->ReadLine(&line_);
  }
}

bool TextRecordReader::NextRecord() {
  line_.clear();
  int start_offset = file_offset_;
  int read_size = 0;
  if (file_offset_ < end_offset_) {
    read_size = reader_->ReadLine(&line_);
    file_offset_ += read_size;
  }
  if (read_size == 0) {
    // No more data to read.
    current_key_.reset();
    current_value_.reset();
    return false;
  }
  // Serialize offset as an integer.
  key_buffer_.clear();
  StringOutputStream key_output(&key_buffer_);
  SerializationHandler<int>::Serialize(&start_offset, &key_output);
  // Serialize line as a string.
  value_buffer_.clear();
  StringOutputStream value_output(&value_buffer_);
  SerializationHandler<std::string>::Serialize(&line_, &value_output);
  // Reset key and value input streams.
  // FIXME: should use resettable input stream instead.
  current_key_.reset(new ArrayInputStream(string_as_array(&key_buffer_),
    key_buffer_.size()));
  current_value_.reset(new ArrayInputStream(string_as_array(&value_buffer_),
    value_buffer_.size()));
  return true;
}

}   // namespace mapreduce
