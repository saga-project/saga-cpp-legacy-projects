//  Copyright (c) 2009 Miklos Erdelyi
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "MapTaskRunner.hpp"

// A Mapper that consumes input without producing any intermediate data.
class SinkMapper : public mapreduce::Mapper<std::string, std::string,
  std::string, std::string> {
 public:
  void Map(const std::string& key, const std::string& value, Context* context) {}
};
REGISTER_MAPPER_CLASS(SinkMapper, 0);

namespace mapreduce { namespace worker {

void MapTaskRunner::Execute() {
  LOG_DEBUG << "Executing mapper " << job_->get_mapper_class();
  // Get MapRunner for the specified mapper class.
  mapreduce::MapRunner* runner = mapreduce::MapRunnerFactory::get_by_key(
    job_->get_mapper_class());
  // Store chunk_id.
  job_->set_attribute("runner.chunk_id", chunk_id_);
  // Create input stream for raw chunk.
  ArrayInputStream ais(string_as_array(raw_chunk_), raw_chunk_->size());
  // Execute map task.
  runner->Execute(job_, &ais);
}

}   // namespace worker
}   // namespace mapreduce
