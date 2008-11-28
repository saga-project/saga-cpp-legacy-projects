//  Copyright (c) 2008 Michael Miceli and Christopher Miceli
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "Master.hpp"
#include "../xmlParser/xmlParser.h"
#include <boost/scoped_ptr.hpp>

using namespace MapReduce::Master;

class MasterImpl : public Master<MasterImpl> {
  public:
   MasterImpl(int argC, char **argV) : Master<MasterImpl>(argC,argV) {}
   void chunker(std::vector<saga::url> &retval, std::string fileArg) {
      int mode = saga::filesystem::Read;
      int x=0;
      saga::size_t const KB64 = 67108864;
      saga::size_t bytesRead;
      saga::url urlFile(fileArg);
      boost::scoped_array<char> data(new char[KB64+1]);
      saga::filesystem::file f(urlFile, mode);
      while((bytesRead = f.read(saga::buffer(data.get(),KB64))) != 0) {
         saga::size_t pos;
         int gmode = saga::filesystem::Write | saga::filesystem::Create;
         saga::filesystem::file g(saga::url(fileArg + "chunk" + boost::lexical_cast<std::string>(x)), gmode);
         if(bytesRead < KB64)
         {
            g.write(saga::buffer(data.get(), bytesRead));
            retval.push_back(g.get_url());
         }
         else
         {
            for(int y=bytesRead; y >= 0;y--) {
               if(data[y]==' ') {
                  pos=y;
                  break;
               }
            }
            int dist = -(bytesRead-pos);
            g.write(saga::buffer(data.get(), pos));
            f.seek(dist,saga::filesystem::Current);
            retval.push_back(g.get_url());
            x++;
            for ( unsigned int i = 2; i <= KB64; ++i ) { data[i] = '\0'; }
         }
      }
   }
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
      return 255;
   }
   catch (xmlParser::exception const& e) {
      std::cerr << "xmlParser:  exception caught: " << e.what() << std::endl;
      std::cerr << "Exiting..." << std::endl;
      return 255;
   }
   catch (std::exception const& e) {
      std::cerr << "std:  exception caught: " << e.what() << std::endl;
      std::cerr << "Exiting..." << std::endl;
      return 255;
   }
   catch (...) {
      std::cerr << "FATAL Exception caught!" << std::endl << "Exiting..." << std::endl;
      return 255;
   }
   return 0;
}
