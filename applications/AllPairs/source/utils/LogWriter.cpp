//  Copyright (c) 2008 Michael Miceli and Christopher Miceli
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

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
  //int mode = advert::ReadWrite | advert::CreateParents;
  SAGA_OSSTREAM adv;

  try
  {
    //saga::advert::entry log(advURL_, mode);
    time_t rawTime; struct tm * timeInfo;
    time(&rawTime); timeInfo = localtime(&rawTime);
    std::string tm_sec, tm_mon, tm_mday, tm_hour, tm_min;
    if(timeInfo->tm_mon < 10) {
       tm_mon = "0";
       tm_mon += boost::lexical_cast<std::string>(timeInfo->tm_mon);
    }
    else {
       tm_mon = boost::lexical_cast<std::string>(timeInfo->tm_mon);
    }
    if(timeInfo->tm_mday < 10) {
       tm_mday= "0";
       tm_mday += boost::lexical_cast<std::string>(timeInfo->tm_mday);
    }
    else {
       tm_mday = boost::lexical_cast<std::string>(timeInfo->tm_mday);
    }
    if(timeInfo->tm_hour < 10) {
       tm_hour = "0";
       tm_hour += boost::lexical_cast<std::string>(timeInfo->tm_hour);
    }
    else {
       tm_hour = boost::lexical_cast<std::string>(timeInfo->tm_hour);
    }
    if(timeInfo->tm_min< 10) {
       tm_min= "0";
       tm_min+= boost::lexical_cast<std::string>(timeInfo->tm_min);
    }
    else {
       tm_min = boost::lexical_cast<std::string>(timeInfo->tm_min);
    }
    if(timeInfo->tm_sec < 10) {
       tm_sec = "0";
       tm_sec += boost::lexical_cast<std::string>(timeInfo->tm_sec);
    }
    else {
       tm_sec = boost::lexical_cast<std::string>(timeInfo->tm_sec);
    }
    adv << tm_mon << "/" << tm_mday << "/" 
         << (timeInfo->tm_year+1900) << " ";
    adv << tm_hour << ":" << tm_min << ":" 
         << tm_sec << "\t ";
    
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
    std::string report = SAGA_OSSTREAM_GETSTRING(adv);
    //log.store_string(report);
    std::cerr << report;
  }
  catch (saga::exception const& e) {
     std::cerr << "caught saga::exception while logging: " << e.what() << std::endl;
  }
}
