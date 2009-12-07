 //  Copyright (c) 2008 Michael Miceli and Christopher Miceli
 // 
 //  Distributed under the Boost Software License, Version 1.0. (See accompanying 
 //  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 
#include "AllPairsBase.hpp"
//#include "../utils/type.hpp"
//#include <fstream>
#include <unistd.h>
#include <wand/MagickWand.h>


 using namespace std;
 using namespace AllPairs;
 
 class AllPairsImpl : public AllPairsBase<AllPairsImpl> {
   public:
    AllPairsImpl(int argCount, char **argList)
      : AllPairsBase<AllPairsImpl>(argCount, argList) {
    }
    double compare(saga::url testUrl, saga::url baseUrl) {
      std::cerr << "two strings are " << std::endl;
      std::cerr << testUrl.get_string() << std::endl;
      std::cerr << baseUrl.get_string() << std::endl;
      saga::size_t const n = 1024*64;
      saga::uint8_t data[n+1];
      if(testUrl.get_scheme().compare("gridftp") == 0)
      {
         std::cout << "Reading " << testUrl.get_string() << "!" << std::endl;
         saga::url temporaryURL(testUrl);
         temporaryURL.set_scheme("gsiftp");
         std::string commandString("globus-url-copy ");
         commandString += temporaryURL.get_string();
         commandString += " file://localhost//tmp/tempfiledeletesaga.tmp";
         std::cerr << "COMMAND STRING IS " << commandString << std::flush << std::endl;
         system(commandString.c_str());
         system("rm /tmp/tempfiledeletesaga.tmp");
         std::cout << "Read " << testUrl.get_string() << "!" << std::endl;
      }
      else
      {
         saga::filesystem::file f (testUrl, saga::filesystem::Read);
         saga::filesystem::file g (baseUrl, saga::filesystem::Read);
         try {
            std::cout << "Reading " << baseUrl.get_string() << "!" << std::endl;
            while (true) {
               std::memset(data, n+1, '\0');
               // read a chunk into the buffer
               if ( f.read (saga::buffer (data, n), n) ) {
                  //std::cout << data;
               }
               else {
                  break;
               }
            }
            std::cout << "Read " << testUrl.get_string() << "!" << std::endl;
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
      }
      if(baseUrl.get_scheme().compare("gridftp") == 0)
      {
         std::cout << "Reading " << testUrl.get_string() << "!" << std::endl;
         saga::url temporaryURL(testUrl);
         temporaryURL.set_scheme("gsiftp");
         std::string commandString("globus-url-copy ");
         commandString += temporaryURL.get_string();
         commandString += " file://localhost//tmp/tempfiledeletesaga.tmp";
         std::cerr << "COMMAND STRING IS " << commandString << std::flush << std::endl;
         system(commandString.c_str());
         system("rm /tmp/tempfiledeletesaga.tmp");
         std::cout << "Read " << testUrl.get_string() << "!" << std::endl;
      }
      else
      {
         saga::filesystem::file f (testUrl, saga::filesystem::Read);
         saga::filesystem::file g (baseUrl, saga::filesystem::Read);
         try {
            std::cout << "Reading " << baseUrl.get_string() << "!" << std::endl;
            while (true) {
               std::memset(data, n+1, '\0');
               // read a chunk into the buffer
               if ( g.read (saga::buffer (data, n), n) ) {
                   //std::cout << data;
               }
               else {
                   break;
               }
            }
            std::cout << "Read " << baseUrl.get_string() << "!" << std::endl;
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
      }
      return 0;
   }
};

/*********************************************************
 * This is the entry point for the worker, the master    *
 * will call this, there is no need to ever call this    *
 * directly.                                             *
 * ******************************************************/
int main(int argc,char **argv) {
   char env[] = "SAGA_VERBOSE=100";
   putenv(env);
   std::string uuid(saga::uuid().string());
   std::string err("/home/mmicel2/worker-" + uuid + "stderr.txt");
   std::string out("/home/mmicel2/worker-" + uuid + "stdout.txt");
   std::freopen(err.c_str(), "w", stderr);
   std::freopen(out.c_str(), "w", stdout);
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
