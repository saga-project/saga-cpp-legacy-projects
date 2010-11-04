//  Copyright (c) 2009 Miklos Erdelyi
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef MAPREDUCE_IO_SEQUENCEFILE_HPP_
#define MAPREDUCE_IO_SEQUENCEFILE_HPP_

#include <saga/saga.hpp>
#include "../serialization.hpp"
#include "../io/saga_file_adaptors.hpp"

using google::protobuf::io::ZeroCopyOutputStream;
using google::protobuf::io::CopyingOutputStreamAdaptor;
using google::protobuf::io::CopyingInputStreamAdaptor;
using google::protobuf::io::CodedOutputStream;
using google::protobuf::io::CodedInputStream;

namespace mapreduce { namespace io {

class SequenceFileReader;
class SequenceFileWriter;

#define SEQUENCEFILE_VERSION  1

// Class holding common infrastructure for SequenceFileReader/SequenceFileWriter.
class SequenceFile {
 public:
  static char header[];
  friend class SequenceFileReader;
  friend class SequenceFileWriter;
};

// Reader for sequences of records.
class SequenceFileReader {
 public:
  SequenceFileReader(const saga::url& path, long offset = 0) {
    Initialize(path, offset);
  }
  bool ReadRaw(ZeroCopyOutputStream* key, ZeroCopyOutputStream* value);
  void Close();
 private:
  void Initialize(const saga::url& path, long offset);

  SagaFileInputStream* input_stream_;
  boost::scoped_ptr<CopyingInputStreamAdaptor> input_adaptor_;
  CodedInputStream* input_;
};

// Writer for sequences of records.
class SequenceFileWriter {
 public:
  SequenceFileWriter(saga::url& path, bool append = false) {
    Initialize(path, append);
  }
  void AppendRaw(const void* raw_key, int key_length, const void* raw_value,
    int value_length);
  void Close();
 private:
  void Initialize(saga::url& path, bool append);
  void WriteHeader();

  saga::filesystem::file* file_;
  boost::scoped_ptr<CopyingOutputStreamAdaptor> output_adaptor_;
  CodedOutputStream* output_;
};

}   // namespace io
}   // namespace mapreduce

#endif  // MAPREDUCE_IO_SEQUENCEFILE_HPP_
