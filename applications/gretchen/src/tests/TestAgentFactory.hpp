/*
 *  TestAgentFactory.h
 *  gretchen
 *
 *  Created by Ole Weidner on 7/30/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include <boost/test/unit_test.hpp>
#include <boost/test/framework.hpp>

#include <faust/agent/AgentFactory.hpp>

using namespace std;
using namespace boost;
using namespace boost::unit_test;
using namespace faust::agent;

#ifndef FAUST_TESTS_TEST_AGENT_FACTORY
#define FAUST_TESTS_TEST_AGENT_FACTORY

namespace faust { namespace tests {

  BOOST_AUTO_TEST_CASE( TestAgentFactory )
  {    
    AgentFactory * af1 = AgentFactory::getFactory();    
    AgentFactory * af2 = AgentFactory::getFactory();
    
    // check if singleton works
    if( af1 != af2 )
      BOOST_FAIL ("different pointer addresses.\n");
    
    // check listAgent() function
    if( AgentFactory::getFactory()->listAgents().empty() != true)
      BOOST_FAIL ("listAgents() doesn't seem to work properly.\n");    
    
    af1->createAgent("", AdvertAgentT);
    af2->createAgent("", AdvertAgentT);
    af1->createAgent("", AdvertAgentT);
    af2->createAgent("", AdvertAgentT);
    af1->createAgent("", AdvertAgentT);
    af2->createAgent("", AdvertAgentT);
  
    // check listAgent() function
    if( AgentFactory::getFactory()->listAgents().size() != 6)
      BOOST_FAIL ("listAgents() doesn't seem to work properly.\n");
    
    // create a shitload of agents
    #define SHIT_LOAD 1
    
    std::cout << "Creating a shitload of agents: " << std::flush;
    for(int i=0; i < SHIT_LOAD; ++i) {
      AgentFactory::getFactory()->createAgent("", AdvertAgentT);
      if(i%100 == 0) std::cout << "*" << std::flush;
    }
    std::cout << " DONE. " << std::endl;
        
    if(AgentFactory::getFactory()->listAgents().size() != SHIT_LOAD+6)
      BOOST_FAIL ("listAgents() doesn't seem to work properly.\n");

  }
  
}}

#endif // FAUST_TESTS_TEST_AGENT_FACTORY
