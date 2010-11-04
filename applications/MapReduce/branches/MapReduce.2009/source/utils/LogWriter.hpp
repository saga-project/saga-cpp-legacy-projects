//  Copyright (c) 2008 Michael Miceli and Christopher Miceli
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef MR_LOG_WRITER_HPP
#define MR_LOG_WRITER_HPP

#include <string>
#include <saga/saga.hpp>

#define MR_LOGLEVEL_FATAL     0x2   
#define MR_LOGLEVEL_ERROR     0x4  
#define MR_LOGLEVEL_WARNING   0x8   
#define MR_LOGLEVEL_INFO      0x16   
#define MR_LOGLEVEL_DEBUG     0x32  

namespace mapreduce
{
  class LogWriter {

  private:
    
    std::string appName_;
    saga::url   advURL_;
    
  public:
    
    LogWriter(std::string appName, saga::url advURL);
    LogWriter(saga::url advURL);
    
    ~LogWriter();
    
    void write(std::string, unsigned int logLevel);
    
  };
}

#endif // MR_LOG_WRITER_HPP 
