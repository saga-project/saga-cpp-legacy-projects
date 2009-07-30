
#include <faust/agent/AgentFactory.hpp>
#include <faust/agent/ThriftAgent/ThriftAgent.hpp>
#include <faust/agent/AdvertAgent/AdvertAgent.hpp>


using namespace faust::agent;

AgentFactory* AgentFactory::singleton = NULL;

AgentFactory::AgentFactory()
{
}

AgentFactory * AgentFactory::getFactory() 
{
  if(NULL == singleton)
    singleton = new AgentFactory();
  return singleton;
}

Agent *AgentFactory::createAgent(std::string rmURL, AgentType type, int instances) 
{
  Agent * a;
  
  switch (type  ) {
    case AdvertAgentT:
      a = new AdvertAgent(rmURL, instances);
      break;
      
    case ThriftAgentT:
      a = new ThriftAgent(rmURL, instances);
      break;
      
    default:
      //a = new AdvertAgent(rmURL, instances);
      break;
  }
  
  listOfAllAgents.push_back(a);
  return a;
}

std::vector<Agent*> AgentFactory::listAgents()
{
  return listOfAllAgents;
}