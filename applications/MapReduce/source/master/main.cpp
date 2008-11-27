//  Copyright (c) 2008 Michael Miceli and Christopher Miceli
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "Master.hpp"
#include "../xmlParser/xmlParser.h"

using namespace MapReduce::Master;

class MasterImpl : public Master<MasterImpl> {
  public:
   MasterImpl(int argC, char **argV) : Master<MasterImpl>(argC,argV) {}
};

/*********************************************
 * This is the entry point for the MapReduce *
 * framework.  Just create an instance of    *
 * Master and call init with args            *
 * Possible args are --config config_file    *
 * ******************************************/
int main(int argc, char* argv[]) {
  try {
      MasterImpl app(argc, argv);
      app.run();
   }
   catch (saga::exception const& e) {
      std::cerr << "Saga:  exception caught: " << e.what() << std::endl;
      std::cerr << "Exiting..." << std::endl;
   }
   catch (xmlParser::exception const& e) {
      std::cerr << "xmlParser:  exception caught: " << e.what() << std::endl;
      std::cerr << "Exiting..." << std::endl;
   }
   catch (std::exception const& e) {
      std::cerr << "std:  exception caught: " << e.what() << std::endl;
      std::cerr << "Exiting..." << std::endl;
   }
   catch (...) {
      std::cerr << "FATAL Exception caught!" << std::endl << "Exiting..." << std::endl;
      return 255;
   }
   return 0;
}
