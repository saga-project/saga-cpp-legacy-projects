//  Copyright (c) 2009 Miklos Erdelyi
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "job.hpp"

namespace mapreduce {

std::string JobDescription::default_empty_string;

//
// Job
//
void JobDescription::Serialize(CodedOutputStream* output) const {
  // FIXME: replace with generalized collection serializer (ala boost::serialization).
  output->WriteVarint32(attributes_.size());
  std::string buffer;
  StringOutputStream string_out(&buffer);
  for (std::map<std::string, std::string>::const_iterator it = attributes_.begin();
    it != attributes_.end(); ++it) {
    SerializationHandler<std::string>::Serialize(&it->first, &string_out);
    SerializationHandler<std::string>::Serialize(&it->second, &string_out);
  }
  output->WriteVarint32(buffer.size());
  output->WriteString(buffer);
}

void JobDescription::Deserialize(CodedInputStream* input) {
  attributes_.clear();
  uint32 num_attributes;
  input->ReadVarint32(&num_attributes);
  uint32 size;
  input->ReadVarint32(&size);
  std::string buffer;
  input->ReadString(&buffer, size);
  ArrayInputStream string_in(string_as_array(&buffer), size);
  for (uint32 i = 0; i < num_attributes; ++i) {
    std::string key;
    SerializationHandler<std::string>::Deserialize(&string_in, &key);
    std::string value;
    SerializationHandler<std::string>::Deserialize(&string_in, &value);
    attributes_[key] = value;
  }
}

//
// TaskDescription
//
TaskDescription::TaskDescription(JobDescription* job) {
  Initialize(job);
}

}   // namespace mapreduce
