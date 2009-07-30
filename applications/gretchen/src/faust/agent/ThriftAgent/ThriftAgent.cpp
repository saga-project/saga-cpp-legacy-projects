#include <faust/agent/ThriftAgent/ThriftAgent.hpp>

using namespace faust::agent;

namespace {
  faust::agent::AgentState sagaToAgentState(saga::job::state s)
  {
    faust::agent::AgentState fas = faust::agent::AgentNew;
    
    switch(s) {
        
      case saga::job::New:
        fas = faust::agent::AgentNew;
        break;
        
      case saga::job::Running:
        fas = faust::agent::AgentRunning;
        break;
        
      case saga::job::Suspended:
        fas = faust::agent::AgentTerminated;
        break;
        
      case saga::job::Failed:
        fas = faust::agent::AgentFailed;
        break;
        
      case saga::job::Canceled:
        fas = faust::agent::AgentTerminated;
        break;
        
      case saga::job::Done:
        fas = faust::agent::AgentTerminated;
        break;
    }
    
    return fas;
  };
}

ThriftAgent::ThriftAgent(std::string rmURL, int instances)
: rmURL(rmURL), instances(instances)
{
  
}



void ThriftAgent::start() 
{
  std::cout << "START" << std::endl;
  
  prepareJob();
  launchJob();
}

void ThriftAgent::terminate() 
{
  try {
    sagaJob.cancel();
    Agent::set_state_ts(AgentTerminated); 
  }
  catch(saga::exception const & e) {
    set_state_ts(AgentFailed);
  }
  
  std::cout << "STOP" << std::endl;
}


void ThriftAgent::prepareJob()
{
  try {
    saga::job::service js(rmURL);
    saga::job::description jd;
    jd.set_attribute("Executable", "/usr/bin/python");
    
    std::vector<std::string> args;
    args.push_back("/tmp/test.py");
    jd.set_vector_attribute("Arguments", args);
    
    sagaJob = js.create_job(jd);
  }
  catch(saga::exception const & e) {
    set_state_ts(AgentFailed);
  }
}

void ThriftAgent::launchJob()
{
  try {
    set_state_ts(AgentStarting);
    sagaJob.run();
    
    faust::agent::AgentState s = sagaToAgentState(sagaJob.get_state());
    set_state_ts(s);
  }
  catch(saga::exception const & e) {
    set_state_ts(AgentFailed);
  }
  
  //if something went wrong, set state to failed!
  
}

