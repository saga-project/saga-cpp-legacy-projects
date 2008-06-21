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

#ifndef TF_FRONTEND_SUBMITAPP_HPP
#define TF_FRONTEND_SUBMITAPP_HPP

#include <saga/saga.hpp>
#include "ConfigFileParser.hpp"
#include "LogWriter.hpp"

namespace TaskFarmer 
{
  namespace Frontend 
  {
    class SubmitApp 
      {
      private:
        
        time_t startupTime_;
        
        std::string uuid_;
        std::string database_;
        
        saga::advert::directory sessionBaseDir_;
        saga::advert::directory agentsDir_;
        saga::advert::directory binariesDir_;
        saga::advert::directory chunksDir_;
        
        TaskFarmer::LogWriter * log;
        ConfigFileParser cfgFileParser_;
        
      protected:
        
        void registerWithDB_(void);
        void createNewSession_(void);
        
        void populateBinariesList_(void);
        void populateChunkList_(void);
        
        void spawnAgents_(void);
        
      public:
        
        SubmitApp(std::string configFilePath);
        ~SubmitApp(void);
        
        void run();
      }; 
  } //namespace Frontend
} //namespace TaskFarmer

#endif //TF_FRONTEND_TFAGENTAPP_HPP
