 //  Copyright (c) 2008 Michael Miceli and Christopher Miceli
 // 
 //  Distributed under the Boost Software License, Version 1.0. (See accompanying 
 //  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <saga/saga.hpp>
#include <unistd.h>
#include <wand/MagickWand.h>
#include <boost/lexical_cast.hpp>

#define ThrowWandException(wand) \
{ \
char \
*description; \
\
ExceptionType \
severity; \
\
description=MagickGetException(wand,&severity); \
(void) fprintf(stderr,"%s %s %lu %s\n",GetMagickModule(),description); \
description=(char *) MagickRelinquishMemory(description); \
exit(-1); \
}

using namespace std;
 
double compare(saga::url testUrl, saga::url baseUrl) {
   // Need to create tmp names, since I need ImageMagick to understand the names
   // therefore I need to copy the images to those files
   // If I can get imagemagick to read directly from the url, I woudn't waste
   // the time in copying files...  
   char tName[] = "/tmp/testImgXXXXXX";
   char bName[] = "/tmp/baseImgXXXXXX";
   std::cerr << "about to generate names!" << std::endl;
   int testFile = mkstemp(tName);
   std::cerr << "got one (" << tName << ")" << std::endl;
   if(testFile == -1) 
      perror(NULL);
   int baseFile = mkstemp(bName);
   std::cerr << "got two (" << bName << ")" << std::endl;
   if(baseFile == -1) 
      perror(NULL);

   std::cerr << "Before copying the files... " << std::endl; 
   
   saga::size_t const KB64 = 1024*64; //64KB
   saga::size_t bytesRead;
   std::cerr << "about to open files (" << testUrl.get_string() << ", " << baseUrl.get_string() << ")" << std::endl;
   saga::filesystem::file test(testUrl, saga::filesystem::Read);
   std::cerr << "1opened!" << std::endl;
   saga::filesystem::file base    (baseUrl    , saga::filesystem::Read);
   std::cerr << "2opened!" << std::endl;
   
   char data[KB64+1];
   std::cerr << "about to read" << std::endl;
   try {
      while((bytesRead = test.read(saga::buffer(data,KB64)))!=0) {
         std::cerr << "read from testFile" << std::endl;
         int retval = write(testFile, data, bytesRead);
         if(retval < 0)
            perror(NULL);
      }
   }
   catch(saga::exception const & e) {
      std::cerr << "Exception caught: " << e.what() << std::endl;
   }
   test.close();
   fsync(testFile);
   close(testFile);
   try {
      while((bytesRead = base.read(saga::buffer(data,KB64)))!=0) {
         std::cerr << "read from baseFile" << std::endl;
         std::cerr << "size = " << base.get_size() << std::endl;
         int retval = write(baseFile, data, bytesRead);
         if(retval < 0)
            perror(NULL);
      }
   }
   catch(saga::exception const & e) {
      std::cerr << "Exception caught: " << e.what() << std::endl;
   }
   base.close();
   fsync(baseFile);
   close(baseFile);
   
   std::cerr << "Just before Image magick" << std::endl;
   
   MagickWand *magick_wand_1;
   MagickWand *magick_wand_2;
   MagickWand *compare_wand;
   double difference;
   MagickBooleanType status_1, status_2;
   
   MagickWandGenesis();
   magick_wand_1=NewMagickWand();
   status_1=MagickReadImage(magick_wand_1,bName);
   if (status_1 == MagickFalse)
	   ThrowWandException(magick_wand_1);
   MagickWandGenesis();
   magick_wand_2=NewMagickWand();
   status_2=MagickReadImage(magick_wand_2,tName);
   if (status_2 == MagickFalse)
	   ThrowWandException(magick_wand_2);
   compare_wand = NewMagickWand();
   compare_wand=MagickCompareImages(magick_wand_1,magick_wand_2, AbsoluteErrorMetric, &difference);
   std::cerr << "Difference is: " << difference << std::endl;
   
   DestroyMagickWand(magick_wand_1);
   DestroyMagickWand(magick_wand_2);

   MagickWandTerminus();
   remove(bName);
   remove(tName);
   return difference;
}

/*********************************************************
 * This is the entry point for the worker, the master    *
 * will call this, there is no need to ever call this    *
 * directly.                                             *
 * ******************************************************/
int main(int argc,char **argv) {
   char env[] = "SAGA_VERBOSE=100";
   putenv(env);
   saga::url first, second;
   saga::url advertURL;
   saga::advert::entry advert;
   bool firstSet, secondSet;
   firstSet = false;
   secondSet = false;
   //Advert is always last because we are generating the command!
   first     = saga::url(argv[1]);
   second    = saga::url(argv[2]);
   advertURL = saga::url(argv[4]);
   std::cerr << "set advert to : " << advertURL.get_string();
   std::cerr << "set first to : " << first.get_string();
   std::cerr << "set second to : " << second.get_string();
   try
   {
      double result = compare(first, second);
      saga::advert::entry advert(advertURL, saga::advert::ReadWrite | 
                                            saga::advert::Overwrite | 
                                            saga::advert::CreateParents);
      std::cerr << "ABOUT TO STORE " << result << " IN ADVERT: " << advertURL.get_string() << std::endl;
      //FIXME: Possibly put result as a double in there
      advert.store_object<std::string>(boost::lexical_cast<std::string>(result));
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
