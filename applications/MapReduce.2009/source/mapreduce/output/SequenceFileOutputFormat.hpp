//  Copyright (c) 2009 Miklos Erdelyi
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef MAPREDUCE_OUTPUT_SEQUENCEFILEOUTPUTFORMAT_HPP_
#define MAPREDUCE_OUTPUT_SEQUENCEFILEOUTPUTFORMAT_HPP_

#include "../input_output.hpp"
#include "FileOutputFormat.hpp"
#include "../io/SequenceFile.hpp"
#include "../factory_impl.hpp"

using mapreduce::io::SequenceFileWriter;

namespace mapreduce {

// RawRecordWriter implementation for SequenceFiles.
class SequenceFileRecordWriter : public RawRecordWriter {
 public:
  SequenceFileRecordWriter(saga::url& path) {
    writer_.reset(new SequenceFileWriter(path));
  }
  ~SequenceFileRecordWriter() {}
  // RawRecordWriter implementation.
  void Write(const std::string& key, const std::string& value) {
    writer_->AppendRaw(key.data(), key.size(), value.data(), value.size());
  }
  void Close() {
    writer_->Close();
  }
 private:
  boost::scoped_ptr<SequenceFileWriter> writer_;
};

// Output format for SequenceFiles.
class SequenceFileOutputFormat : public FileOutputFormat {
 public:
  RawRecordWriter* GetRecordWriter(TaskDescription* task) {
    // Default work path for task.
    saga::url default_url = FileOutputFormat::GetUrl(*task,
      FileOutputFormat::GetUniqueWorkFile(task));
    return new SequenceFileRecordWriter(default_url);
  }
};

}   // namespace mapreduce

REGISTER_OUTPUTFORMAT(SequenceFile, mapreduce::SequenceFileOutputFormat, 1);

#endif  // MAPREDUCE_OUTPUT_SEQUENCEFILEOUTPUTFORMAT_HPP_
