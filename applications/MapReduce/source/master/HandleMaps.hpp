//  Copyright (c) 2008 Michael Miceli and Christopher Miceli
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef MR_HANDLE_MAPS_HPP
#define MR_HANDLE_MAPS_HPP

#include <string>
#include <vector>
#include <saga/saga.hpp>
#include "version.hpp"

namespace MapReduce {
   class HandleMaps {
     public:
      HandleMaps(std::vector<saga::url> &chunks, saga::advert::directory workerDir);
      bool assignMaps();

     private:
      void issue_command_(std::string file);
      std::string getCandidate_();
      std::vector<std::string> finished_;
      std::vector<saga::url>::const_iterator candidateIT_;
      std::vector<saga::url> chunks_;
      std::vector<saga::url> workers_;
      saga::advert::directory workerDir_;
   };
} //Namespace MapReduce

#endif // MR_HANDLE_MAPS_HPP

