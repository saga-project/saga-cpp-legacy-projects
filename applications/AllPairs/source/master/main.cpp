//  Copyright (c) 2008 Michael Miceli and Christopher Miceli
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "Master.hpp"

using namespace AllPairs::Master;

/*********************************************
 * This is the entry point for the AllPairs *
 * framework.  Just create an instance of    *
 * Master and call init with args            *
 * Possible args are --config config_file    *
 * ******************************************/
int main(int argc, char* argv[]) {
  try {
      Master app;
      app.init(argc,argv);
   }
   catch (...) {
      std::cerr << "FATAL Exception caught! Exiting..." << std::endl;
      return 255;
   }
   return 0;
}
