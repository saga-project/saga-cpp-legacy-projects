// The MIT License
//
// Copyright (c) 2007 Ole Weidner (oweidner@cct.lsu.edu)

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include "SystemInfo.hpp"

#include <cstdlib>
#if defined(SAGA_APPLE)
  #include <sys/sysctl.h>
  #include <mach/machine.h>
#endif


using namespace saga;
using namespace TaskFarmer::Agent;

///////////////////////////////////////////////////////////////////////////////
//
SystemInfo::SystemInfo()
  : hostName_(""), hostUname_(""), hostLoadAverage_("")
{
  pathToWhich_  = "which";
  pathToUname_  = findExecutable_("uname");
  pathToUptime_ = findExecutable_("uptime");

  // These are static - won't change during instance lifetime
  gatherHardwareInfo_();
  detectHostName_();
  detectHostUname_();
}

///////////////////////////////////////////////////////////////////////////////
//
SystemInfo::~SystemInfo()
{
}

///////////////////////////////////////////////////////////////////////////////
//
std::string SystemInfo::hostLoadAverage()
{ 
  detectHostLoadAverage_(); 
  return hostLoadAverage_;
}

///////////////////////////////////////////////////////////////////////////////
//
void SystemInfo::detectHostName_()
{
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

///////////////////////////////////////////////////////////////////////////////
//
void SystemInfo::gatherHardwareInfo_()
{
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

///////////////////////////////////////////////////////////////////////////////
//
void SystemInfo::detectHostUname_()
{
  job::ostream in;
  job::istream out;
  job::istream err;
    
  if(pathToUname_.length() > 0)
  {
    try {
      job::service js(saga::url("localhost"));
      job::job j1 = js.run_job (pathToUname_, "localhost", in, out, err);
      
      job::state state = j1.get_state ();
      if(state != job::Running && state != job::Done)
      {
        //TODO - something went wron
      }
    }
    catch (std::exception const & e) {
      std::cout << "error: " << e.what() << std::endl;
    }
  
    while ( true ) {
      char buffer[255];
      out.read (buffer, sizeof (buffer));
        
      if ( out.gcount () > 0 ) {
        hostUname_ += std::string (buffer, out.gcount ());
      }
      if ( out.fail () ) {
        break;
      }
    }
  }
  if(hostUname_.length() > 0 && hostUname_[hostUname_.length()-1] == '\n') 
    hostUname_ = hostUname_.substr(0,hostUname_.length()-1);
}

///////////////////////////////////////////////////////////////////////////////
//
void SystemInfo::detectHostLoadAverage_()
{
  job::ostream in;
  job::istream out;
  job::istream err;
  
  if(pathToUptime_.length() > 0)
  {
    try {
      job::service js(saga::url("localhost"));
      job::job j1 = js.run_job (pathToUptime_, "localhost", in, out, err);

      job::state state = j1.get_state ();
      if(state != job::Running && state != job::Done)
      {
        //TODO - something went wrong
      }
    }
    catch (std::exception const & e) {
      std::cout << "error: " << e.what() << std::endl;
    }

    while ( true ) {
      char buffer[255];
      out.read (buffer, sizeof (buffer));
        
      if ( out.gcount () > 0 ) {
        hostLoadAverage_ += std::string (buffer, out.gcount ());
      }
      if ( out.fail () ) {
        break;
      }
    }
  }
  if(hostLoadAverage_.length() > 0) 
    hostLoadAverage_ = hostLoadAverage_.substr(hostLoadAverage_.find("es: ")+4);
}
    
///////////////////////////////////////////////////////////////////////////////
//
std::string SystemInfo::findExecutable_(std::string name)
{
  std::string findCommand(pathToWhich_);
  findCommand += " "; findCommand += name;
  
  job::ostream in;
  job::istream out;
  job::istream err;
  
  try {
    job::service js(saga::url("localhost"));
    job::job j1 = js.run_job (findCommand, "localhost", in, out, err);

    job::state state = j1.get_state ();
    if(state != job::Running && state != job::Done)
    {
      //TODO - something went wron
    }
  }
  catch (std::exception const & e) {
    std::cout << "error: " << e.what() << std::endl;
  }
  
  std::string path("");
  while ( true ) {
    char buffer[255];
    out.read (buffer, sizeof (buffer));
      
    if ( out.gcount () > 0 ) {
      path += std::string (buffer, out.gcount ());
    }
    if ( out.fail () ) {
      break;
    }
  }
  return path;
}
