//  Copyright (c) 2008 Michael Miceli and Christopher Miceli
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef MR_RUN_MAP_HPP
#define MR_RUN_MAP_HPP

#include <string>
#include <vector>
#include <saga/saga.hpp>
#include <boost/lexical_cast.hpp>
#include "../utils/defines.hpp"
#include "version.hpp"

namespace MapReduce {
   class RunMap {
     public:
      RunMap(saga::advert::directory workerDir,
             saga::advert::directory chunksDir,
             saga::advert::directory intermediateDir);
      ~RunMap();
      saga::url getFile();
     private:
      saga::advert::directory workerDir_;
      saga::advert::directory chunksDir_;
      saga::advert::directory intermediateDir_;
   }; // class RunMap
} // namespace MapReduce

#endif // MR_RUN_MAP_HPP

