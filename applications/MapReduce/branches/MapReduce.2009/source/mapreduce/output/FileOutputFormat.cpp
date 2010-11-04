//  Copyright (c) 2009 Miklos Erdelyi
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "FileOutputFormat.hpp"
#include "input_output.hpp"

namespace mapreduce {

void FileOutputFormat::SetOutputBase(JobDescription& job,
  const std::string& value) {
  job.set_attribute(JOB_ATTRIBUTE_FILE_OUTPUTBASE, value);
}

std::string FileOutputFormat::GetOutputBase(const JobDescription& job) {
  return job.get_attribute(JOB_ATTRIBUTE_FILE_OUTPUTBASE, "file://localhost/");
}

void FileOutputFormat::SetOutputPath(JobDescription& job, const std::string& path) {
  std::string current_paths(job.get_attribute(JOB_ATTRIBUTE_FILE_OUTPUTPATH));
  if (!current_paths.empty()) {
    current_paths += PATH_LIST_SEPARATOR;
  }
  current_paths += path;
  // Replace attribute value.
  job.set_attribute(JOB_ATTRIBUTE_FILE_OUTPUTPATH, current_paths);
}

std::string FileOutputFormat::GetOutputPath(const JobDescription& job) {
  return job.get_attribute(JOB_ATTRIBUTE_FILE_OUTPUTPATH, "/tmp/");
}

saga::url FileOutputFormat::GetUrl(const JobDescription& job,
  const std::string& path) {
  std::string result(GetOutputBase(job));
  result.append(path);
  return saga::url(result);
}

std::string FileOutputFormat::GetOutputPartitionName(const JobDescription* job,
  const std::string& task, const std::string& name, int partition) {
  std::string path(task);
  path.append(1, '-');
  path.append(name);
  path.append(1, '-');
  path.append(boost::lexical_cast<std::string>(partition));
  return path;
}

std::string FileOutputFormat::GetUniqueWorkFile(TaskDescription* task) {
  std::string path(FileOutputFormat::GetOutputPath(*task));
  path += "part-";
  path += boost::lexical_cast<std::string>(task->get_id());
  return path;
}

}   // namespace mapreduce
