//  Copyright (c) 2008 Michael Miceli and Christopher Miceli
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/lexical_cast.hpp>
#include <unistd.h>

#include "Master.hpp"
#include "version.hpp"
#include "parseCommand.hpp"
#include "HandleComparisons.hpp"
#include "../utils/defines.hpp"
#include <unistd.h>

using namespace AllPairs::Master;


/***************************************
 * Init function starts the framework  *
 * by reading config and taking all    *
 * appropriate actions                 *
 * \param argC argc from main          *
 * \param argV argv from main          *
***************************************/
Master::Master(int argC, char *argV[]) {
   //Parses the command line options passed from main
   //looking for --config to find config xml file
   boost::program_options::variables_map vm;
   parseCommand(argC, argV, vm);
/*   if (!parseCommand(argC, argV, vm))
      return -2;*/

   std::string configFilePath (vm["config"].as<std::string>());

   cfgFileParser_ = ConfigFileParser(configFilePath, *log);

   database_      = cfgFileParser_.getSessionDescription().orchestrator;

   // create a UUID for this agent
   //uuid_ = uuid().string();  //Temporarily disabled
   uuid_ = "DUMMY-UUID";

   saga::url advertKey(std::string("advert://" + database_ + "//" + uuid_ + "/log"));
   logURL_ = advertKey;
   log = new AllPairs::LogWriter(std::string(AP_MASTER_EXE_NAME), advertKey);
}

Master::~Master() {
   delete log;
}

void Master::run() {
   // generate a startup timestamp 
   time(&startupTime_);

   //Generate log information
   std::string message(std::string(AP_MASTER_EXE_NAME));
   message = message + " " + AP_MASTER_VERSION_FULL + " - creating new session.";
   log->write(message, LOGLEVEL_INFO);


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

   // Launch all worker command on all
   // host defined in config file
   spawnAgents_();

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
 * session (AllPairs instance).                         *
 * ******************************************************/
void Master::createNewSession_() { 
   int mode = saga::advert::ReadWrite | saga::advert::Create;  
   std::string advertKey("advert://");
   std::string message("Creating a new session (");
   saga::task_container tc;
  
   message += (uuid_) + ")... ";
   advertKey += database_ + "//" + uuid_ + "/";
   try {
      
      std::string file(cfgFileParser_.getFile());

      sessionBaseDir_ = saga::advert::directory(advertKey, mode);
      tc.add_task(sessionBaseDir_.set_attribute<saga::task_base::ASync>("name",    cfgFileParser_.getSessionDescription().name));
      tc.add_task(sessionBaseDir_.set_attribute<saga::task_base::ASync>("user",    cfgFileParser_.getSessionDescription().user));
      tc.add_task(sessionBaseDir_.set_attribute<saga::task_base::ASync>("version", cfgFileParser_.getSessionDescription().version));
      tc.add_task(sessionBaseDir_.set_attribute<saga::task_base::ASync>("file",    file));
      saga::task t0 = sessionBaseDir_.open_dir<saga::task_base::ASync>(saga::url(ADVERT_DIR_WORKERS),  mode); //workersDir_
      saga::task t1 = sessionBaseDir_.open_dir<saga::task_base::ASync>(saga::url(ADVERT_DIR_BINARIES), mode); //binariesDir_
      tc.add_task(t0);
      tc.add_task(t1);
      tc.wait();
      workersDir_  = t0.get_result<saga::advert::directory>();
      binariesDir_ = t1.get_result<saga::advert::directory>();
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
        saga::task t0 = adv.set_attribute<saga::task_base::ASync>(ATTR_EXE_ARCH,    (*binaryListIT).targetArch);
        saga::task t1 = adv.set_attribute<saga::task_base::ASync>(ATTR_EXE_LOCATION,(*binaryListIT).URL);
        t0.wait();
        t1.wait();
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
               args.push_back("-l");
               args.push_back(logURL_.get_string());
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

void Master::runComparisons_(void) {
   HandleComparisons comparisonHandler(workersDir_);
   std::string message("Launching comparisons...");

   log->write(message, LOGLEVEL_INFO);
   sleep(5); //In here temporarily to allow time for all jobs to create advert entries
   comparisonHandler.assignWork();
}

