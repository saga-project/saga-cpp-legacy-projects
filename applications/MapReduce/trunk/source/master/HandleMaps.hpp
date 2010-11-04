//  Copyright (c) 2008 Michael Miceli and Christopher Miceli
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef MR_HANDLE_MAPS_HPP
#define MR_HANDLE_MAPS_HPP

#include <string>
#include <vector>
#include <saga/saga.hpp>
#include "../utils/LogWriter.hpp"
#include "version.hpp"

namespace MapReduce {
   class HandleMaps {
     public:
      HandleMaps(std::vector<saga::url> &chunks,
                 saga::url serverURL,
                 LogWriter *log);
     ~HandleMaps();
      bool assignMaps();

     private:
      void issue_command_();
      std::string getCandidate_();

      saga::stream::server    *service_;
      std::vector<std::string> unassigned_;
      std::vector<std::string> assigned_;
      std::vector<std::string> finished_;
      saga::url                serverURL_;
      LogWriter               *log_;
      std::vector<std::string>::size_type totalChunks_;
   };
} //Namespace MapReduce

#endif // MR_HANDLE_MAPS_HPP

