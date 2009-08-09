//  Copyright (c) 2009 Miklos Erdelyi
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef MAPREDUCE_IO_SAGA_FILE_ADAPTORS_HPP_
#define MAPREDUCE_IO_SAGA_FILE_ADAPTORS_HPP_

#include <saga/saga.hpp>

using google::protobuf::io::CopyingInputStream;
using google::protobuf::io::CopyingOutputStream;

namespace mapreduce { namespace io {

// Adaptor for emulating CopyingInputStream through SAGA's file API.
class SagaFileInputStream : public CopyingInputStream {
 public:
  SagaFileInputStream(const saga::url& path, int mode) : file_(path, mode) {}
  // CopyingInputStream implementation.
  int Read(void* buffer, int size) {
    int ret =  file_.read(saga::buffer(buffer, size), size);
    return ret;
  }
  int Skip(int count) {
    // Move the internal file pointer ahead by count bytes.
    return file_.seek(count, saga::filesystem::Current);
  }
 private:
  saga::filesystem::file file_;
};

// Adaptor for emulating CopyingOutputStream through SAGA's file API.
class SagaFileOutputStream : public CopyingOutputStream {
 public:
  SagaFileOutputStream(saga::filesystem::file file) : file_(file) {}
  // CopyingOutputStream implementation.
  bool Write(const void* buffer, int size) {
    int written = file_.write(saga::buffer(buffer, size), size);
    return written == size;
  }
 private:
  saga::filesystem::file file_;
};

}   // namespace io
}   // namespace mapreduce

#endif  // MAPREDUCE_IO_SAGA_FILE_ADAPTORS_HPP_
