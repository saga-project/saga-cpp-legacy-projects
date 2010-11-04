//  Copyright (c) 2009 Miklos Erdelyi
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef MAPREDUCE_WORKER_REDUCETASKRUNNER_HPP_
#define MAPREDUCE_WORKER_REDUCETASKRUNNER_HPP_

#include "../Reducer.hpp"
#include "../../utils/logging.hpp"

namespace mapreduce { namespace worker {

class ReduceTaskRunner {
 public:
  ReduceTaskRunner(TaskDescription* job, int partition,
    saga::advert::directory& input_dir)
    : job_(job), partition_(partition), input_dir_(input_dir) {}
  ~ReduceTaskRunner() {}
  void Execute();
 private:
  logger log_;
  TaskDescription* job_;
  int partition_;
  saga::advert::directory& input_dir_;
};

}   // namespace worker
}   // namespace mapreduce

#endif  // MAPREDUCE_WORKER_REDUCETASKRUNNER_HPP_
