//  Copyright (c) 2008 Michael Miceli and Christopher Miceli
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef AP_RUN_COMPARISON_HPP
#define AP_RUN_COMPARISON_HPP

#include <string>
#include <vector>
#include <saga/saga.hpp>
#include <boost/lexical_cast.hpp>
#include "../utils/LogWriter.hpp"
#include "version.hpp"

namespace AllPairs {
   class RunComparison {
     public:
      RunComparison(saga::advert::directory workerDir,
             AllPairs::LogWriter *log);
      ~RunComparison();
      std::vector<std::string> getComparisons();
     private:
      saga::advert::directory workerDir_;
      saga::url file_;
      AllPairs::LogWriter *log_;

   }; // class RunComparison
} // namespace AllPairs

#endif // MR_RUN_COMPARISON_HPP

