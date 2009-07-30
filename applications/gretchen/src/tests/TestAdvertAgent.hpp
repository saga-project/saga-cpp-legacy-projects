/*
 *  TestAgentFactory.h
 *  gretchen
 *
 *  Created by Ole Weidner on 7/30/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include <saga/saga.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/framework.hpp>

#include <faust/agent/AgentFactory.hpp>
#include <faust/agent/AdvertAgent/AdvertAgent.hpp>

using namespace std;
using namespace boost;
using namespace boost::unit_test;
using namespace faust::agent;

#ifndef FAUST_TESTS_TEST_ADVERTAGENT
#define FAUST_TESTS_TEST_ADVERTAGENT

namespace faust { namespace tests {
  
  BOOST_AUTO_TEST_CASE( TestAdvertAgent )
  {        
    std::string agentAdvertURL("advert://macpro01.cct.lsu.edu//FAUST/AGENTS/");
    std::string taskAdvertURL("advert://macpro01.cct.lsu.edu//FAUST/TASKQUEUES/"); 
    
    // Cast explicitly so we can use some of the "hidden" functionality
    // of the Advert Agent, like getTaskQueueUUID().
    AdvertAgent * a = dynamic_cast<AdvertAgent*>
      ( AgentFactory::getFactory()->createAgent("", AdvertAgentT));
    
    a->start();
    std::string auuid(a->getUUID());

    if(auuid.empty() == true)
      BOOST_FAIL("agent doesn't have a valid UUID.\n");
   
    agentAdvertURL += auuid;
    if(!saga::advert::entry(agentAdvertURL).is_entry())
      BOOST_FAIL("agent doesn't appear to be in the Advert DB.");
    
    Task t("test_cmd");
    std::vector<std::string> args;
    args.push_back("test_args");
    t.setArguments(args);
    
    a->taskQueuePush(t);
        
    std::string tuuid(t.getUUID());
    taskAdvertURL += a->getTaskQueueUUID() + std::string("/") + tuuid;
    
    saga::advert::entry ae(taskAdvertURL);

    if(!ae.is_entry())
      BOOST_FAIL("task doesn't appear to be in the Advert DB.");
    
    if(ae.get_attribute("COMMAND").compare(t.getCommand()) != 0)
      BOOST_FAIL("task attribute 'COMMAND' doesn't appear to be in the Advert DB'.\n");

    if(ae.get_attribute("STATE") != "TaskNew")
      BOOST_FAIL("task attribute 'STATE' doesn't appear to be in the Advert DB'.\n");    

    if(ae.get_attribute("RESULT").compare(t.getResult()) != 0)
      BOOST_FAIL("task attribute 'RESULT' doesn't appear to be in the Advert DB'.\n");    

    if(ae.get_attribute("EXECUTIONTIME").compare(boost::lexical_cast<std::string>(t.getExecutionTime())) != 0)
      BOOST_FAIL("task attribute 'EXECUTIONTIME' doesn't appear to be in the Advert DB'.\n");    

    if(ae.get_vector_attribute("ARGS").at(0).compare(t.getArguments().at(0)) != 0)
      BOOST_FAIL("task attribute 'ARGS' doesn't appear to be in the Advert DB'.\n");    
    
  }
  
}}

#endif // FAUST_TESTS_TEST_ADVERTAGENT
