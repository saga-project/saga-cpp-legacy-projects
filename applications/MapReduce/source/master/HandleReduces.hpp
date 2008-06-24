//  Copyright (c) 2008 Michael Miceli and Christopher Miceli
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef MR_HANDLE_REDUCES_HPP
#define MR_HANDLE_REDUCES_HPP

#include <string>
#include <vector>
#include <saga/saga.hpp>
#include "../utils/defines.hpp"
#include "version.hpp"

namespace MapReduce {
   class HandleReduces {
     public:
      HandleReduces(int fileCount, std::vector<saga::url> workers);
      bool assignReduces();
     private:
      void issue_command_(std::vector<std::string> inputs);
      std::vector<std::string> groupFiles_(int counter);
    
      std::vector<std::string> finished_;
      int fileCount_;
      std::vector<saga::url> workers_;
   };
} //Namespace MapReduce

#endif // MR_HANDLE_REDUCES_HPP

