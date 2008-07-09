//  Copyright (c) 2008 Michael Miceli and Christopher Miceli
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef MR_MASTER_HPP
#define MR_MASTER_HPP

#include <saga/saga.hpp>
#include "ConfigFileParser.hpp"
#include "../utils/LogWriter.hpp"

/***********************************
 * This is the Master class that   *
 * handles all details of the      *
 * framework.                      *
 * ********************************/
namespace MapReduce {
   namespace Master {
      class Master {
        private:
         time_t startupTime_;
         std::string uuid_;
         std::string database_;
         saga::url   logURL_;
        
         std::vector<std::string> fileChunks_;
         
         saga::advert::directory sessionBaseDir_;
         saga::advert::directory workersDir_;
         saga::advert::directory binariesDir_;
         saga::advert::directory chunksDir_;
         
         MapReduce::LogWriter * log;
         ConfigFileParser cfgFileParser_;
         
        protected:
         void registerWithDB_  (void);
         void createNewSession_(void);
        
         void populateBinariesList_(void);
         void populateFileList_    (void);
         
         void spawnAgents_(void);
 
         void runMaps_   (void);
         void sort_      (void);
         void runReduces_(void);
 
        public:
         Master(int argC, char **argV);
         void run();
         ~Master(void);
      }; 
   } // namespace Master
} // namespace MapReduce

#endif //MR_MASTER_HPP

