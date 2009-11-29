#ifndef MAPREDUCE_INPUT_LINEREADER_HPP_
#define MAPREDUCE_INPUT_LINEREADER_HPP_

#include <climits>
#include <string>
#include "serialization.hpp"
#include "../io/saga_file_adaptors.hpp"

using google::protobuf::io::CopyingInputStreamAdaptor;
using google::protobuf::io::CopyingInputStream;

namespace mapreduce {

class LineReader {
 public:
  LineReader(ZeroCopyInputStream* input_stream)
      : buffer_pointer_(NULL), buffer_position_(0), buffer_length_(0),
        input_stream_(input_stream) {
  }
  // Reads one line from the input stream. Lines can be terminated by any pattern
  // of \r or \n characters. Returns with the number of bytes consumed from the
  // stream.
  int ReadLine(std::string* line, int max_line_length);
  int ReadLine(std::string* line) {
    return ReadLine(line, INT_MAX);
  }
 private:
  const char* buffer_pointer_;
  int buffer_position_;
  int buffer_length_;
  ZeroCopyInputStream* input_stream_;
};

}   // namespace mapreduce

#endif  // MAPREDUCE_INPUT_LINEREADER_HPP_
