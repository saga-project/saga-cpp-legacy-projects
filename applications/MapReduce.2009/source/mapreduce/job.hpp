//  Copyright (c) 2009 Miklos Erdelyi
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef MAPREDUCE_JOB_HPP_
#define MAPREDUCE_JOB_HPP_

#include <map>
#include <string>
#include <boost/lexical_cast.hpp>

#include "serialization.hpp"

using google::protobuf::io::ArrayInputStream;

namespace mapreduce {

#define JOB_ATTRIBUTE_INPUTFORMAT "mapred.input.format"
#define JOB_ATTRIBUTE_OUTPUTFORMAT "mapred.output.format"
#define JOB_ATTRIBUTE_MAPPER_CLASS "mapred.mapper.class"
#define JOB_ATTRIBUTE_PARTITIONER_CLASS "mapred.partitioner.class"
#define JOB_ATTRIBUTE_REDUCER_CLASS "mapred.reducer.class"
#define JOB_ATTRIBUTE_NUM_REDUCE_TASKS "mapred.reducer.num_tasks"

// Descriptor of a MapReduce job.
// FIXME: this should be based on saga::attributes.
class JobDescription : public Serializable {
 public:
  void set_attribute(const std::string& name, const std::string& value) {
    attributes_[name] = value;
  }
  const std::string& get_attribute(const std::string& name,
    const std::string& default_value = default_empty_string) const {
    std::map<std::string, std::string>::const_iterator it =
      attributes_.find(name);
    if (it != attributes_.end()) {
      return it->second;
    } else {
      return default_value;
    }
  }
  std::string get_attribute(const std::string& name,
    const char* default_value) {
    const std::string& result = get_attribute(name);
    if (result.empty()) {
      return std::string(default_value);
    } else {
      return result;
    }
  }
  int get_attribute(const std::string& name, int default_value) const {
    return boost::lexical_cast<int>(get_attribute(name, "0"));
  }
  float get_attribute(const std::string& name, float default_value) const {
    return boost::lexical_cast<float>(get_attribute(name, "0.0"));
  }
  void set_input_format(const std::string& input_format) {
    attributes_[JOB_ATTRIBUTE_INPUTFORMAT] = input_format;
  }
  const std::string& get_input_format() const {
    return get_attribute(JOB_ATTRIBUTE_INPUTFORMAT, "SequenceFile");
  }
  void set_output_format(const std::string& output_format) {
    attributes_[JOB_ATTRIBUTE_OUTPUTFORMAT] = output_format;
  }
  const std::string& get_output_format() const {
    return get_attribute(JOB_ATTRIBUTE_OUTPUTFORMAT, "SequenceFile");
  }
  void set_mapper_class(const std::string& classname) {
    attributes_[JOB_ATTRIBUTE_MAPPER_CLASS] = classname;
  }
  const std::string& get_mapper_class() const {
    return get_attribute(JOB_ATTRIBUTE_MAPPER_CLASS);
  }
  void set_partitioner_class(const std::string& classname) {
    attributes_[JOB_ATTRIBUTE_PARTITIONER_CLASS] = classname;
  }
  const std::string& get_partitioner_class() const {
    return get_attribute(JOB_ATTRIBUTE_PARTITIONER_CLASS);
  }
  void set_reducer_class(const std::string& classname) {
    attributes_[JOB_ATTRIBUTE_REDUCER_CLASS] = classname;
  }
  const std::string& get_reducer_class() const {
    return get_attribute(JOB_ATTRIBUTE_REDUCER_CLASS, "IdentityReducer");
  }
  void set_num_reduce_tasks(int num_tasks) {
    attributes_[JOB_ATTRIBUTE_NUM_REDUCE_TASKS] =
      boost::lexical_cast<std::string>(num_tasks);
  }
  const int get_num_reduce_tasks() const {
    const std::string& num_tasks = get_attribute(JOB_ATTRIBUTE_NUM_REDUCE_TASKS,
      "0");
    return boost::lexical_cast<int>(num_tasks);
  }
  // Serializable interface implementation.
  virtual void Serialize(CodedOutputStream* output) const;
  virtual void Deserialize(CodedInputStream* input);
 protected:
  void Initialize(const JobDescription* other) {
    // Copy attributes from other descrtiption.
    attributes_ = other->attributes_;
  }
 private:
  std::map<std::string, std::string> attributes_;
  static std::string default_empty_string;
};

// Description of a task, eg., a map or reduce task.
class TaskDescription : public JobDescription {
 public:
  TaskDescription(JobDescription* job);
  // Partition number.
  void set_id(int id) { task_id_ = id; }
  int get_id() { return task_id_; }
 private:
  int task_id_;
};

} // namespace mapreduce

#endif  // MAPREDUCE_JOB_HPP_
