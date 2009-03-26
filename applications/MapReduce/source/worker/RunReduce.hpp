//  Copyright (c) 2008 Michael Miceli and Christopher Miceli
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef MR_RUN_REDUCE_HPP
#define MR_RUN_REDUCE_HPP

#include "unorderedMap.hpp"
#include <string>
#include <vector>
#include <saga/saga.hpp>
#include "version.hpp"

namespace MapReduce {
   class RunReduce {
     public:
      RunReduce(saga::advert::directory workerDir, saga::advert::directory reduceInputDir,
                std::string outputPrefix);
      ~RunReduce();
      void getLines(unorderedMap &keyValues);
      int  getCurrent(void);
     private:
      std::vector<std::string> files_;
      saga::advert::directory workerDir_;
      saga::advert::directory reduceInputDir_;
      std::string outputPrefix_;
   }; // class RunReduce
} // namespace MapReduceBase

#endif // MR_RUN_REDUCE_HPP

