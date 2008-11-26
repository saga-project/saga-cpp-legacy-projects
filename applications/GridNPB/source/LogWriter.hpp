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

#ifndef GRIDNPB_LOGWRITER_HPP
#define GRIDNPB_LOGWRITER_HPP

#define LOGLEVEL_FATAL     0x2   
#define LOGLEVEL_ERROR     0x4  
#define LOGLEVEL_WARNING   0x8   
#define LOGLEVEL_INFO      0x16   
#define LOGLEVEL_DEBUG     0x32  

#include <string>
#include <saga/saga.hpp>

namespace GridNPB 
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

#endif // TF_LOG_WRITER_HPP

