//  Copyright (c) 2009 Miklos Erdelyi
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "ReduceTaskRunner.hpp"

// A Reducer that outputs all input.
class IdentityReducer : public mapreduce::Reducer<std::string, std::string,
  std::string, std::string> {
 public:
  void Reduce(const std::string& key,  Iterator<std::string>& values,
    Context* context) {
    while (values.Next()) {
      context->Emit(key, values.Current());
    }
  }
};
REGISTER_REDUCER_CLASS(IdentityReducer, 0);

namespace mapreduce { namespace worker {

void ReduceTaskRunner::Execute() {
  LOG_DEBUG << "Executing reducer " << job_->get_reducer_class();
  // Get ReduceRunner for the specified reducer class.
  mapreduce::ReduceRunner* runner = mapreduce::ReduceRunnerFactory::get_by_key(
    job_->get_reducer_class());
  if (runner == NULL) {
    throw saga::exception("Could not instantiate Reducer class");
  }
  // Execute reduce task.
  runner->Execute(job_, input_dir_);
}

}   // namespace worker
}   // namespace mapreduce
