//  Copyright (c) 2008 Michael Miceli and Christopher Miceli
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef MP_MERGER_HPP 
#define MP_MERGER_HPP 

#include <saga/saga.hpp>
#include "type.hpp"
#include <vector>

namespace MapReduce {
   template <class T>
   std::vector<std::string> merger(std::vector<std::string> fileList) {
      std::vector<std::string> list;
      while(fileList.size() > 0) {
         boost::iostreams::stream <saga_file_device> in (fileList.back());
         char line[1000];
         while(in.getline(line,1000)) {
            list.push_back(line);
         }
         fileList.pop_back();
      }
      std::sort(list.begin(),list.end());
      return list;
   }
} // namespace MapReduce

#endif

