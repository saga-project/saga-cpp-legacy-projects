//  Copyright (c) 2009 Miklos Erdelyi
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef MAPREDUCE_OUTPUT_FILEOUTPUTFORMAT_HPP_
#define MAPREDUCE_OUTPUT_FILEOUTPUTFORMAT_HPP_

#include <boost/lexical_cast.hpp>
#include "../job.hpp"
#include "../io/raw_format.hpp"

namespace mapreduce {

#define JOB_ATTRIBUTE_FILE_OUTPUTPATH "mapreduce.file.output_path"

class FileOutputFormat : public RawOutputFormat {
 public:
  static void SetOutputPath(JobDescription& job, const std::string& path);
  static std::string GetOutputPath(const JobDescription& job);
  static std::string GetOutputPartitionName(const JobDescription* job,
    const std::string& task, const std::string& name, int partition);
  static std::string GetUniqueWorkFile(TaskDescription* task);
};

}   // namespace mapreduce

#endif  // MAPREDUCE_OUTPUT_FILEOUTPUTFORMAT_HPP_
