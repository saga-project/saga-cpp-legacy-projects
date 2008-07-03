//  Copyright (c) 2008 Michael Miceli and Christopher Miceli
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/lexical_cast.hpp>
#include <unistd.h>

#include "Master.hpp"
#include "HandleMaps.hpp"
#include "HandleReduces.hpp"
#include "version.hpp"
#include "parseCommand.hpp"
#include "../utils/defines.hpp"
#include "../utils/chunker.hpp"
#include <unistd.h>

using namespace MapReduce::Master;

// constructor: initialize member variables
Master::Master()
  : log(NULL)
{}

Master::~Master() {
   delete log;
}

/***************************************
 * Init function starts the framework  *
 * by reading config and taking all    *
 * appropriate actions                 *
 * \param argC argc from main          *
 * \param argV argv from main          *
***************************************/
int Master::init(int argC, char *argV[]) {
   //Parses the command line options passed from main
   //looking for --config to find config xml file
   boost::program_options::variables_map vm;
   if (!parseCommand(argC, argV, vm))
      return -2;
   std::string configFilePath (vm["config"].as<std::string>());

   //create new LogWriter instance that writes to stdout
   log = new MapReduce::LogWriter(std::string(MR_MASTER_EXE_NAME), std::cout);

   cfgFileParser_ = ConfigFileParser(configFilePath, *log);
   database_      = cfgFileParser_.getSessionDescription().orchestrator;
   // generate a startup timestamp 
   time(&startupTime_);
   // create a UUID for this agent
   //uuid_ = uuid().string();  //Temporarily disabled
   uuid_ = "DUMMY-UUID";

   //Generate log information
   std::string message(std::string(MR_MASTER_EXE_NAME));
   message = message + " " + MR_MASTER_VERSION_FULL + " - creating new session.";
   log->write(message,LOGLEVEL_INFO);
   message = "Using session description: " + configFilePath;
   log->write(message,LOGLEVEL_INFO);
   run_();
   return 0;
}

void Master::run_() {
   // register with the db
   // Just connect to see if it exists
   registerWithDB_();

   // create a new session
   // create all necessary directories
   createNewSession_();

   // add binaries to the Orchestrator DB
   // Take binaries from config file and
   // advertise them
   populateBinariesList_();

   // Take input files from xml and pass
   // them to be chunked into smaller files,
   // then advertise the chunk on the DB
   populateFileList_();

   // Launch all worker command on all
   // host defined in config file
   spawnAgents_();

   // Find workers that have registered back
   // with db, and try to give them some work
   runMaps_();

   // After all maps are done, go through workers
   // and try to reduce output from mappping by
   // assigning tasks to some workers
   runReduces_();

   log->write("All done - exiting normally", LOGLEVEL_INFO);
}
/*********************************************************
 * registerWithDB_ attempts to make a connection to the  *
 * database supplied in the config file, it does not     *
 * create anything.                                      *
 * ******************************************************/
void Master::registerWithDB_() { 
   int mode = saga::advert::ReadWrite;  
   std::string message("Connecting to Orchestrator Database (");
   message += (database_) + ")... ";
   
   std::string advertKey("advert://");
   advertKey += database_ + "//" ;
   try {
      //If this line succeeds, then there is a
      //connection to the database
      saga::advert::directory(advertKey, mode);
   }
   catch(saga::exception const & e) {
      message += e.what();
      log->write(message, LOGLEVEL_FATAL);
      APPLICATION_ABORT
   }
   message += "SUCCESS";
   log->write(message, LOGLEVEL_INFO);
}

/*********************************************************
 * createaNewSession_ connects to database, then proceeds*
 * to create all necessary directories (binaries, chunks,*
 * workers) and set all attributes describing this       *
 * session (MapReduce instance).                         *
 * ******************************************************/
