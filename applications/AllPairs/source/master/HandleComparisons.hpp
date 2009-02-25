//  Copyright (c) 2008 Michael Miceli and Christopher Miceli
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef AP_HANDLE_COMPARISONS_HPP
#define AP_HANDLE_COMPARISONS_HPP

#include <string>
#include <saga/saga.hpp>
#include "../utils/LogWriter.hpp"
#include "version.hpp"
#include <vector>

namespace AllPairs {
   typedef std::pair<std::string, std::string> assignment;
   typedef std::vector<assignment> assignmentChunk;
   typedef std::vector<assignmentChunk> assignmentChunksVector;
   class HandleComparisons {
     public:
      HandleComparisons(assignmentChunksVector &assignments, saga::url serverURL_, LogWriter *log);
      ~HandleComparisons();
      void assignWork();
     private:
      void issueCommand_();
      assignmentChunk getChunk_();
    
      std::vector<int> finished_;
      std::vector<int> assigned_;
      std::vector<int> unassigned_;
      assignmentChunksVector   assignments_;
      saga::stream::server    *service_;
      saga::advert::directory  workerDir_;
      saga::url serverURL_;
      LogWriter *log_;
   };
} //Namespace AllPairs

#endif // AP_HANDLE_COMPARISONS_HPP

