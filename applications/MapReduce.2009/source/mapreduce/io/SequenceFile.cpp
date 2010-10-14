//  Copyright (c) 2009 Miklos Erdelyi
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "SequenceFile.hpp"
#include <climits>
#include "../io/saga_file_adaptors.hpp" 

namespace mapreduce { namespace io {

char SequenceFile::header[] = {'S', 'Q', 'F', SEQUENCEFILE_VERSION};

//
// SequenceFileReader implementation.
//

void SequenceFileReader::Initialize(const saga::url& path, long offset) {
  int mode = saga::filesystem::Read;
  // Open file.
  input_stream_ = new SagaFileInputStream(path, mode);
  if (offset > 0) {
    input_stream_->Skip(offset);
  }
  // Read header.
  char header_contents[sizeof(SequenceFile::header)];
  input_stream_->Read(reinterpret_cast<void*>(&header_contents),
    sizeof(header_contents));
  if (header_contents[0] != SequenceFile::header[0] ||
    header_contents[1] != SequenceFile::header[1] ||
    header_contents[2] != SequenceFile::header[2]) {
    throw saga::exception(path.get_string() + " is not a SequenceFile");
  }
  if (header_contents[3] > SEQUENCEFILE_VERSION) {
    throw saga::exception(path.get_string() + " is unsupported");
  }
  // Create input stream.
  input_adaptor_.reset(new CopyingInputStreamAdaptor(input_stream_));
  input_adaptor_->SetOwnsCopyingStream(true);
  input_ = new CodedInputStream(input_adaptor_.get());
  // Disable warning about too many bytes read from input.
  input_->SetTotalBytesLimit(INT_MAX, -1);
}

// Copy a block of given size from the input stream into the output stream.
// Avoids memory copy operations.
inline static bool TransferBetweenStreams(CodedInputStream* input,
                                          ZeroCopyOutputStream* output,
                                          int size) {
  // Get buffer for output.
  void* buffer;
  int buffer_size;
  int remaining = size;
  while (remaining > 0) {
    output->Next(&buffer, &buffer_size);
    int read_size = std::min(buffer_size, remaining);
    if (!input->ReadRaw(buffer, read_size)) {
      return false;
    }
    remaining -= read_size;
    if (buffer_size > read_size) {
      output->BackUp(buffer_size - read_size);
    }
  }
  return true;
}

bool SequenceFileReader::ReadRaw(ZeroCopyOutputStream* key, ZeroCopyOutputStream* value) {
  uint32 record_length;
  uint32 key_length;
  if (!input_->ReadVarint32(&record_length)) {
    return false;
  }
  if (!input_->ReadVarint32(&key_length)) {
    return false;
  }
  if (!TransferBetweenStreams(input_, key, key_length)) {
    return false;
  }
  if (!TransferBetweenStreams(input_, value, record_length - key_length)) {
    return false;
  }
  return true;
}

void SequenceFileReader::Close() {
  delete input_;
}

//
// SequenceFileWriter implementation.
//

void SequenceFileWriter::Initialize(saga::url& path, bool append) {
  // Determine mode to be used for opening file.
  int mode = saga::filesystem::Write;
  if (append) {
    mode |= saga::filesystem::Append;
  } else {
    mode |= saga::filesystem::Create;
  }
  // Create CodedOutputStream for file.
  SagaFileOutputStream* copying_stream_ = new SagaFileOutputStream(
    saga::filesystem::file(path, mode));
  output_adaptor_.reset(new CopyingOutputStreamAdaptor(copying_stream_));
  // Let the adaptor free the CopyingStream.
  output_adaptor_->SetOwnsCopyingStream(true);
  output_ = new CodedOutputStream(output_adaptor_.get());
  if (!append) {
    WriteHeader();
  }
}

void SequenceFileWriter::AppendRaw(const void* raw_key, int key_length,
                                   const void* raw_value, int value_length) {
  int record_length = key_length + value_length;
  output_->WriteVarint32(record_length);
  output_->WriteVarint32(key_length);
  output_->WriteRaw(raw_key, key_length);
  output_->WriteRaw(raw_value, value_length);
}

void SequenceFileWriter::WriteHeader() {
  // Write magic bytes and version number.
  output_->WriteRaw(reinterpret_cast<void*>(&SequenceFile::header),
    sizeof(SequenceFile::header));
}

void SequenceFileWriter::Close() {
  delete output_;
  output_adaptor_->Flush();
  output_adaptor_.reset();
}

}   // namespace io
}   // namespace mapreduce
