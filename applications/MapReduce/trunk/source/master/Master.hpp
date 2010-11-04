//  Copyright (c) 2008 Michael Miceli and Christopher Miceli
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef MR_MASTER_HPP
#define MR_MASTER_HPP

#include <saga/saga.hpp>
#include "ConfigFileParser.hpp"
#include "../utils/LogWriter.hpp"
#include "../utils/defines.hpp"
#include "version.hpp"
#include "HandleMaps.hpp"
#include "HandleReduces.hpp"
#include "parseCommand.hpp"

#define JOBS_PER_SERVICE 1

/***********************************
 * This is the Master class that   *
 * handles all details of the      *
 * framework.                      *
 * ********************************/
namespace MapReduce {
   namespace Master {
      template <class Derived>
      class Master {
        public:
         /***************************************
          * run starts the framework            *
          * by reading config and taking all    *
          * appropriate actions                 *
          * \param argC argc from main          *
          * \param argV argv from main          *
         ***************************************/
         Master(int argC, char **argV) {
            boost::program_options::variables_map vm;
            try {
               if (!parseCommand(argC, argV, vm))
                  throw saga::exception("Incorrect command line arguments", saga::BadParameter);
            }
            catch(saga::exception const& e) {
               throw;
            }
            std::string configFilePath (vm["config"].as<std::string>());
            MapReduce::LogWriter *initialLogger = new MapReduce::LogWriter(std::string(MR_MASTER_EXE_NAME), *(new saga::url("")));
            cfgFileParser_ = ConfigFileParser(configFilePath, *initialLogger);
            database_      = cfgFileParser_.getSessionDescription().orchestrator;
            serverURL_     = cfgFileParser_.getMasterAddress();
            outputPrefix_  =  cfgFileParser_.getOutputPrefix();
            // create a UUID for this agent
            uuid_ = std::string("MapReduce-") + saga::uuid().string();

            saga::url advertKey(std::string(database_ + "//" + uuid_ + "/log"));
            logURL_ = advertKey;
            //create new LogWriter instance that writes to advert
            log = new MapReduce::LogWriter(std::string(MR_MASTER_EXE_NAME), advertKey);
         }
         void run() {
            // generate a startup timestamp 
            time(&startupTime_);
            
            //Generate log information
            std::string message(std::string(MR_MASTER_EXE_NAME));
            message = message + " " + MR_MASTER_VERSION_FULL + " - creating new session.";
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

            //Send quit to all workers
            sendQuit_();

            log->write("All done - exiting normally", LOGLEVEL_INFO);
         }
         ~Master(void) {
            delete log;
         }
        private:
         time_t startupTime_;
         std::string uuid_;
         std::string database_;
         std::string serverURL_;
         saga::url   logURL_;

         std::vector<saga::url>      fileChunks_;
         saga::advert::directory     sessionBaseDir_;
         saga::advert::directory     workersDir_;
         saga::advert::directory     binariesDir_;
         saga::advert::directory     chunksDir_;
         std::string                 outputPrefix_;
         std::vector<saga::job::job> jobs_;
         
         MapReduce::LogWriter * log;
         ConfigFileParser cfgFileParser_;
         
