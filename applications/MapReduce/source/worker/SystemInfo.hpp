//  Copyright (c) 2008 Michael Miceli and Christopher Miceli
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef MR_WORKER_SYSTEMINFO_HPP
#define MR_WORKER_SYSTEMINFO_HPP

#include <string>
#include <saga/saga.hpp>

namespace MapReduce {
   struct hwInfo {
      int      nCpu;
      int      cpuType;
      int      cpuSubtype;
      boost::uint64_t cpuFrequency;
      boost::uint64_t totalMemory;
   };
 
   class SystemInfo {
     private:
      std::string pathToUname_;
      std::string pathToWhich_;
      std::string pathToUptime_;
      
      std::string hostName_;
      std::string hostUname_;
      std::string hostLoadAverage_;
      
      hwInfo hardwareInfo_;
                  
      void detectHostName_();
      void detectHostUname_();
      void detectHostLoadAverage_();
      
      void gatherHardwareInfo_();
      
      std::string findExecutable_(std::string name);
     
     public:
      SystemInfo();
      ~SystemInfo();
     
      // Static system properties
      std::string hostName() { return hostName_; }
      std::string hostType() { return hostUname_; }
      hwInfo hardwareInfo()  { return hardwareInfo_; }
      
      // Dynamic system properties
      std::string hostLoadAverage();
   }; 

} // namespace MapReduce

#endif //MR_WORKER_SYSTEMINFO_HPP

