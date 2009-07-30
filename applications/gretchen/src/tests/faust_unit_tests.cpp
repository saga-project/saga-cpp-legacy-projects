/*
 *  faust_unit_tests.cpp
 *  gretchen
 *
 *  Created by Ole Weidner on 7/30/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include <boost/version.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/framework.hpp>

#include <tests/TestAgentFactory.hpp>
#include <tests/TestTask.hpp>
#include <tests/TestADvertAgent.hpp>


using namespace faust::tests;

#if BOOST_VERSION >= 103400


// Init function for boost::unit_tests >= boost 1.34 //////////////////////////
//
bool init_function()
{
  using namespace boost::unit_test;
  
  BOOST_TEST_SUITE ("FAUST Unit Tests");
  
  return true;
}

int main(int argc, char* argv[])
{
  return boost::unit_test::unit_test_main(&init_function, argc, argv);
}

#else /////////////////////////////////////////////////////////////////////////


// Pre-Bost-1.34 entry point 
//
boost::unit_test::test_suite *init_unit_test_suite (int argc, char* argv[])
{
  using namespace boost::unit_test;
  test_suite * suite = BOOST_TEST_SUITE ("FAUST Unit Tests");
  
  return suite;
}

#endif ////////////////////////////////////////////////////////////////////////