         Derived& derived() {
            return static_cast<Derived&>(*this);
         }
         /*********************************************************
          * registerWithDB_ attempts to make a connection to the  *
          * database supplied in the config file, it does not     *
          * create anything.                                      *
          * ******************************************************/
         void registerWithDB_(void) {
            int mode = saga::advert::ReadWrite;  
            std::string message("Connecting to Orchestrator Database (");
            message += (database_) + ")... ";
            
            std::string advertKey(database_ + "//");
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
         void createNewSession_(void) {
            int mode = saga::advert::ReadWrite | saga::advert::Create;
            saga::task_container tc;

            std::string message("Creating a new session (");
            message += uuid_ + ")... ";
            std::string advertKey(database_ + "//" + uuid_ + "/");
            try {
               sessionBaseDir_ = saga::advert::directory(advertKey, mode);
               tc.add_task(sessionBaseDir_.set_attribute<saga::task_base::Async>("name",    cfgFileParser_.getSessionDescription().name));
               tc.add_task(sessionBaseDir_.set_attribute<saga::task_base::Async>("user",    cfgFileParser_.getSessionDescription().user));
               tc.add_task(sessionBaseDir_.set_attribute<saga::task_base::Async>("version", cfgFileParser_.getSessionDescription().version));
               saga::task t0 = sessionBaseDir_.open_dir<saga::task_base::Async>(saga::url(ADVERT_DIR_WORKERS),  mode); //workersDir_
               saga::task t1 = sessionBaseDir_.open_dir<saga::task_base::Async>(saga::url(ADVERT_DIR_BINARIES), mode); //binariesDir_
               saga::task t2 = sessionBaseDir_.open_dir<saga::task_base::Async>(saga::url(ADVERT_DIR_CHUNKS),   mode); //chunksDir_
               saga::task t3 = sessionBaseDir_.open<saga::task_base::Async>(saga::url(ADVERT_ENTRY_SERVER), mode);           //server address for worker
               tc.add_task(t0);
               tc.add_task(t1);
               tc.add_task(t2);
               tc.add_task(t3);
               tc.wait();
               workersDir_       = t0.get_result<saga::advert::directory>();
               binariesDir_      = t1.get_result<saga::advert::directory>();
               chunksDir_        = t2.get_result<saga::advert::directory>();
               saga::advert::entry address = t3.get_result<saga::advert::entry>();
               address.store_string(serverURL_);
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
                 saga::task t0 = adv.set_attribute<saga::task_base::Async>(ATTR_EXE_ARCH,    binaryListIT->targetArch);
                 saga::task t1 = adv.set_attribute<saga::task_base::Async>(ATTR_EXE_LOCATION,binaryListIT->URL);
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
            unsigned int successCounter = 0;
            int mode = saga::advert::Create | saga::advert::ReadWrite;
            std::vector<FileDescription> fileList                   = cfgFileParser_.getFileList();
            std::vector<FileDescription>::const_iterator fileListIT = fileList.begin();
            Derived& d = derived();

            
            // Translate FileDescriptions returned by getFileList
            // into names to be chunked by chunker
            while(fileListIT != fileList.end()) {
               std::vector<saga::url> temp;
               d.chunker(temp, fileListIT->name);
               for(std::vector<saga::url>::const_iterator tempIT = temp.begin();tempIT!=temp.end();tempIT++) {
                  fileChunks_.push_back(*tempIT);
               }
               std::cerr << "Number of chunks: " << temp.size() << std::endl;
               fileListIT++;
            }
            std::vector<saga::url>::const_iterator fileChunks_IT = fileChunks_.begin();
            // Advertise chunks
            while(fileChunks_IT != fileChunks_.end()) {
               std::string message("Adding new chunk " + (fileChunks_IT->get_string()) + "...");
               try {
                  saga::advert::entry adv = chunksDir_.open(saga::url("chunk-" + boost::lexical_cast<std::string>(successCounter)), mode);
                  adv.store_string(fileChunks_IT->get_string());
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
                        args.push_back("--session");
                        args.push_back(uuid_);
                        args.push_back("--database");
                        args.push_back(database_);
                        args.push_back("--log");
                        args.push_back(logURL_.get_string());
                        args.push_back("--output");
                        args.push_back(outputPrefix_);
                        jd.set_attribute(saga::job::attributes::description_executable, command);
                        jd.set_vector_attribute(saga::job::attributes::description_arguments, args);
                        saga::job::service js(hostListIT->rmURL);
                        for ( int i = 0; i < JOBS_PER_SERVICE; i++ )
                        {
                          saga::job::job agentJob= js.create_job(jd);
                          agentJob.run();
                          log->write(agentJob.get_job_id (), LOGLEVEL_INFO);
                          jobs_.push_back(agentJob); // Hack to prevent destructor of job object from being called
                        }
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
         void runMaps_(void) {
            std::string message("Launching maps...");
            HandleMaps mapHandler(fileChunks_, serverURL_, log);
   
            log->write(message, LOGLEVEL_INFO);
            mapHandler.assignMaps();
         }
         /*********************************************************
          * runReduces_ uses the helper class HandleReduces to    *
          * assign reduce jobs to running workers.  The class     *
          * handles grouping files and assigning them for the     *
          * master                                                *
          * ******************************************************/
         void runReduces_(void) {
            std::string message("Beginning Reduces...");
            HandleReduces reduceHandler(NUM_MAPS, workersDir_, serverURL_, log);
   
            log->write(message, LOGLEVEL_INFO);
            reduceHandler.assignReduces();
         }
         void sendQuit_(void) {
            char buff[255];
            int successCounter = 0;
            int workersSize = workersDir_.list("*").size();
            std::vector<saga::url> list = workersDir_.list("*");
            try {
               while(successCounter < workersSize)
               {
                  saga::stream::server *service = new saga::stream::server(serverURL_);
                  saga::stream::stream worker = service->serve(25);
                  std::string message("Established connection to ");
                  message += worker.get_url().get_string();
                  log->write(message, LOGLEVEL_INFO);
                  worker.write(saga::buffer(WORKER_COMMAND_QUIT, 4));
                  saga::ssize_t read_bytes = worker.read(saga::buffer(buff));
                  if(std::string(buff, read_bytes) != WORKER_RESPONSE_ACKNOLEDGE)
                  {
                     log->write(std::string("Misbehaving worker!"), LOGLEVEL_WARNING);
                  }
                  else
                  {
                     successCounter++;
                  }
                  service->close();
                  delete service;
               }
            }
            catch(saga::exception const & e) {
               std::cerr << e.what() << std::endl;
            }
         }
      };
   } // namespace Master
} // namespace MapReduce

#endif //MR_MASTER_HPP

