#include <saga/saga.hpp>
#include <faust/agent/Agent.hpp>

#ifndef FAUST_AGENT_ADVERTAGENT
#define FAUST_AGENT_ADVERTAGENT

#define CONFIG_ADVERT_BASE_DIR "advert://macpro01.cct.lsu.edu//FAUST/"

namespace faust { namespace agent {
  
  class AgentFactory;
  
  /*! \brief Implementation of an experimental %Agent that uses the facebook
   *         Thrift cross-language services framework for the underlying 
   *         messaging implementation.
   *
   */  
  class AdvertAgent : public Agent
    {
      
      friend class faust::agent::AgentFactory;
      
    private: 
      
      std::string agentAdvertURL;
      std::string taskQueueAdvertURL;
      
      saga::job::job sagaJob;
      
      std::string rmURL;
      int instances;
      
      AdvertAgent(std::string rmURL, int instances=1);
      
      void initAdvertDB();
      
      void stageIn();
      void stateOut();
      
      void prepareJob();
      void launchJob();
      
    public: 
      
      void start();
      void terminate();
      
      void taskQueuePush(Task task);
      void taskQueuePop();
      
      ///@cond - hide from doxygen
      std::string getTaskQueueUUID() { return taskQueueUUID; }
      ///@endcond
      
    };
  
} }

#endif