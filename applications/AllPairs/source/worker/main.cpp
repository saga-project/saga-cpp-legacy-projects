 //  Copyright (c) 2008 Michael Miceli and Christopher Miceli
 // 
 //  Distributed under the Boost Software License, Version 1.0. (See accompanying 
 //  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 
#include "AllPairsBase.hpp"
//#include "../utils/type.hpp"
#include <fstream>
#include <wand/MagickWand.h>


 using namespace std;
 using namespace AllPairs;
 
 class AllPairsImpl : public AllPairsBase<AllPairsImpl> {
   public:
    AllPairsImpl(int argCount, char **argList)
      : AllPairsBase<AllPairsImpl>(argCount, argList) {
   }
   double compare(saga::url testUrl, saga::url baseUrl) {
	   
	   
	   // Need to create tmp names, since I need ImageMagick to understand the names
	   // therefore I need to copy the images to those files
	   // If I can get imagemagick to read directly from the url, I woudn't waste
	   // the time in copying files...  
	   
	   char *testName = tempnam(NULL, "testImg.");
	   char *baseName = tempnam(NULL, "baseImg.");
	   
	   std::cerr << "About to open files (" << testName << ", " << baseName << ")" << std::endl;
	   
	   ofstream testFile(testName, ios::out);
	   ofstream baseFile(baseName, ios::out);
	   
	   std::cerr << "Before copying the files... " << std::endl; 
	   
	   saga::size_t const KB64 = 1024*64; //64KB
	   saga::size_t bytesRead;
	   std::cerr << "about to open files (" << testUrl.get_string() << ", " << baseUrl.get_string() << ")" << std::endl;
	   saga::filesystem::file test(testUrl, saga::filesystem::Read);
	   std::cerr << "1opened!" << std::endl;
	   saga::filesystem::file base    (baseUrl    , saga::filesystem::Read);
	   std::cerr << "2opened!" << std::endl;
	   
	   char data[KB64+1];
	   double ImageDifferencePcnt = -1.0;
	   while((bytesRead = test.read(saga::buffer(data,KB64)))!=0) {
		   testFile.write(data, bytesRead);
	   }
	   while((bytesRead = base.read(saga::buffer(data,KB64)))!=0) {
		   baseFile.write(data, bytesRead);
	   }
	   
	   std::cerr << "Before closing files" << std::endl;
	   
	   // Need to close them before I can actually do something
	   
	   testFile.close();
	   baseFile.close();
	   
	   std::cerr << "Just before Image magick" << std::endl;
	   
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
	   
	   MagickWand *magick_wand_1;
	   MagickWand *magick_wand_2;
	   MagickWand *compare_wand;
	   double difference;
	   MagickBooleanType status_1, status_2;
	   
	   MagickWandGenesis();
	   magick_wand_1=NewMagickWand();
	   status_1=MagickReadImage(magick_wand_1,baseName);
	   if (status_1 == MagickFalse)
		   ThrowWandException(magick_wand_1);
	   
	   MagickWandGenesis();
	   magick_wand_2=NewMagickWand();
	   status_2=MagickReadImage(magick_wand_2,testName);
	   if (status_2 == MagickFalse)
		   ThrowWandException(magick_wand_2);
	   
	   compare_wand = NewMagickWand();
	   compare_wand=MagickCompareImages(magick_wand_1,magick_wand_2, AbsoluteErrorMetric, &difference);
	   std::cerr << "Difference is: " << difference << std::endl;
	   
	   unsigned long rows, cols;
	   cols = MagickGetImageWidth(compare_wand);
	   rows = MagickGetImageHeight(compare_wand);
	   std::cerr << "Size baseimage (rows,cols): " << rows << "x" << cols << std::endl;
	   
	   double numPixels;
	   
	   numPixels=1.0*rows*cols;
	   std::cerr << "Total num pixels: " << numPixels << std::endl;
	   
	   ImageDifferencePcnt = difference*100.0/numPixels;	   
	   std::cerr << "Image differ in (%): " << ImageDifferencePcnt << std::endl;
	   
	   DestroyMagickWand(compare_wand);
	   
	   DestroyMagickWand(magick_wand_1);
	   DestroyMagickWand(magick_wand_2);
	   MagickWandTerminus();
	   
	   return ImageDifferencePcnt;
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
