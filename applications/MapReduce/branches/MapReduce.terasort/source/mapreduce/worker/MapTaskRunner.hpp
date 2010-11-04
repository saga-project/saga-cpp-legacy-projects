//  Copyright (c) 2009 Miklos Erdelyi
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef MAPREDUCE_WORKER_MAPTASKRUNNER_HPP_
#define MAPREDUCE_WORKER_MAPTASKRUNNER_HPP_

#include "../Mapper.hpp"
#include "../../utils/logging.hpp"

namespace mapreduce { namespace worker {

class MapTaskRunner {
 public:
  MapTaskRunner(JobDescription* job, std::string& chunk_id,
    std::string* raw_chunk) : job_(job), chunk_id_(chunk_id),
    raw_chunk_(raw_chunk) {}
  ~MapTaskRunner() {
    delete job_;
    delete raw_chunk_;
  }
  void Execute();
 private:
  DECLARE_LOGGER(MapTaskRunner);
  JobDescription* job_;
  std::string chunk_id_;
  std::string* raw_chunk_;
};

}   // namespace worker
}   // namespace mapreduce

#endif  // MAPREDUCE_WORKER_MAPTASKRUNNER_HPP_
