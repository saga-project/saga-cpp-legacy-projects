 //  Copyright (c) 2008 Michael Miceli and Christopher Miceli
 // 
 //  Distributed under the Boost Software License, Version 1.0. (See accompanying 
 //  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 
#include "AllPairsBase.hpp"
//#include "../utils/type.hpp"
#include <fstream>
#include <Magick++.h>
 
 using namespace std;
 using namespace Magick;
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
	
	std::cerr << "Before copying the files... " << endl; 
   
	saga::size_t const KB64 = 1024*64; //64KB
	saga::size_t bytesRead;
	std::cerr << "about to open files (" << testUrl.get_string() << ", " << baseUrl.get_string() << ")" << std::endl;
	saga::filesystem::file test(testUrl, saga::filesystem::Read);
	std::cerr << "1opened!" << std::endl;
	saga::filesystem::file base    (baseUrl    , saga::filesystem::Read);
	std::cerr << "2opened!" << std::endl;
   
	char data[KB64+1];
	double minimum = -1.0;
	while((bytesRead = test.read(saga::buffer(data,KB64)))!=0) {
         testFile.write(data, bytesRead);
	}
	while((bytesRead = base.read(saga::buffer(data,KB64)))!=0) {
		baseFile.write(data, bytesRead);
	}

	std::cerr << "Before closing files" << endl;

    // Need to close them before I can actually do something

	testFile.close();
	baseFile.close();
	
	
	// This is/was another way to compare the files "imagemagick-free"
	/*
    string cmpCommand = string("cmp ") + testName + " " + baseName;
	
	int answer;
    answer = system( cmpCommand.c_str() );
   
    std::cerr << "cmp says: " << answer << endl;
	
	*/
	
	
	//double minimum = -1.0;
	// Construct the image object. Seperating image construction from the 
	// the read operation ensures that a failure to read the image file 
	// doesn't render the image object useless. 
	
	std::cerr << "Before Image magick" << endl;

	Image testImage;

	try { 
		// Read a file into image object 
		testImage.read( testName );
	} 
	catch( Exception &error_ ) 
    { 
      std::cerr << "Caught exception: " << error_.what() << endl; 
      return 1; 
    } 
	
	Image baseImage;
	
	try { 
		// Read a file into image object 
		baseImage.read( baseName );
	} 
	catch( Exception &error_ ) 
    { 
      std::cerr << "Caught exception: " << error_.what() << endl; 
      return 1; 
    } 
	
	bool answerCompare;
	try {
	answerCompare = baseImage.compare(testImage);
	}
	catch( Exception &error_ ) 
    { 
      std::cerr << "Caught exception: " << error_.what() << endl; 
      return 1; 
    } 
	
	std::cerr << "image.compare says: " << answerCompare << endl;
		
   /*
      saga::size_t const KB64 = 1024*64; //64KB
      saga::size_t bytesRead;
      std::cerr << "about to open files (" << fragmentUrl.get_string() << ", " << baseUrl.get_string() << ")" << std::endl;
      saga::filesystem::file fragment(fragmentUrl, saga::filesystem::Read);
      std::cerr << "1opened!" << std::endl;
      saga::filesystem::file base    (baseUrl    , saga::filesystem::Read);
      std::cerr << "2opened!" << std::endl;
      std::string fragment_string;
      std::string base_string;
      char data[KB64+1];
      double minimum = -1.0;
      while((bytesRead = fragment.read(saga::buffer(data,KB64)))!=0) {
         fragment_string += data;
      }
      std::cerr << "read fragment: " << fragment_string << std::endl;
      while((bytesRead = base.read(saga::buffer(data,KB64)))!=0) {
         base_string += data;
      }
      std::cerr << "read base: " << base_string << std::endl;
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
        std::cout << "new dist = " << distance << std::endl;
        if(minimum == -1.0 || minimum > distance) {
           minimum = distance;
        }
      } */
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
