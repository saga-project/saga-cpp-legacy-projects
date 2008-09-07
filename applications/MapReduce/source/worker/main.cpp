//  Copyright (c) 2008 Michael Miceli and Christopher Miceli
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "MapReduceBase.hpp"
#include "../utils/type.hpp"

using namespace MapReduce;

class MapReduceImpl : public MapReduceBase<MapReduceImpl> {
  public:
   MapReduceImpl(int argCount, char **argList) : MapReduceBase<MapReduceImpl>(argCount,argList) {}
   /*********************************************************
    * The implemented map function that will get called when*
    * it becomes time to do some mapping.                   *
    * ******************************************************/
   void map(saga::url chunkName) {
      boost::iostreams::stream <saga_file_device> in (chunkName.get_string());
      std::string elem;
      while(in >> elem) {
         emitIntermediate(elem,"1");
      }
   }
      
   /*********************************************************
    * The implemented reduce function that will get called  *
    * when it becomes time to do some reducing.             *
    * ******************************************************/
   void reduce(std::string key, const std::vector<std::string> &values) {
     int result = 0;
     std::vector<std::string>::const_iterator valuesIT = values.begin();
     while(valuesIT != values.end()) {
        result += boost::lexical_cast<int>(*valuesIT);
        valuesIT++;
     }
     emit(key, boost::lexical_cast<std::string>(result));
   }
};

/*********************************************************
 * This is the entry point for the worker, the master    *
 * will call this, there is no need to ever call this    *
 * directly.                                             *
 * ******************************************************/
int main(int argc,char **argv) {
  putenv("SAGA_VERBOSE=100");
  std::freopen("/tmp/worker-stderr.txt", "w", stderr);
  std::freopen("/tmp/worker-stdout.txt", "w", stdout);
  try {
      MapReduceImpl app(argc,argv);
      app.run();
   }
   catch (saga::exception const& e) {
      std::cerr << "Saga:  exception caught: " << e.what() << std::endl;
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
