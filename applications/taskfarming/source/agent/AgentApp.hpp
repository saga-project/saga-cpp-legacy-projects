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

#ifndef TF_AGENT_TFAGENTAPP_HPP
#define TF_AGENT_TFAGENTAPP_HPP

#include <time.h>
#include <string>
#include <saga/saga.hpp>

#include "SystemInfo.hpp"
#include "../LogWriter.hpp"

namespace TaskFarmer 
{
  namespace Agent 
  {
    class AgentApp 
    {
      private:
        std::string uuid_;
        std::string sessionUUID_;
        std::string database_;
        
        time_t startupTime_;
        SystemInfo systemInfo_;
        
        saga::advert::directory agentAdvertDir_;
        TaskFarmer::LogWriter * logWriter_;
        
        void updateStatus_();
        void registerWithDB_();
        void mainLoop_(unsigned int updateInterval);
        
        void cleanup_();
        
        int getFrontendCommand_();
        
      public:
        AgentApp(std::string sessionUUID, std::string database);
        ~AgentApp();
        
        void run();
      }; 
  }// namespace Agent
}// namespace TaskFarmer

#endif //TF_AGENT_TFAGENTAPP_HPP