void Master::createNewSession_() { 
   int mode = saga::advert::ReadWrite | saga::advert::Create;  
   std::string advertKey("advert://");
   std::string message("Creating a new session (");
  
   message += (uuid_) + ")... ";
   advertKey += database_ + "//" + uuid_ + "/";
   try {
      sessionBaseDir_ = saga::advert::directory(advertKey, mode);
      sessionBaseDir_.set_attribute("name",    cfgFileParser_.getSessionDescription().name);
      sessionBaseDir_.set_attribute("user",    cfgFileParser_.getSessionDescription().user);
      sessionBaseDir_.set_attribute("version", cfgFileParser_.getSessionDescription().version);
      workersDir_  = sessionBaseDir_.open_dir(saga::url(ADVERT_DIR_WORKERS),  mode);
      binariesDir_ = sessionBaseDir_.open_dir(saga::url(ADVERT_DIR_BINARIES), mode);
      chunksDir_   = sessionBaseDir_.open_dir(saga::url(ADVERT_DIR_CHUNKS),   mode);
   }
   catch(saga::exception const & e) {
      message += e.what();
      log->write(message, LOGLEVEL_FATAL);
      APPLICATION_ABORT
   }
   message += "SUCCESS";
   log->write(message, LOGLEVEL_INFO);
}

/*********************************************************
 * populateBinariesList_ reads all described binaries    *
 * from the xml file and stores them internally and on   *
 * the database under the directory ADVERT_DIR_BINARIES  *
 * ******************************************************/
void Master::populateBinariesList_(void) {
   std::vector<BinaryDescription> binaryList                   = cfgFileParser_.getExecutableList();
   std::vector<BinaryDescription>::const_iterator binaryListIT = binaryList.begin();
   unsigned int successCounter = 0;
   int mode = saga::advert::ReadWrite | saga::advert::Create;

   while(binaryListIT != binaryList.end()) {
      std::string message("Adding new binary for "+ (*binaryListIT).targetOS + "/" 
                          + (*binaryListIT).targetArch + " to session... ");
      try {
        saga::advert::entry adv = binariesDir_.open((*binaryListIT).targetOS+"_"+(*binaryListIT).targetArch, mode);
        //Now set some properties of the binaries
        adv.set_attribute(ATTR_EXE_ARCH,    (*binaryListIT).targetArch);
        adv.set_attribute(ATTR_EXE_LOCATION,(*binaryListIT).URL);
        message += "SUCCESS";
        log->write(message, LOGLEVEL_INFO);
        successCounter++;
      }
      catch(saga::exception const & e) {
         message += e.what();
         log->write(message, LOGLEVEL_ERROR);
      }
      binaryListIT++;
   }
   if(successCounter == 0) {
      log->write("No binaries defined for this session. Aborting", LOGLEVEL_FATAL);
      APPLICATION_ABORT;
   }
}

/*********************************************************
 * populateFileList_ takes a list of input files from the*
 * config file and passes them to be broken up by the    *
 * chunker.  The output of chunker is a series of chunks *
 * that are then advertised in the ADVERT_DIR_CHUNKS dir *
 * of the database.                                      *
 ********************************************************/
void Master::populateFileList_(void) {
   std::vector<FileDescription> fileList                   = cfgFileParser_.getFileList();
   std::vector<FileDescription>::const_iterator fileListIT = fileList.begin();
   std::vector<std::string> fileNameList;
   unsigned int successCounter = 0;
   int mode = saga::advert::ReadWrite | saga::advert::Create;
   
   // Translate FileDescriptions returned by getFileList
   // into names to be chunked by chunker
   while(fileListIT != fileList.end()) {
      fileNameList.push_back((*fileListIT).name);
      fileListIT++;
   }
   fileChunks_ = chunker<std::string>(fileNameList); //Defaults to 64 M chunks, fix later
   std::vector<std::string>::const_iterator fileChunks_IT = fileChunks_.begin();
   // Advertise chunks
   while(fileChunks_IT != fileChunks_.end()) {
      std::string message("Adding new chunk " + (*fileChunks_IT) + "...");
      try {
         saga::advert::entry adv = chunksDir_.open(saga::url("chunk-" + boost::lexical_cast<std::string>(successCounter)), mode);
         adv.store_string(*fileChunks_IT);
         message += "SUCCESS";
         log->write(message, LOGLEVEL_INFO);
         successCounter++;
      }
      catch(saga::exception const & e) {
         message += e.what();
         log->write(message, LOGLEVEL_ERROR);
       }
       fileChunks_IT++;
   }
   if(successCounter == 0) {
      log->write("No chunks made for this session. Aborting", LOGLEVEL_FATAL);
      APPLICATION_ABORT;
   }
}

