/*
 *  logwriter.cpp
 *  FAUST - Framework for Adaptive Ubiquitous Scalable Tasks
 *
 *  Created by Ole Weidner <oweidner@cct.lsu.edu> on 11/23/08.
 *  Copyright 2008-2009 Center for Computation & Technology. 
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying 
 *  LICENSE file or copy at http://www.boost.org/LICENSE_1_0.txt)
 */


#include <time.h>
#include <iostream>

#include <faust/impl/logwriter.hpp>

///////////////////////////////////////////////////////////////////////////////
//
faust::detail::logwriter::logwriter(void)
: appName_(""), out_(std::cout)
{
  
}

///////////////////////////////////////////////////////////////////////////////
//
faust::detail::logwriter::logwriter(std::string appName)
: appName_(appName), out_(std::cout)
{
  
}

///////////////////////////////////////////////////////////////////////////////
//
faust::detail::logwriter::logwriter(std::string appName, std::ostream& logStream = std::cout)
: appName_(appName), out_(logStream)
{
  
}

///////////////////////////////////////////////////////////////////////////////
//

faust::detail::logwriter::~logwriter()
{
  
}

///////////////////////////////////////////////////////////////////////////////
//

void faust::detail::logwriter::write(std::string message, unsigned int logLevel)
{
  // generate timestamp string
  time_t rawTime; struct tm * timeInfo;
  time(&rawTime); timeInfo = localtime(&rawTime);
  out_ << timeInfo->tm_mon << "/" << timeInfo->tm_mday << "/" 
       << timeInfo->tm_year << " ";
  out_ << timeInfo->tm_hour << ":" << timeInfo->tm_min << ":" 
       << timeInfo->tm_sec << "\t ";
  
  // append application name
  out_ << appName_ << "\t ";
  
  // append debug level
  switch(logLevel) {
      
    case(LOGLEVEL_DEBUG):
      out_ << "[DEBUG]\t"; break;
      
    case(LOGLEVEL_INFO):
      out_ << "[INFO]\t"; break;
      
    case(LOGLEVEL_WARNING):
      out_ << "[WARNING]\t"; break;

    case(LOGLEVEL_ERROR):
      out_ << "[ERROR]\t"; break;
      
    case(LOGLEVEL_FATAL):
      out_ << "[FATAL]\t"; break;
  }
  
  //append message
  out_ << message << std::endl;
}
