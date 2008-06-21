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

#ifndef TF_AGENT_HEALTH_MANAGER_HPP
#define TF_AGENT_HEALTH_MANAGER_HPP

#include <string>
#include <saga/saga.hpp>

namespace TaskFarmer 
{
  namespace Agent 
  {
    class HealthManager {
    
    private:
      saga::advert::directory * sessionBaseDir_;
      saga::advert::directory * agentBaseDir_;
      
      void startElection_();
      void runAsLeader_();
      void runAsSlave_();
      
    public:
      HealthManager();
      HealthManager(saga::advert::directory & sessionBaseDir,
                    saga::advert::directory & agentBaseDir);
      
      ~HealthManager();
    
    };
  }
}

#endif // TF_AGENT_HEALTH_MANAGER_HPP
