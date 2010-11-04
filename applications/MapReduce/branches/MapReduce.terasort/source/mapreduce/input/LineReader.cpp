#include "LineReader.hpp"

namespace mapreduce {

int LineReader::ReadLine(std::string* line, int max_line_length) {
  line->clear();
  int newline_length = 0;
  int start_position = buffer_position_;
  int consumed_bytes = 0;
  int size = 0;
  while (1) {
    if (buffer_position_ >= buffer_length_) {
      // Read another portion of the input.
      bool success = input_stream_->Next(reinterpret_cast<const void**>(
        &buffer_pointer_), &buffer_length_);
      if (!success) {
        return 0;
      }
      buffer_position_ = start_position = 0;
    }
    bool previous_cr = false;   // Whether we read a CR in the previous pass.
    for (; buffer_position_ < buffer_length_; ++buffer_position_) {
      if (buffer_pointer_[buffer_position_] == '\r') {
        previous_cr = true;
        ++newline_length;
        continue;
      } else if (buffer_pointer_[buffer_position_] == '\n') {
        ++newline_length;
        continue;
      } else if (newline_length > 0){
        break;
      }
    }
    consumed_bytes += buffer_position_ - start_position;
    // Decide how much to append to the line buffer.
    int append_length = buffer_position_ - start_position - newline_length;
    if (append_length > 0) {
      line->append(reinterpret_cast<const char*>(&buffer_pointer_[start_position]),
          append_length);
      size = append_length;
    }
    if (newline_length > 0) {
      if (size > max_line_length) {
        size = 0;
        line->clear();
        continue;
      } else {
        break;
      }
    }
  }
  return consumed_bytes;
}

}   // namespace mapreduce
