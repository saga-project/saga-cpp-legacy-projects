//  Copyright (c) 2009 Miklos Erdelyi
//  Copyright (c) 2008 Michael Miceli and Christopher Miceli
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef MR_HANDLE_REDUCES_HPP
#define MR_HANDLE_REDUCES_HPP

#include <string>
#include <vector>
#include <map>
#include <set>
#include <saga/saga.hpp>
#include "utils/LogWriter.hpp"
#include "job.hpp"
#include "protocol.hpp"
#include "utils/logging.hpp"

namespace mapreduce { namespace master {

class HandleReduces {
 public:
  HandleReduces(const JobDescription& job,
                std::map<std::string, saga::url>& committed_chunks,
                saga::advert::directory workerDir,
                saga::url serverURL_,
                LogWriter *log);
 ~HandleReduces();
  bool assignReduces();
 private:
  void issue_command_();
  std::vector<std::string> groupFiles_(int counter);
  void wait_for_results_();

  const JobDescription&    job_;
  std::map<std::string, saga::url>& committed_chunks_;
  int                      fileCount_;
  saga::advert::directory  workerDir_;
  saga::url                serverURL_;
  LogWriter               *log_;
  std::set<int> finished_;  // Finished partition's ID.
  std::vector<saga::url>   workers_;
  saga::stream::server    *service_;
  int                      currentPartition_;
};

} // namespace master
} // namespace mapreduce

#endif // MR_HANDLE_REDUCES_HPP
