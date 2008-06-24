//  Copyright (c) 2008 Michael Miceli and Christopher Miceli
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef MR_PARSE_MAP_LINE_HPP
#define MR_PARSE_MAP_LINE_HPP

#include <sstream>
#include <vector>
#include <string>
#include <iostream>

namespace MapReduce {
   std::string getKey(std::string line) {
      std::string::size_type pos = line.find(' ');
      return line.substr(0,pos);
   }
   std::vector<std::string> parseMapLine(std::string line) {
      std::vector<std::string> retValues;
      for(std::string::size_type x=0;x<line.size();x++) {
         std::string::size_type pos1 = line.find(' ', x);
         std::string::size_type pos2 = line.find(',', pos1);
         if(pos1 != std::string::npos && pos2 != std::string::npos && pos1+1 < line.size() ) {
             retValues.push_back(line.substr(pos1+1,pos2-pos1-1));
             x = pos2;
          }
          else {
             pos2 = line.find(';', pos1);
             if(pos1 != std::string::npos && pos2 != std::string::npos && pos1+1 < line.size() ) {
                retValues.push_back(line.substr(pos1+1,pos2-pos1-1));
                x = pos2;
             }
          }
      }
      return retValues;
   }
} // namespace MapReduce

#endif

