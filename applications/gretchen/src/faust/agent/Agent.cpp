/*
 *  Agent.cpp
 *  gretchen
 *
 *  Created by Ole Weidner on 7/25/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include <faust/agent/Agent.hpp>

using namespace faust::agent;

AgentState Agent::getState()
{
  return get_state_ts();
}

AgentState Agent::get_state_ts()
{
  boost::mutex::scoped_lock l(m_mutex);
  return state;
}

void Agent::set_state_ts(AgentState state)
{
  boost::mutex::scoped_lock l(m_mutex);
  this->state = state;
}


void Agent::taskQueuePush(Task t)
{
  taskQueue.push_back(t);
}

void Agent::taskQueuePop()
{
  // TODO 
}

std::vector<Task> Agent::taskQueueList()
{
  return taskQueue;
}

std::string Agent::getUUID()
{
  return UUID;
}

