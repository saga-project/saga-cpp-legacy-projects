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

#ifndef TF_AGENT_SYSTEMINFO_HPP
#define TF_AGENT_SYSTEMINFO_HPP

#include <string>
#include <saga/saga.hpp>

namespace TaskFarmer 
{
  namespace Agent 
  {
    typedef struct hwInfo 
    {
      int      nCpu;
      int      cpuType;
      int      cpuSubtype;
      uint64_t cpuFrequency;
      uint64_t totalMemory;
    };

    class SystemInfo 
    {
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
  }// namespace Agent
}// namespace TaskFarmer

#endif //TF_AGENT_SYSTEMINFO_HPP

