//  Copyright (c) 2008 Michael Miceli and Christopher Miceli
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef MF_MAPREDUCEBASE_HPP
#define MF_MAPREDUCEBASE_HPP

#include <vector>
#include <string>
#include <time.h>
#include <saga/saga.hpp>
#include <boost/lexical_cast.hpp>
#include "../utils/LogWriter.hpp"
#include "../utils/defines.hpp"
#include "RunMap.hpp"
#include "RunReduce.hpp"
#include "SystemInfo.hpp"

namespace MapReduce {
   class MapReduceBase {
     public:
      int init(int argCount,char **argList);
      virtual      ~MapReduceBase() {}
     protected:
      virtual void map(std::string chunkName) = 0;
      virtual void reduce(std::string key, std::vector<std::string> values) = 0;
      virtual int  hash(std::string input,unsigned int limit);
      void         emitIntermediate(std::string key, std::string value);
      void         emit(std::string key, std::string value);
     private:
      std::string uuid_;
      std::string sessionUUID_;
      std::string database_;
   
      time_t startupTime_;
      SystemInfo systemInfo_;
   
      saga::advert::directory workerDir_;
      saga::advert::directory intermediateDir_;
      saga::advert::directory chunksDir_;
      saga::advert::directory reduceInputDir_;
      MapReduce::LogWriter * logWriter_;
   
      void updateStatus_(void);
      void cleanup_(void);
      void registerWithDB(void);
      void mainLoop(unsigned int updateInterval);
      void run(void);
      std::string getFrontendCommand_(void);
   };
}

#endif //MF_MAPREDUCEBASE_HPP

