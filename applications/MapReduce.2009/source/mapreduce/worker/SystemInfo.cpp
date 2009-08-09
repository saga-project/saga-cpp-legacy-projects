//  Copyright (c) 2008 Michael Miceli and Christopher Miceli
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "SystemInfo.hpp"

#include <cstdlib>
#if defined(SAGA_APPLE)
   #include <sys/sysctl.h>
   #include <mach/machine.h>
#endif

using namespace saga;

namespace mapreduce { namespace worker {

SystemInfo::SystemInfo()
  : hostName_(""), hostUname_(""), hostLoadAverage_("") {
   //pathToWhich_  = "which";
   //pathToUname_  = findExecutable_("uname");
   //pathToUptime_ = findExecutable_("uptime");

   // These are static - won't change during instance lifetime
   gatherHardwareInfo_();
   detectHostName_();
   detectHostUname_();
}

SystemInfo::~SystemInfo() {

}

std::string SystemInfo::hostLoadAverage() { 
   detectHostLoadAverage_(); 
   return hostLoadAverage_;
}

void SystemInfo::detectHostName_() {
   #ifndef   MAX_PATH
   # if defined(SAGA_APPLE)
   #  define MAX_PATH PATH_MAX
   # else
   #  define MAX_PATH _POSIX_PATH_MAX
   # endif
   #endif
   char buffer[MAX_PATH] = { '\0' };
   gethostname(buffer, sizeof(buffer));
   hostName_ = std::string(buffer);
}

void SystemInfo::gatherHardwareInfo_() {
   #if defined(SAGA_APPLE)
      saga::size_t len;
      len = sizeof(hardwareInfo_.totalMemory);
      sysctlbyname("hw.memsize", &hardwareInfo_.totalMemory, &len, NULL, 0);
      len = sizeof(hardwareInfo_.nCpu);
      sysctlbyname("hw.ncpu", &hardwareInfo_.nCpu, &len, NULL, 0);
      len = sizeof(hardwareInfo_.cpuFrequency);
      sysctlbyname("hw.cpufrequency_max", &hardwareInfo_.cpuFrequency, &len, NULL, 0);
      len = sizeof(hardwareInfo_.cpuType);
      sysctlbyname("hw.cputype", &hardwareInfo_.cpuType, &len, NULL, 0);
      len = sizeof(hardwareInfo_.cpuSubtype);
      sysctlbyname("hw.cpusubtype", &hardwareInfo_.cpuSubtype, &len, NULL, 0);
   #endif
}

void SystemInfo::detectHostUname_() {
   job::ostream in;
   job::istream out;
   job::istream err;
     
   if(pathToUname_.length() > 0) {
      try {
         job::service js(saga::url("localhost"));
         job::job j1 = js.run_job (pathToUname_, "localhost", in, out, err);
         
         job::state state = j1.get_state ();
         if(state != job::Running && state != job::Done) {
            //TODO - something went wrong
         }
      }
      catch (std::exception const & e) {
         std::cout << "error: " << e.what() << std::endl;
      }

      while(true) {
         char buffer[255];
         out.read (buffer, sizeof (buffer));
         if(out.gcount() > 0) {
            hostUname_ += std::string (buffer, out.gcount ());
         }
         if(out.fail()) {
            break;
         }
      }
   }
   if(hostUname_.length() > 0 && hostUname_[hostUname_.length()-1] == '\n') {
      hostUname_ = hostUname_.substr(0,hostUname_.length()-1);
   }
}

void SystemInfo::detectHostLoadAverage_() {
   job::ostream in;
   job::istream out;
   job::istream err;
   
   if(pathToUptime_.length() > 0) {
      try {
         job::service js(saga::url("localhost"));
         job::job j1 = js.run_job (pathToUptime_, "localhost", in, out, err);
        
         job::state state = j1.get_state ();
         if(state != job::Running && state != job::Done) {
           //TODO - something went wrong
         }
      }
      catch (std::exception const & e) {
         std::cout << "error: " << e.what() << std::endl;
      }
      while(true) {
         char buffer[255];
         out.read (buffer, sizeof (buffer));
           
         if(out.gcount() > 0) {
            hostLoadAverage_ += std::string (buffer, out.gcount ());
         }
         if(out.fail()) {
            break;
         }
      }
   }
   if(hostLoadAverage_.length() > 0) {
      hostLoadAverage_ = hostLoadAverage_.substr(hostLoadAverage_.find("es: ")+4);
   }
}

std::string SystemInfo::findExecutable_(std::string name) {
   std::string findCommand(pathToWhich_);
   findCommand += " "; findCommand += name;
   job::ostream in;
   job::istream out;
   job::istream err;
   
   try {
      job::service js(saga::url("localhost"));
      job::job j1 = js.run_job (findCommand, "localhost", in, out, err);
      
      job::state state = j1.get_state ();
      if(state != job::Running && state != job::Done) {
        //TODO - something went wrong
      }
   }
   catch (std::exception const & e) {
      std::cout << "error: " << e.what() << std::endl;
   }
   std::string path("");
   while(true) {
      char buffer[255];
      out.read (buffer, sizeof (buffer));
      if (out.gcount() > 0) {
         path += std::string (buffer, out.gcount ());
      }
      if(out.fail()) {
         break;
      }
   }
   return path;
}

} // namespace worker
} // namespace mapreduce
