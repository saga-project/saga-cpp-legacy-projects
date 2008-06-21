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
#include "SubmitApp.hpp"



using namespace TaskFarmer::Frontend;
namespace po = boost::program_options;

///////////////////////////////////////////////////////////////////////////////
//
bool parse_commandline(int argc, char *argv[], po::variables_map& vm)
{
  po::options_description desc_cmdline ("Usage: "+std::string(TF_FRONTEND_EXE_NAME)+" [options]");
  try {
    desc_cmdline.add_options()
      ("help,h", "Display this information and exit")
      
      ("version,v", "Print version information and exit")

      ("config,c", po::value<std::string>(), 
       "The config filename for this session")
    
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
      std::cout << std::endl << TF_FRONTEND_VERSION_FULL << std::endl;
      return false;
    }

    if (!vm.count("config")) {
      std::cerr << "Missing config filename: use --config" 
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
  std::string config   (vm["config"].as<std::string>());
  
  // start the main application loop
  try {
    SubmitApp app = SubmitApp(config);
    app.run();
  }
  catch (...) {
    std::cerr << "FATAL Exception caught! Exiting..." << std::endl;
    return(255);
  }
}
