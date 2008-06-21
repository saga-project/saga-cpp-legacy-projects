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

#include <time.h>
#include <ostream>

#include "defines.hpp"
#include "LogWriter.hpp"

using namespace saga;
using namespace TaskFarmer;

///////////////////////////////////////////////////////////////////////////////
//
LogWriter::LogWriter(void)
: appName_(""), out_(std::cout)
{
  
}

///////////////////////////////////////////////////////////////////////////////
//
LogWriter::LogWriter(std::string appName)
: appName_(appName), out_(std::cout)
{
  
}

///////////////////////////////////////////////////////////////////////////////
//
LogWriter::LogWriter(std::string appName, std::ostream& logStream = std::cout)
: appName_(appName), out_(logStream)
{
  
}

///////////////////////////////////////////////////////////////////////////////
//

LogWriter::~LogWriter()
{
  
}

///////////////////////////////////////////////////////////////////////////////
//

void LogWriter::write(std::string message, unsigned int logLevel)
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
