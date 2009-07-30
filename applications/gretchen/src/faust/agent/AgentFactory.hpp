#include <iostream>
#include <vector>

#include <faust/agent/Agent.hpp>

#ifndef FAUST_AGENT_AGENTFACTORY
#define FAUST_AGENT_AGENTFACTORY


namespace faust { namespace agent {

  /*! \brief Possible %Agent types.
   *
   */     
  enum AgentType {
    ThriftAgentT = 0x2001, /**< %Agent that uses the facebook Thrift framework 
     for communication                             */
    AdvertAgentT = 0x2002  /**< %Agent that uses an advert DB for (indirect) 
     communication                                 */
  };
  
  /*! \brief Provides a factory to create %Agent instances.
   *
   */  
  class AgentFactory {
  
  private:
    
    static AgentFactory * singleton;
    std::vector<Agent*> listOfAllAgents;
    
  protected:
    
    AgentFactory();
    
  public: 
    
    /*! \brief Returns the global %Agent Factory. The %Agent %Factory is a 
     *         singleton instance - only one can exist within an application.
     *
     */   
    static AgentFactory *getFactory();
    
    /*! \brief Creates and returns a new Agent instance.
     *
     */   
    Agent *createAgent(std::string rmURL, AgentType type, int instances=1); 
    
    /*! \brief Returns a list of all agents known by this factory.
     *
     */   
    std::vector<Agent*> listAgents();
    
  };

} }

#endif