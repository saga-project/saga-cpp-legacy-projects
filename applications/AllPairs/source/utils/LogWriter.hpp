//  Copyright (c) 2008 Michael Miceli and Christopher Miceli
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef AP_LOG_WRITER_HPP
#define AP_LOG_WRITER_HPP

#include <string>
#include <saga/saga.hpp>

namespace AllPairs
{
  class LogWriter {

  private:
    
    std::string appName_;
    std::ostream & out_;
    
  public:
    
    LogWriter();
    LogWriter(std::string appName);
    LogWriter(std::string appName, std::ostream& logStream);
    
    ~LogWriter();
    
    void write(std::string, unsigned int logLevel);
    
  };
}

#endif // AP_LOG_WRITER_HPP 
