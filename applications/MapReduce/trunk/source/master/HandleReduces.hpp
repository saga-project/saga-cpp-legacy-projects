//  Copyright (c) 2008 Michael Miceli and Christopher Miceli
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef MR_HANDLE_REDUCES_HPP
#define MR_HANDLE_REDUCES_HPP

#include <string>
#include <vector>
#include <saga/saga.hpp>
//#include "../utils/defines.hpp"
#include "../utils/LogWriter.hpp"
#include "version.hpp"

namespace MapReduce {
   class HandleReduces {
     public:
      HandleReduces(int fileCount,
                    saga::advert::directory workerDir,
                    saga::url serverURL_,
                    LogWriter *log);
     ~HandleReduces();
      bool assignReduces();
     private:
      void issue_command_();
      std::vector<std::string> groupFiles_(int counter);
      void wait_for_results_();
    
      int                      fileCount_;
      saga::advert::directory  workerDir_;
      saga::url                serverURL_;
      LogWriter               *log_;
      std::vector<std::string> finished_;
      std::vector<saga::url>   workers_;
      saga::stream::server    *service_;
      int                      currentCount;
   };
} //Namespace MapReduce

#endif // MR_HANDLE_REDUCES_HPP

