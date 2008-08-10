//  Copyright (c) 2008 Michael Miceli and Christopher Miceli
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef MR_MASTER_HPP
#define MR_MASTER_HPP

#include <saga/saga.hpp>
#include <boost/program_options.hpp>
#include "ConfigFileParser.hpp"
#include "../xmlParser/xmlParser.h"
#include "../utils/LogWriter.hpp"
#include "../utils/defines.hpp"
#include "version.hpp"
#include "HandleComparisons.hpp"
#include "parseCommand.hpp"

/***********************************
 * This is the Master class that   *
 * handles all details of the      *
 * framework.                      *
 * ********************************/
namespace AllPairs {
   namespace Master {
      template <class Derived>
      class Master {
        public:
         /***************************************
          * Init function starts the framework  *
          * by reading config and taking all    *
          * appropriate actions                 *
          * \param argC argc from main          *
          * \param argV argv from main          *
         ***************************************/
         Master(int argC,char **argV) {
            //Parses the command line options passed from main
            //looking for --config to find config xml file
            boost::program_options::variables_map vm;
            try {
               if (!parseCommand(argC, argV, vm))
                  throw saga::exception("Incorrect command line arguments", saga::BadParameter);
            }
            catch(saga::exception const& e) {
               throw;
            }
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
         void run() {
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
            
            // Take input files from xml and 
            // then advertise the chunk on the DB
            populateFileList_();

            // Launch all worker command on all
            // host defined in config file
            spawnAgents_();
            runComparisons_();
            log->write("All done - exiting normally", LOGLEVEL_INFO);
         }
         ~Master(void) {
            delete log;
         }
        private:
         time_t startupTime_;
         std::string uuid_;
         std::string database_;
         saga::url logURL_;
        
         saga::advert::directory sessionBaseDir_;
         saga::advert::directory workersDir_;
         saga::advert::directory binariesDir_;
         saga::advert::directory baseFilesDir_;
         saga::advert::directory fragmentFilesDir_;
         std::vector<saga::url> fragmentFiles_;
         std::vector<saga::url> baseFiles_;
         
         AllPairs::LogWriter * log;
         ConfigFileParser cfgFileParser_;
         
         /*********************************************************
          * registerWithDB_ attempts to make a connection to the  *
          * database supplied in the config file, it does not     *
          * create anything.                                      *
          * ******************************************************/
         void registerWithDB_(void) {
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
         void createNewSession_(void) {
            int mode = saga::advert::ReadWrite | saga::advert::Create;  
            std::string advertKey("advert://");
            std::string message("Creating a new session (");
            saga::task_container tc;
           
            message += (uuid_) + ")... ";
            advertKey += database_ + "//" + uuid_ + "/";
            try {
               sessionBaseDir_ = saga::advert::directory(advertKey, mode);
               tc.add_task(sessionBaseDir_.set_attribute<saga::task_base::ASync>("name",    cfgFileParser_.getSessionDescription().name));
               tc.add_task(sessionBaseDir_.set_attribute<saga::task_base::ASync>("user",    cfgFileParser_.getSessionDescription().user));
               tc.add_task(sessionBaseDir_.set_attribute<saga::task_base::ASync>("version", cfgFileParser_.getSessionDescription().version));
               saga::task t0 = sessionBaseDir_.open_dir<saga::task_base::ASync>(saga::url(ADVERT_DIR_WORKERS),   mode);      //workersDir_
               saga::task t1 = sessionBaseDir_.open_dir<saga::task_base::ASync>(saga::url(ADVERT_DIR_BINARIES),  mode);      //binariesDir_
               saga::task t2 = sessionBaseDir_.open_dir<saga::task_base::ASync>(saga::url(ADVERT_BASE_DIR_FILES), mode);     //baseDir_
               saga::task t3 = sessionBaseDir_.open_dir<saga::task_base::ASync>(saga::url(ADVERT_FRAGMENT_DIR_FILES), mode); //fragmentDir_
               tc.add_task(t0);
               tc.add_task(t1);
               tc.add_task(t2);
               tc.add_task(t3);
               tc.wait();
               workersDir_       = t0.get_result<saga::advert::directory>();
               binariesDir_      = t1.get_result<saga::advert::directory>();
               baseFilesDir_     = t2.get_result<saga::advert::directory>();
               fragmentFilesDir_ = t3.get_result<saga::advert::directory>();
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
         void populateBinariesList_(void) {
            std::vector<BinaryDescription> binaryList                   = cfgFileParser_.getExecutableList();
            std::vector<BinaryDescription>::const_iterator binaryListIT = binaryList.begin();
            unsigned int successCounter = 0;
            int mode = saga::advert::ReadWrite | saga::advert::Create;

            while(binaryListIT != binaryList.end()) {
               std::string message("Adding new binary for "+ binaryListIT->targetOS + "/" 
                                   + binaryListIT->targetArch + " to session... ");
               try {
                 saga::advert::entry adv = binariesDir_.open(binaryListIT->targetOS+"_"+binaryListIT->targetArch, mode);
                 //Now set some properties of the binaries
                 saga::task t0 = adv.set_attribute<saga::task_base::ASync>(ATTR_EXE_ARCH,    binaryListIT->targetArch);
                 saga::task t1 = adv.set_attribute<saga::task_base::ASync>(ATTR_EXE_LOCATION,binaryListIT->URL);
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
          * populateFileList_ takes a list of input files from the*
          * config file and passes them to be broken up by the    *
          * chunker.  The output of chunker is a series of chunks *
          * that are then advertised in the ADVERT_DIR_CHUNKS dir *
          * of the database.                                      *
          ********************************************************/
         void populateFileList_(void) {
            std::vector<FileDescription> fileListFragment                   = cfgFileParser_.getFileListFragment();
            std::vector<FileDescription> fileListBase                       = cfgFileParser_.getFileListBase();
            std::vector<FileDescription>::const_iterator fileListBaseIT     = fileListBase.begin();
            std::vector<FileDescription>::const_iterator fileListFragmentIT = fileListFragment.begin();
            unsigned int successCounter = 0;

            //int mode = saga::advert::ReadWrite | saga::advert::Create;
            
            // Translate FileDescriptions returned by getFileList
            // into names to be chunked by chunker
            //std::cout << "About to add fragments and bases" << std::endl;
            while(fileListBaseIT != fileListBase.end()) {
               baseFiles_.push_back(saga::url(fileListBaseIT->name));
               //std::cout << "Adding " << fileListBaseIT->name << " to Base list" << std::endl;
               fileListBaseIT++;
            }
            while(fileListFragmentIT != fileListFragment.end()) {
               fragmentFiles_.push_back(saga::url(fileListFragmentIT->name));
               //std::cout << "Adding " << fileListFragmentIT->name << " to Fragment list" << std::endl;
               fileListFragmentIT++;
            }
            std::vector<saga::url>::const_iterator fragmentFiles_IT = fragmentFiles_.begin();
            std::vector<saga::url>::const_iterator baseFiles_IT     = baseFiles_.begin();
            // Advertise chunks
            while(baseFiles_IT != baseFiles_.end()) {
               std::string message("Adding new chunk base " + (baseFiles_IT->get_string()) + "...");
               try {
            //      saga::advert::entry adv = baseFilesDir_.open(saga::url("file-" + boost::lexical_cast<std::string>(successCounter)), mode);
            //      adv.store_string(baseFiles_IT->get_string());
                  message += "SUCCESS";
            //      log->write(message, LOGLEVEL_INFO);
                  successCounter++;
               }
               catch(saga::exception const & e) {
                  message += e.what();
                  log->write(message, LOGLEVEL_ERROR);
               }
               baseFiles_IT++;
               //log->write(message,LOGLEVEL_INFO);
            }
            if(successCounter == 0) {
               log->write("No base files added for this session. Aborting", LOGLEVEL_FATAL);
               APPLICATION_ABORT;
            }
            successCounter=0;
            // Advertise chunks
            while(fragmentFiles_IT != fragmentFiles_.end()) {
               std::string message("Adding new chunk fragment " + (fragmentFiles_IT->get_string()) + "...");
               try {
                  //saga::advert::entry adv = fragmentFilesDir_.open(saga::url("file-" + boost::lexical_cast<std::string>(successCounter)), mode);
                  //adv.store_string(fragmentFiles_IT->get_string());
                  message += "SUCCESS";
                 //log->write(message, LOGLEVEL_INFO);
                  successCounter++;
               }
               catch(saga::exception const & e) {
                  message += e.what();
                  log->write(message, LOGLEVEL_ERROR);
               }
               //log->write(message,LOGLEVEL_INFO);
               fragmentFiles_IT++;
            }
            if(successCounter == 0) {
               log->write("No fragment files added for this session. Aborting", LOGLEVEL_FATAL);
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
         void spawnAgents_(void) {
            std::vector<BinaryDescription> binaryList               = cfgFileParser_.getExecutableList();
            std::vector<HostDescription>  hostList                  = cfgFileParser_.getTargetHostList();
            std::vector<HostDescription>::const_iterator hostListIT = hostList.begin();
            unsigned int successCounter = 0;

            while(hostListIT != hostList.end()) {
               std::string message("Launching agent on host " + hostListIT->rmURL + "... ");
               saga::job::description jd;
               std::vector<BinaryDescription>::const_iterator binaryListIT = binaryList.begin();
               try {
                  while(binaryListIT != binaryList.end()) {
                     // Now try to find a matching binary for this host
                     if(hostListIT->hostArch == binaryListIT->targetArch
                     && hostListIT->hostOS   == binaryListIT->targetOS) {
                        // Found one, now try to launch it with proper arguments
                        std::string command(binaryListIT->URL);
                        std::vector<std::string> args;
                        args.push_back("-s");
                        args.push_back(uuid_);
                        args.push_back("-d");
                        args.push_back(database_);
                        args.push_back("-l");
                        args.push_back(logURL_.get_string());
                        jd.set_attribute(saga::job::attributes::description_executable, command);
                        jd.set_attribute(saga::job::attributes::description_interactive, saga::attributes::common_false);
                        jd.set_vector_attribute(saga::job::attributes::description_arguments, args);
                        saga::job::service js("any://" + hostListIT->rmURL);
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
         void runComparisons_(void) {
            HandleComparisons comparisonHandler(baseFiles_, fragmentFiles_, workersDir_, log);
            std::string message("Running Comparisons ...");
            log->write(message, LOGLEVEL_INFO);
            sleep(5); //In here temporarily to allow time for all jobs to create advert entries
            std::vector<std::string> data = comparisonHandler.assignWork();
         }
      };
   } // namespace Master
} // namespace AllPairs

#endif //MR_MASTER_HPP

