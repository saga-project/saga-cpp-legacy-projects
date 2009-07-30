#include <saga/saga.hpp>
#include <faust/agent/Agent.hpp>

#ifndef FAUST_AGENT_THRIFTAGENT
#define FAUST_AGENT_THRIFTAGENT


namespace faust { namespace agent {
  
  class AgentFactory;
  
  /*! \brief Implementation of an experimental %Agent that uses the facebook
   *         Thrift cross-language services framework for the underlying 
   *         messaging implementation.
   *
   */  
  class ThriftAgent : public Agent
    {
      
      friend class faust::agent::AgentFactory;
      
    private: 
      saga::job::job sagaJob;
      
      std::string rmURL;
      int instances;
      
      ThriftAgent(std::string rmURL, int instances=1);
      
      void stageIn();
      void stateOut();
      
      void prepareJob();
      void launchJob();
      
    public: 
      void start();
      void terminate();
      
    };
  
} }

#endif