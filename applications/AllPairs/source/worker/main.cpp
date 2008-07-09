//  Copyright (c) 2008 Michael Miceli and Christopher Miceli
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "AllPairsBase.hpp"
#include "../utils/type.hpp"

class AllPairsImpl : public AllPairs::AllPairsBase {
  public:
   AllPairsImpl(int argCount, char **argList)
     : AllPairsBase(argCount, argList) {}
   void compare(std::string object1, std::string object2) {
      emit(object1 == object2);
   }
};

/*********************************************************
 * This is the entry point for the worker, the master    *
 * will call this, there is no need to ever call this    *
 * directly.                                             *
 * ******************************************************/
int main(int argc,char **argv) {
   try {
      AllPairs::AllPairsBase *allPairs = new AllPairsImpl(argc, argv);
      allPairs->run();
   }
   catch (saga::exception const & e)
   {
      std::cerr << "ERROR: " << e.get_message() << std::endl;
   }
   catch (std::exception const & e)
   {
      std::cerr << "ERROR: " << e.what() << std::endl;
   }
   return 0;
}
