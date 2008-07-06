//  Copyright (c) 2008 Michael Miceli and Christopher Miceli
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <string>
#include <vector>
#include "type.hpp"

#ifndef AP_SORTER_HPP
#define AP_SORTER_HPP 

namespace AllPairs {
   template <class T>
   std::string sorter(std::vector<std::string> fileList) {
      while(fileList.size() > 0) {
         boost::iostreams::stream <saga_file_device> list(fileList.back());
         fileList.pop_back();
      }
      return "mapFile-merged";
   }
} // namespace AllPairs

#endif // AP_SORTER_HPP
