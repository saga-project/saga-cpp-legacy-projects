/*
 *  main.cpp
 *  FAUST - Framework for Adaptive Ubiquitous Scalable Tasks
 *
 *  Created by Ole Weidner <oweidner@cct.lsu.edu> on 01/10/09.
 *  Copyright 2008-2009 Center for Computation & Technology. 
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying 
 *  LICENSE file or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include <string>
#include <fstream>
#include <iostream>

#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>

namespace po = boost::program_options;

///////////////////////////////////////////////////////////////////////////////
//
bool parse_commandline(int argc, char *argv[], po::variables_map& vm)
{
  po::options_description desc_cmdline ("Usage: "+std::string("faust_agent")+" [options]");
  try {
    desc_cmdline.add_options()
    ("help,h", "Display this information and exit")
    
    ("version,v", "Print version information and exit")
    
    ("session,s", po::value<std::string>(), 
     "Session UUID this agent should register with")
    
    ("database,d", po::value<std::string>(), 
     "Hostname of the orchestrator database")
    ;
    
    po::positional_options_description p;
    
    po::store(po::parse_command_line(argc, argv, desc_cmdline), vm);
    po::notify(vm);
    
    if (vm.count("help")) {
      std::cout << std::endl << desc_cmdline << std::endl;
      return false;
    }
    
    if (vm.count("version")) {
      std::cout << std::endl << "FAUST - Framework for Adaptive Ubiquitous Scalable Tasks 0.1" << std::endl;
      return false;
    }
    
    if (!vm.count("session")) {
      std::cerr << "Missing session UUID: use --session" 
      << std::endl;
      return false;
    }
    
    else if (!vm.count("database")) {
      std::cerr << "Missing orchestrator database hostname: use --database" 
      << std::endl;
      return false;
    }
    
  }
  catch (std::exception const& e) {
    std::cerr << std::endl << e.what() << std::endl 
    << std::endl << desc_cmdline << std::endl;
    return false;
  }
  return true;
}

///////////////////////////////////////////////////////////////////////////////
//
int main( int argc, char** argv )
{
  po::variables_map vm;
  if (!parse_commandline(argc, argv, vm))
    return -2;
  
  // extract command line arguments 
  std::string session  (vm["session"].as<std::string>());
  std::string database (vm["database"].as<std::string>());
  
  // start the main application loop
  
  return 0;
}

