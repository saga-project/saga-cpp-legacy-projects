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

#include "../defines.hpp"
#include "version.hpp"

#include "AgentApp.hpp"
#include "SystemInfo.hpp"

#include <boost/lexical_cast.hpp>

using namespace saga;
using namespace TaskFarmer::Agent;

///////////////////////////////////////////////////////////////////////////////
//
AgentApp::AgentApp(std::string sessionUUID, std::string database)
  : sessionUUID_(sessionUUID), database_(database)
{
  // create new LogWriter instance that writes to stdout
  logWriter_ = new TaskFarmer::LogWriter(std::string(TF_AGENT_EXE_NAME), 
                                         std::cout);
  
  // generate a startup timestamp 
  time(&startupTime_);
  
  // create a UUID for this agent
  uuid_ = uuid().string();
  
  // LOG_MESSAGE (extract later)
  std::cout << "Starting "+std::string(TF_AGENT_EXE_NAME) << " " 
            << TF_AGENT_VERSION_FULL << " (" <<  uuid_ << "): " << std::flush;
  // more stuff that can fail goes here...
  std::cout << "SUCCESSFUL" << std::endl;
  
  std::cout << "\tOrchestratorDB = " << database_ << std::endl
            << "\tSession UUID   = " << sessionUUID_ << std::endl
            << "\tStartup Time   = " << ctime(&startupTime_) << std::endl;
  
  // launch a SystemInfo instance
  systemInfo_ = SystemInfo();
}

///////////////////////////////////////////////////////////////////////////////
//
AgentApp::~AgentApp()
{
  delete logWriter_;
}

///////////////////////////////////////////////////////////////////////////////
//
void AgentApp::run()
{
  // get some environment information
  
  // register with the db
  registerWithDB_();
  
  // run the main loop
  mainLoop_(5);
}

///////////////////////////////////////////////////////////////////////////////
//
void AgentApp::registerWithDB_()
{ 
  int mode = advert::ReadWrite;
    
  std::cout << "Registering with OrchestratorDB: " << std::flush;
  
  //(1) connect to the orchestrator database ==================================
  std::string advertKey("advert://");
  advertKey += database_ + "//" + sessionUUID_ + "/" ;
  
  try {
    advert::directory(advertKey, mode);
  }
  catch(saga::exception const & e) {
    std::cout << "FAILED (" << e.get_error() << ")" << std::endl;
    throw;
  }
  
  //(2a) create a directory for this agent ====================================
  try {
    advertKey += ADVERT_DIR_AGENTS;
    advertKey += "/" + uuid_ + "/";
    
    agentAdvertDir_ = advert::directory(advertKey, mode | advert::Create);
  }
  catch(saga::exception const & e) {
    std::cout << "FAILED (" << e.get_error() << ")" << std::endl;
    throw;
  }
  
  //(2b) create the frontend command advert ===================================
  try {
    // the COMMAND attribute is for sending notifications to this agent
    agentAdvertDir_.set_attribute("COMMAND", "");
  }
  catch(saga::exception const & e) {
    std::cout << "FAILED (" << e.get_error() << ")" << std::endl;
    throw;
  }
    
  //(3) add some initial system information ===================================
  try {
    agentAdvertDir_.set_attribute(ATTR_CPU_COUNT, 
        boost::lexical_cast<std::string>(systemInfo_.hardwareInfo().nCpu));
    agentAdvertDir_.set_attribute(ATTR_CPU_TYPE, 
        boost::lexical_cast<std::string>(systemInfo_.hardwareInfo().cpuType));
    agentAdvertDir_.set_attribute(ATTR_CPU_SUBTYPE, 
        boost::lexical_cast<std::string>(systemInfo_.hardwareInfo().cpuSubtype));
    agentAdvertDir_.set_attribute(ATTR_CPU_FREQ, 
        boost::lexical_cast<std::string>(systemInfo_.hardwareInfo().cpuFrequency));
    agentAdvertDir_.set_attribute(ATTR_MEM_TOTAL, 
        boost::lexical_cast<std::string>(systemInfo_.hardwareInfo().totalMemory));
  
    agentAdvertDir_.set_attribute(ATTR_HOST_NAME,     systemInfo_.hostName());
    agentAdvertDir_.set_attribute(ATTR_HOST_TYPE,     systemInfo_.hostType());
    agentAdvertDir_.set_attribute(ATTR_HOST_LOAD_AVG, systemInfo_.hostLoadAverage());
  }
  catch(saga::exception const & e) {
    std::cout << "FAILED (" << e.get_error() << ")" << std::endl;
    throw;
  }
  
  //(4) set the last seen (keep alive) timestamp ==============================
  time_t timestamp; time(&timestamp);
  try {
    agentAdvertDir_.set_attribute(ATTR_LAST_SEEN, 
      boost::lexical_cast<std::string>(timestamp));
  }
  catch(saga::exception const & e) {
    std::cout << "FAILED (" << e.get_error() << ")" << std::endl;
    throw;
  }
  
  std::cout << "SUCCESSFUL" << std::endl;
  std::cout << "\tContact URL = " << agentAdvertDir_.get_url() << std::endl;
}

///////////////////////////////////////////////////////////////////////////////
//
void AgentApp::updateStatus_()
{
  std::cout << std::endl << "Updating agent status: " << std::flush;

  //(1) update the last seen (keep alive) timestamp 
  time_t timestamp; time(&timestamp);
  try {
    agentAdvertDir_.set_attribute(ATTR_LAST_SEEN, 
      boost::lexical_cast<std::string>(timestamp)); 
  }
  catch(saga::exception const & e) {
    std::cout << "FAILED (" << e.get_error() << ")" << std::endl;
    throw;
  }
  
  //(2) update the current load average
  try {
    agentAdvertDir_.set_attribute(ATTR_HOST_LOAD_AVG, 
                                    systemInfo_.hostLoadAverage());
  }
  catch(saga::exception const & e) {
    std::cout << "FAILED (" << e.get_error() << ")" << std::endl;
    throw;
  }
  
  //(3) update execution status
  

  
  std::cout << "SUCCESSFUL" << std::endl;
}

///////////////////////////////////////////////////////////////////////////////
//
int AgentApp::getFrontendCommand_()
{
  std::string commandString;
  
  try {
    commandString = agentAdvertDir_.get_attribute("COMMAND");
  }
  catch(saga::exception const & e) {
    std::cout << "FAILED (" << e.get_error() << ")" << std::endl;
    throw;
  }
  try {
    // get command number & reset the attribute to "" 
    int command = boost::lexical_cast<int>(commandString);
    agentAdvertDir_.set_attribute("COMMAND", "");
    return command;
  }
  catch(std::exception const & e) {
    // bad lexical cast = unknown/no command
    return -1;
  }
}

///////////////////////////////////////////////////////////////////////////////
//
void AgentApp::mainLoop_(unsigned int updateInterval)
{
  while(1) {
    
    //(1) read command from orchestrator
    int command = getFrontendCommand_();
    switch(command) 
    {
      case AGENT_COMMAND_DISCARD:
        break;
      case AGENT_COMMAND_RESUME:
        break;
      case AGENT_COMMAND_PAUSE:
        break;
      case AGENT_COMMAND_QUIT:
        return;
        break;
      default:
        break;
    }
    
      
    //(2) write some statistics + ping signal 
    updateStatus_();
        
    //(3) sleep for a while
    sleep(updateInterval);
  }
}

void AgentApp::cleanup_()
{

}

