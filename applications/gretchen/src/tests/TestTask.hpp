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

#ifndef FAUST_TESTS_TEST_TASK
#define FAUST_TESTS_TEST_TASK

namespace faust { namespace tests {
  
  BOOST_AUTO_TEST_CASE( TestTask )
  {    
    Task t1("/bin/date");
    if(t1.getCommand() != "/bin/date")
      BOOST_FAIL("getCommand() doesn't work properly.\n");
                 
    if(t1.getUUID().empty() == true)
      BOOST_FAIL("task doesn't have a valid UUID.\n");
    
    std::vector<std::string> args;
    args.push_back("arg1");
    t1.setArguments(args);
        
    if(t1.getArguments().at(0).compare("arg1") != 0)
      BOOST_FAIL("getArguments() doesn't work properly.\n");
    
    if(t1.getState() != TaskNew)
      BOOST_FAIL("problem with task state.\n");
    
    if(t1.getResult().empty() != true)
      BOOST_FAIL("new task can't have a result.\n");
    
    if(t1.getExecutionTime() != 0)
      BOOST_FAIL("new task can't have an execution time.\n");
    
    // test copy constructor
    /*Task t2(t1);
    if(t1.getUUID() == t2.getUUID())
      BOOST_FAIL("copied task can't have the same uuid.\n");
    
    Task t3 = t2;
    if(t2.getUUID() == t3.getUUID())
      BOOST_FAIL("copied task can't have the same uuid.\n");*/
    
    
  }
  
}}

#endif // FAUST_TESTS_TEST_TASK
