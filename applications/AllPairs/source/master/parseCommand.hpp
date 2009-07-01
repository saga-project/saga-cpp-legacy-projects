//  Copyright (c) 2008 Michael Miceli and Christopher Miceli
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef AP_PARSE_COMMAND_HPP
#define AP_PARSE_COMMAND_HPP

#include <functional>     // workaround for TR1 problem in Boost V1.35
#include <boost/program_options.hpp>
#include <boost/lexical_cast.hpp>
#include <string>

#include "version.hpp"

namespace po = boost::program_options;

/*********************************************************
 * parseCommand uses boost program options to parse the  *
 * command lines passed directly from main.  The user may*
 * parse the command lines before passing argv and argc. *
 * ******************************************************/
bool parseCommand(int argCount, char *argList[], po::variables_map& vm) {
   po::options_description desc_cmdline ("Usage: "+std::string(AP_MASTER_EXE_NAME)+" [options]");
   try {
      desc_cmdline.add_options()
        ("help,h", "Display this information and exit")
        ("version,v", "Print version information and exit")
        ("config,c", po::value<std::string>(), "The config filename for this session")
        ("database,d", po::value<std::string>(), "Hostname of the orchestrator database")
        ("staging,s", "Enable staging")
        ;
      po::positional_options_description p;
      po::store(po::parse_command_line(argCount, argList, desc_cmdline), vm);
      po::notify(vm);
      if (vm.count("help")) {
         std::cout << std::endl << desc_cmdline << std::endl;
         return false;
      }
      if (vm.count("version")) {
         std::cout << std::endl << AP_MASTER_VERSION_FULL << std::endl;
         return false;
      }
      if (!vm.count("config")) {
         std::cerr << "Missing config filename: use --config" << std::endl;
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

#endif // AP_PARSE_COMMAND_HPP

