//  Copyright (c) 2008 Michael Miceli and Christopher Miceli
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <time.h>
#include <ostream>

#include "defines.hpp"
#include "LogWriter.hpp"

using namespace saga;
using namespace AllPairs;

LogWriter::LogWriter(std::string appName, saga::url advURL)
: appName_(appName), advURL_(advURL)
{
  
}

LogWriter::LogWriter(saga::url advURL)
: advURL_(advURL)
{
  
}

LogWriter::~LogWriter()
{
  
}

void LogWriter::write(std::string message, unsigned int logLevel)
{
  // generate timestamp string
  int mode = advert::ReadWrite | advert::CreateParents;
  SAGA_OSSTREAM adv;

  try
  {
    saga::advert::entry log(advURL_, mode);
    
    time_t rawTime; struct tm * timeInfo;
    time(&rawTime); timeInfo = localtime(&rawTime);
    adv << timeInfo->tm_mon << "/" << timeInfo->tm_mday << "/" 
         << timeInfo->tm_year << " ";
    adv << timeInfo->tm_hour << ":" << timeInfo->tm_min << ":" 
         << timeInfo->tm_sec << "\t ";
    
    // append application name
    adv << appName_ << "\t ";
    
    // append debug level
    switch(logLevel) {
        
      case(LOGLEVEL_DEBUG):
        adv << "[DEBUG]\t"; break;
        
      case(LOGLEVEL_INFO):
        adv << "[INFO]\t"; break;
        
      case(LOGLEVEL_WARNING):
        adv << "[WARNING]\t"; break;
 
      case(LOGLEVEL_ERROR):
        adv << "[ERROR]\t"; break;
        
      case(LOGLEVEL_FATAL):
        adv << "[FATAL]\t"; break;
    }
    //append message
    adv << message << std::endl;
    std::string message(SAGA_OSSTREAM_GETSTRING(adv));
    log.store_string(message);
    std::cout << message;
  }
  catch (saga::exception const& e) {
     std::cerr << "caught saga::exception while logging: " << e.what() << std::endl;
  }
}
