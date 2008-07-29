//  Copyright (c) 2008 Michael Miceli and Christopher Miceli
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef GET_ELEMENTS_HPP
#define GET_ELEMENTS_HPP

#include "type.hpp"

namespace AllPairs {
      std::vector<std::string> getElements(saga::url f) {
      std::string elem;
      std::vector<std::string> elems;
      unsigned int offset       = 0;
      unsigned int duration     = 0; //Read all
      unsigned int elementsRead = 0;
      unsigned int pos          = 0;
      boost::iostreams::stream <saga_file_device> file_stream (f.get_string());
      while(pos < offset) { //Throw away unneeded data
         file_stream >> elem;
      }
      if(duration == 0) {
         while(file_stream >> elem) {
            elems.push_back(elem);
         }
      }
      else { 
         while(elementsRead < duration && file_stream >> elem) {
            elementsRead++;
            elems.push_back(elem);
         }
      }
      return elems;
   }
}

#endif // GET_ELEMENTS_HPP

