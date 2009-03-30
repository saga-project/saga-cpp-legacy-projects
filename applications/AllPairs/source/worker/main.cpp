 //  Copyright (c) 2008 Michael Miceli and Christopher Miceli
 // 
 //  Distributed under the Boost Software License, Version 1.0. (See accompanying 
 //  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 
#include "AllPairsBase.hpp"
//#include "../utils/type.hpp"
 
 using namespace AllPairs;
 
 class AllPairsImpl : public AllPairsBase<AllPairsImpl> {
   public:
    AllPairsImpl(int argCount, char **argList)
      : AllPairsBase<AllPairsImpl>(argCount, argList) {
      dna_distances.insert(std::pair<std::string,double>("aa", 0.0));
      dna_distances.insert(std::pair<std::string,double>("at", 2.0));
      dna_distances.insert(std::pair<std::string,double>("ac", 2.0));
      dna_distances.insert(std::pair<std::string,double>("ag", 2.0));
      dna_distances.insert(std::pair<std::string,double>("ta", 2.0));
      dna_distances.insert(std::pair<std::string,double>("tt", 0.0));
      dna_distances.insert(std::pair<std::string,double>("tc", 2.0));
      dna_distances.insert(std::pair<std::string,double>("tg", 2.0));
      dna_distances.insert(std::pair<std::string,double>("ca", 2.0));
      dna_distances.insert(std::pair<std::string,double>("ct", 2.0));
      dna_distances.insert(std::pair<std::string,double>("cc", 0.0));
      dna_distances.insert(std::pair<std::string,double>("cg", 2.0));
      dna_distances.insert(std::pair<std::string,double>("ga", 2.0));
      dna_distances.insert(std::pair<std::string,double>("gt", 2.0));
      dna_distances.insert(std::pair<std::string,double>("gc", 2.0));
      dna_distances.insert(std::pair<std::string,double>("gg", 0.0));
   }
   double compare(saga::url fragmentUrl, saga::url baseUrl) {
      saga::size_t const KB64 = 1024*64; //64KB
      saga::size_t bytesRead;
      saga::filesystem::file fragment(fragmentUrl, saga::filesystem::Read);
      saga::filesystem::file base    (baseUrl    , saga::filesystem::Read);
      std::string fragment_string;
      std::string base_string;
      char data[KB64+1];
      double minimum = -1.0;
      while((bytesRead = fragment.read(saga::buffer(data,KB64)))!=0) {
         fragment_string += data;
      }
      while((bytesRead = base.read(saga::buffer(data,KB64)))!=0) {
         base_string += data;
      }
      //go through every substring of base_string
      for(std::string::size_type x = 0;x<base_string.size()-fragment_string.size()+1;x++) {
        //get part of base to compare against
        std::string compare_string = base_string.substr(x,fragment_string.size());
        double distance = 0;
        //calculate the distance
        for(std::string::size_type y = 0;y<compare_string.size();y++) {
           std::string elements = compare_string.substr(y,1) + fragment_string.substr(y,1);
           distance += dna_distances[elements];
        }
        if(minimum == -1.0 || minimum > distance) {
           minimum = distance;
        }
      }
      return minimum;
   }
  private:
   std::map<std::string,double> dna_distances;
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
      AllPairsImpl allPairs(argc, argv);
      allPairs.run();
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
