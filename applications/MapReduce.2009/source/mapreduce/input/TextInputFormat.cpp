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
  file_input->Skip(file_offset_);
  CopyingInputStreamAdaptor* adaptor = new CopyingInputStreamAdaptor(
    file_input);
  // The adaptor should free the CopyingStream.
  adaptor->SetOwnsCopyingStream(true);
  input_stream_.reset(adaptor);
}

bool TextRecordReader::NextRecord() {
  line_.clear();
  int newline_length = 0;
  int start_position = buffer_position_;
  int start_offset = file_offset_;
  while (1) {
    if (buffer_position_ >= buffer_length_) {
      // Read another portion of the input.
      bool success = input_stream_->Next(reinterpret_cast<const void**>(
        &buffer_pointer_), &buffer_length_);
      if (!success) {
        return false;
      }
      buffer_position_ = start_position = 0;
    }
    bool previous_cr = false;   // Whether we read a CR in the previous pass.
    for (; buffer_position_ < buffer_length_; ++buffer_position_) {
      if (buffer_pointer_[buffer_position_] == '\r') {
        previous_cr = true;
        newline_length = 1;
        continue;
      } else if (buffer_pointer_[buffer_position_] == '\n') {
        newline_length = previous_cr ? 2 : 1;
        ++buffer_position_;
        break;
      }
    }
    if (previous_cr && newline_length == 1) {
      ++buffer_position_;
    }
    file_offset_ += buffer_position_;
    // Decide how much to append to the line buffer.
    int append_length = buffer_position_ - start_position - newline_length;
    if (append_length > 0) {
      line_.append(reinterpret_cast<const char*>(&buffer_pointer_[start_position]),
        append_length);
    }
    if (newline_length > 0 && line_.size() > 0) {
      break;
    } else if (newline_length > 0) {
      // Skip newline characters.
      buffer_position_ += newline_length;
    }
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
  if (current_key_) delete current_key_;
  current_key_ = new ArrayInputStream(string_as_array(&key_buffer_),
    key_buffer_.size());
  if (current_value_) delete current_value_;
  current_value_ = new ArrayInputStream(string_as_array(&value_buffer_),
    value_buffer_.size());
  return true;
}

void TextRecordReader::Close() {
}

}   // namespace mapreduce
