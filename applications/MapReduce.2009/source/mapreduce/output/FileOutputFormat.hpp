//  Copyright (c) 2009 Miklos Erdelyi
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef MAPREDUCE_OUTPUT_FILEOUTPUTFORMAT_HPP_
#define MAPREDUCE_OUTPUT_FILEOUTPUTFORMAT_HPP_

#include <boost/lexical_cast.hpp>
#include <saga/saga.hpp>
#include "../job.hpp"
#include "../io/raw_format.hpp"

namespace mapreduce {

#define JOB_ATTRIBUTE_FILE_OUTPUTPATH "mapreduce.file.output_path"
#define JOB_ATTRIBUTE_FILE_OUTPUTBASE "mapreduce.file.output_base"

class FileOutputFormat : public RawOutputFormat {
 public:
  // Set output base for the job. This should include the schema for the
  // desired file system supported by a SAGA adaptor and an optional path
  // prefix.
  static void SetOutputBase(JobDescription& job, const std::string& value);
  // Get output base for the job.
  static std::string GetOutputBase(const JobDescription& job);
  // Set the output path for the job.
  static void SetOutputPath(JobDescription& job, const std::string& path);
  // Get output path specified in the given JobDescription.
  static std::string GetOutputPath(const JobDescription& job);
  // Get a fully qualified output URL for the job for the given path.
  // This method concatenates the default output base and the path.
  static saga::url GetUrl(const JobDescription& job, const std::string& path);
  // Get the name of the partition specified by the parameters.
  static std::string GetOutputPartitionName(const JobDescription* job,
    const std::string& task, const std::string& name, int partition);
  // Get the path to a unique temporary file for the task.
  static std::string GetUniqueWorkFile(TaskDescription* task);
};

}   // namespace mapreduce

#endif  // MAPREDUCE_OUTPUT_FILEOUTPUTFORMAT_HPP_
