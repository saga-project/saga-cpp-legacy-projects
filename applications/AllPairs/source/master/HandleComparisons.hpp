//  Copyright (c) 2008 Michael Miceli and Christopher Miceli
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef AP_HANDLE_COMPARISONS_HPP
#define AP_HANDLE_COMPARISONS_HPP

#include <string>
#include <vector>
#include <saga/saga.hpp>
#include <boost/lexical_cast.hpp>
#include "../utils/LogWriter.hpp"
#include "../utils/network.hpp"
#include "../utils/Graph.hpp"
#include "../utils/defines.hpp"
#include "Assignment.hpp"

namespace AllPairs {
   class HandleComparisons {
     public:
      HandleComparisons(Graph &networkGraph,
                        assignmentChunksVector &assignments,
                        std::map<int, std::vector<saga::url> > &files,
                        const saga::url serverURL_, LogWriter *log);
      ~HandleComparisons();
      void assignWork();
     private:
      void                             issueCommand_();
      AssignmentChunk                  getChunk_(const std::string &location);
      std::pair<AssignmentChunk, bool> AssignmentChunkCheck_(const std::vector<int> &set,
                                                             const std::string &hostname);
    
      std::vector<int> finished_;
      std::vector<int> assigned_;
      std::vector<int> unassigned_;
      Graph            networkGraph_;
      assignmentChunksVector   assignments_;
      std::map<int, std::vector<saga::url> > files_;
      saga::stream::server    *service_;
      saga::advert::directory  workerDir_;
      saga::url serverURL_;
      LogWriter *log_;
   };
} //Namespace AllPairs

#endif // AP_HANDLE_COMPARISONS_HPP

