//  Copyright (c) 2008 Michael Miceli and Christopher Miceli
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef AP_HANDLE_COMPARISONS_HPP
#define AP_HANDLE_COMPARISONS_HPP

#include <string>
#include <vector>
#include <saga/saga.hpp>
#include "../utils/defines.hpp"
#include "version.hpp"

namespace AllPairs {
   class HandleComparisons {
     public:
      HandleComparisons(saga::advert::directory workerDir);
      bool assignWork();
     private:
      void issue_command_();
    
      std::vector<saga::advert::directory> finished_;
      std::vector<saga::url> workers_;
      saga::advert::directory workerDir_;
      unsigned long comparisons_;
   };
} //Namespace AllPairs

#endif // AP_HANDLE_COMPARISONS_HPP

