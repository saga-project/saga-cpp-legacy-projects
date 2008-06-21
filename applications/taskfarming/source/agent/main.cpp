// The MIT License
//
// Copyright (c) 2007 Ole Weidner (oweidner@cct.lsu.edu)

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include <string>
#include <fstream>
#include <iostream>

#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>

#include "version.hpp"
#include "AgentApp.hpp"

using namespace TaskFarmer::Agent;
namespace po = boost::program_options;

///////////////////////////////////////////////////////////////////////////////
//
bool parse_commandline(int argc, char *argv[], po::variables_map& vm)
{
  po::options_description desc_cmdline ("Usage: "+std::string(TF_AGENT_EXE_NAME)+" [options]");
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
      std::cout << std::endl << TF_AGENT_VERSION_FULL << std::endl;
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
int main(int argc, char* argv[])
{
    po::variables_map vm;
    if (!parse_commandline(argc, argv, vm))
      return -2;
    
    // extract command line arguments 
    std::string session  (vm["session"].as<std::string>());
    std::string database (vm["database"].as<std::string>());

    // start the main application loop
    try {
      AgentApp app = AgentApp(session, database);
      app.run();
    }
  catch (saga::exception const & e) {
      std::cerr << "main(): exception caught: " << e.what() << std::endl;
      return -1;
    }
    catch (std::exception const & e) {
      std::cerr << "main(): std::exception caught: " << e.what() << std::endl;
      return -1;
    }
    catch (...) {
      std::cerr << "main(): unknown exception caught " << std::endl;
      return -1;
  }     
    return 0;
    
  }

