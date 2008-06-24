//  Copyright (c) 2008 Michael Miceli and Christopher Miceli
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "version.hpp"
#include <boost/program_options.hpp>

namespace po = boost::program_options;

bool parseCommand(int argc, char *argv[], po::variables_map& vm) {
  po::options_description desc_cmdline ("Usage: "+std::string(MR_WORKER_EXE_NAME)+" [options]");
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
       std::cout << std::endl << MR_WORKER_VERSION_FULL << std::endl;
       return false;
    }
    if (!vm.count("session")) {
       std::cerr << "Missing session UUID: use --session" << std::endl;
       return false;
    }
    else if (!vm.count("database")) {
       std::cerr << "Missing orchestrator database hostname: use --database" << std::endl;
       return false;
    }
  }
  catch (std::exception const& e) {
     std::cerr << std::endl << e.what() << std::endl << std::endl << desc_cmdline << std::endl;
     return false;
  }
  return true;
}
