//  Copyright (c) 2009 Miklos Erdelyi
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef MAPREDUCE_WORKER_TYPEDREDUCERUNNER_HPP_
#define MAPREDUCE_WORKER_TYPEDREDUCERUNNER_HPP_

#include "job.hpp"
#include "serialization.hpp"
#include "utils/logging.hpp"

namespace mapreduce { namespace worker {

// Class handling the actual execution of a map task.
template <typename ReducerT>
class TypedReduceRunner {
 public:
  void RunTask(TaskDescription* job, saga::advert::directory& input_dir);
 private:
  DECLARE_LOGGER(TypedMapRunner);
};

}   // namespace worker
}   // namespace mapreduce

// Implementation.
#include "TypedReduceRunner_impl.hpp"

#endif  // MAPREDUCE_WORKER_TYPEDREDUCERUNNER_HPP_
