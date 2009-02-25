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
   typedef std::pair<std::string, std::string> assignment;
   typedef std::vector<assignment> assignmentChunk;
   typedef std::vector<assignmentChunk> assignmentChunksVector;
   class RunComparison {
     public:
      RunComparison(saga::stream::stream server, AllPairs::LogWriter *log);
      ~RunComparison();
      assignment getAssignment();
      assignmentChunk getAssignmentChunk();
      int getChunkID();
      bool hasAssignment();

     private:
      assignmentChunk::iterator chunk_IT;
      assignmentChunk           chunk_;
      int                       chunkID_;
      saga::stream::stream      server_;

      AllPairs::LogWriter *log_;
   }; // class RunComparison
} // namespace AllPairs

#endif // MR_RUN_COMPARISON_HPP

