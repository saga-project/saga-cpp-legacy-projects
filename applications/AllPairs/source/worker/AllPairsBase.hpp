//  Copyright (c) 2008 Michael Miceli and Christopher Miceli
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef AP_ALLPAIRSBASE_HPP
#define AP_ALLPAIRSBASE_HPP

#include <vector>
#include <string>
#include <time.h>
#include <saga/saga.hpp>
#include <boost/lexical_cast.hpp>
#include "../utils/LogWriter.hpp"
#include "../utils/defines.hpp"
#include "SystemInfo.hpp"

namespace AllPairs {
   class AllPairsBase {
     public:
      AllPairsBase(int argCount,char **argList);
      int run(void);
      virtual      ~AllPairsBase() {}
      virtual void compare(std::string object1, std::string object2) = 0;
//     protected:
      void         emit(bool result);
     private:
      std::string uuid_;
      saga::url file_;
      std::string sessionUUID_;
      std::string logURL_;
      std::string database_;
   
      time_t startupTime_;
      SystemInfo systemInfo_;
   
      saga::advert::directory workerDir_;
      saga::advert::directory sessionBaseDir_;
      AllPairs::LogWriter * logWriter_;
   
      void updateStatus_(void);
      void cleanup_(void);
      void registerWithDB(void);
      void mainLoop(unsigned int updateInterval);
      std::string getFrontendCommand_(void);
   };
}

#endif // AP_ALLPAIRSBASE_HPP

