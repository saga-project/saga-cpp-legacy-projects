//  Copyright (c) 2008 Michael Miceli and Christopher Miceli
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef UNORDERED_MAP
#define UNORDERED_MAP

#include <boost/shared_ptr.hpp>
#include <boost/unordered_map.hpp>
#include <boost/functional/hash.hpp>
#include <vector>
#include <string>

namespace MapReduce {
   typedef boost::shared_ptr<std::vector<std::string> > strVectorPtr;

   struct equal_to : std::binary_function<std::string, std::string, bool> {
      bool operator()(std::string const& x, std::string const& y) const {
         return x == y;
      }
   };
   typedef boost::unordered_map<std::string, strVectorPtr, boost::hash<std::string>, equal_to> unorderedMap;
}//namespace MapReduce

#endif