/*********************************************************
 * spawnAgents_ takes the host list and the binary list  *
 * from the config file and tries to match the proper    *
 * binary image to a host depending of parameters such as*
 * architecture and operating system.  Once a match has  *
 * been made, the binary is launched on the host, where  *
 * the binary is the worker who will carry out chores for*
 * the master                                            *
 * ******************************************************/
void Master::spawnAgents_(void) {
   std::vector<BinaryDescription> binaryList               = cfgFileParser_.getExecutableList();
   std::vector<HostDescription>  hostList                  = cfgFileParser_.getTargetHostList();
   std::vector<HostDescription>::const_iterator hostListIT = hostList.begin();
   unsigned int successCounter = 0;

   while(hostListIT != hostList.end()) {
      std::string message("Launching agent on host " + (*hostListIT).rmURL + "... ");
      saga::job::description jd;
      std::vector<BinaryDescription>::const_iterator binaryListIT = binaryList.begin();
      try {
         while(binaryListIT != binaryList.end()) {
            // Now try to find a matching binary for this host
            if((*hostListIT).hostArch == (*binaryListIT).targetArch
            && (*hostListIT).hostOS   == (*binaryListIT).targetOS) {
               // Found one, now try to launch it with proper arguments
               std::string command((*binaryListIT).URL);
               std::vector<std::string> args;
               args.push_back("-s");
               args.push_back(uuid_);
               args.push_back("-d");
               args.push_back(database_);
               jd.set_attribute(saga::job::attributes::description_executable, command);
               jd.set_attribute(saga::job::attributes::description_interactive, saga::attributes::common_true);
               jd.set_vector_attribute(saga::job::attributes::description_arguments, args);
               saga::job::service js("any://" + (*hostListIT).rmURL);
               saga::job::job agentJob= js.create_job(jd);
               agentJob.run();
               message += "SUCCESS";
               log->write(message, LOGLEVEL_INFO);
               successCounter++;
               break; //Found correct binary, move to next host
            }
            binaryListIT++;
         }
      }
      catch(saga::exception const & e) {
         message += e.what();
         log->write(message, LOGLEVEL_ERROR);
      }
      hostListIT++;
   }
   if(successCounter == 0) {
      log->write("Couldn't launch any agents. Aborting", LOGLEVEL_FATAL);
      APPLICATION_ABORT
   }
}

/*********************************************************
 * runMaps_ uses the helper class HandleMaps to assign   *
 * chunks to a running worker and tell that worker to    *
 * begin working                                         *
 * ******************************************************/
void Master::runMaps_(void) {
   std::vector<saga::url> possibleWorkers(workersDir_.list("?"));
   HandleMaps mapHandler(fileChunks_, possibleWorkers);
   std::string message("Launching maps...");

   log->write(message, LOGLEVEL_INFO);
   sleep(5); //In here temporarily to allow time for all jobs to create advert entries
   mapHandler.assignMaps();
}

/*********************************************************
 * runReduces_ uses the helper class HandleReduces to    *
 * assign reduce jobs to running workers.  The class     *
 * handles grouping files and assigning them for the     *
 * master                                                *
 * ******************************************************/
void Master::runReduces_(void) {
   std::vector<saga::url> possibleWorkers(workersDir_.list("?"));
   HandleReduces reduceHandler(NUM_MAPS, possibleWorkers);
   std::string message("Beginning Reduces...");

   log->write(message, LOGLEVEL_INFO);
   reduceHandler.assignReduces();
}
