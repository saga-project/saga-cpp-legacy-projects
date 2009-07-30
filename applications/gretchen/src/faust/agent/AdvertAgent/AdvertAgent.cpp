
#include <boost/lexical_cast.hpp>
#include <faust/agent/AdvertAgent/AdvertAgent.hpp>

using namespace saga;
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

AdvertAgent::AdvertAgent(std::string rmURL, int instances)
: rmURL(rmURL), instances(instances)
{
  this->UUID = saga::uuid().string();
  this->taskQueueUUID = saga::uuid().string();
  
  // Create an AdvertDB entry for this Agent's task queue
  taskQueueAdvertURL = std::string(CONFIG_ADVERT_BASE_DIR);
  taskQueueAdvertURL += "/TASKQUEUES/" + this->taskQueueUUID;
  advert::directory ad(taskQueueAdvertURL, advert::CreateParents);
  
  // create an AdvertDB entry for this agent
  agentAdvertURL = std::string(CONFIG_ADVERT_BASE_DIR);
  agentAdvertURL += "/AGENTS/" + this->UUID;
  advert::entry ae(agentAdvertURL, advert::CreateParents);
    
  // add a pointer to this Agent's task queue
  ae.set_attribute("TASKQUEUE", taskQueueAdvertURL);
}

void AdvertAgent::taskQueuePush(Task task)
{
  // if the taskQueueUUID is empty, we haven't created an AdvertDB entry
  // for this task queue. 
  advert::directory ad(taskQueueAdvertURL, advert::ReadWrite);
    
  advert::entry ae = ad.open(task.getUUID(), advert::CreateParents);
    
  ae.set_attribute("COMMAND",       task.getCommand());
  ae.set_attribute("RESULT",        task.getResult());
  ae.set_attribute("STATE",         taskStateToStr(task.getState()));
  ae.set_attribute("EXECUTIONTIME", boost::lexical_cast<std::string>(task.getExecutionTime()));
  ae.set_vector_attribute("ARGS",   task.getArguments());
  
  taskQueue.push_back(task);
}

void AdvertAgent::taskQueuePop()
{
  // does the taskQueue need an update? 
  
  Task task(taskQueue.back());
  if( task.getState() != TaskNew )
    throw("Only states in 'TaskNew' state can be removed");
  else {
    
  }
}

void AdvertAgent::initAdvertDB()
{
}

void AdvertAgent::start() 
{
  prepareJob();
  launchJob();
}

void AdvertAgent::terminate() 
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


void AdvertAgent::prepareJob()
{
  try {
    
    // launch the job
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

void AdvertAgent::launchJob()
{
  try {
    set_state_ts(AgentStarting);
    //sagaJob.run();
    
    faust::agent::AgentState s = sagaToAgentState(sagaJob.get_state());
    set_state_ts(s);
  }
  catch(saga::exception const & e) {
    set_state_ts(AgentFailed);
  }
  
  //if something went wrong, set state to failed!
  
}

