/*
 *  main.cpp
 *  FAUST - Framework for Adaptive Ubiquitous Scalable Tasks
 *  Website: https://macpro01.cct.lsu.edu/trac/faust
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

#include <agent/agent.hpp>

namespace po = boost::program_options;

///////////////////////////////////////////////////////////////////////////////
//
bool parse_commandline(int argc, char *argv[], po::variables_map& vm)
{
  po::options_description desc_cmdline ("Usage: "+std::string("faust_agent")+" [options]");
  try {
    desc_cmdline.add_options()
    ("help, h", 
     "Display this information and exit.")
    
    ("version, v", 
     "Print version information and exit.")
    
    ("endpoint, e", po::value<std::string>(), 
     "Advert endpoint this agent should register with.")

    ("identifier, i", po::value<std::string>(), 
     "Unique identifier for this agent.")
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
    
    if (!vm.count("endpoint")) {
      std::cerr << "Missing endpoint URL: use --endpoint=" 
      << std::endl << std::endl << desc_cmdline << std::endl;
      return false;
    }
    
    if (!vm.count("identifier")) {
      std::cerr << "Missing unique identifier: use --identifier=" 
      << std::endl << std::endl << desc_cmdline << std::endl;
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
  std::string endpoint   (vm["endpoint"].as<std::string>());
  std::string identifier (vm["identifier"].as<std::string>());
  
  // main application loop
  faust::agent faust_agent(endpoint, identifier);
  faust_agent.run();
  
  return 0;
}

