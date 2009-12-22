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
#include "HandleComparisons.hpp"
#include "HandleStaging.hpp"
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
            staging_ = vm.count("staging") ? true : false;
            AllPairs::LogWriter *initialLogger = new AllPairs::LogWriter(std::string(AP_MASTER_EXE_NAME), *(new saga::url("")));
            cfgFileParser_ = ConfigFileParser(configFilePath, *initialLogger);
            database_      = cfgFileParser_.getSessionDescription().orchestrator;
            serverURL_     = saga::url(cfgFileParser_.getMasterAddress());
            
            // create a UUID for this agent
            uuid_ = std::string("AllPairs-") + saga::uuid().string();

            saga::url advertKey(std::string(database_ + "//" + uuid_ + "/log"));
            logURL_ = advertKey;
            //create new LogWriter instance that writes to advert
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
            
            // then advertise the chunk on the DB
            populateFileList_();

            // Launch all worker command on all
            // host defined in config file
            spawnAgents_();

            //Handle Staging
            Graph network;
            if(staging_)
            {
               network = runStaging_();
            }

            // Start comparing sources to targets 
            runComparisons_(network);

            //Tell all workers to quit
            sendQuit_();

            log->write("All done - exiting normally", LOGLEVEL_INFO);
         }
         ~Master(void) {
            delete log;
         }
        private:
         bool        staging_;
         time_t      startupTime_;
         std::string uuid_;
         std::string database_;
         saga::url   serverURL_;
         saga::url   logURL_;

         assignmentChunksVector                 assignments_;
         saga::advert::directory                sessionBaseDir_;
         saga::advert::directory                workersDir_;
         saga::advert::directory                binariesDir_;
         saga::advert::directory                FilesDir_;
         std::map<int, std::vector<saga::url> > Files_;
         std::vector<saga::job::job>            jobs_;
         
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
          * session (AllPairs instance).                         *
          * ******************************************************/
         void createNewSession_(void) {
            int mode = saga::advert::ReadWrite | saga::advert::Create;
            saga::task_container tc;

            std::string message("Creating a new session (");
            message += uuid_ + ")... ";
            std::string advertKey(database_ + "//" + uuid_ + "/");
            try {
               sessionBaseDir_ = saga::advert::directory(advertKey, mode);
               tc.add_task(sessionBaseDir_.set_attribute<saga::task_base::ASync>("name",    cfgFileParser_.getSessionDescription().name));
               tc.add_task(sessionBaseDir_.set_attribute<saga::task_base::ASync>("user",    cfgFileParser_.getSessionDescription().user));
               tc.add_task(sessionBaseDir_.set_attribute<saga::task_base::ASync>("version", cfgFileParser_.getSessionDescription().version));
               saga::task t0 = sessionBaseDir_.open_dir<saga::task_base::ASync>(saga::url(ADVERT_DIR_WORKERS),   mode); //workersDir_
               saga::task t1 = sessionBaseDir_.open_dir<saga::task_base::ASync>(saga::url(ADVERT_DIR_BINARIES),  mode); //binariesDir_
               saga::task t2 = sessionBaseDir_.open_dir<saga::task_base::ASync>(saga::url(ADVERT_DIR_FILES), mode);     //Dir_
               saga::task t3 = sessionBaseDir_.open<saga::task_base::ASync>(saga::url(ADVERT_ENTRY_SERVER), mode);      //server address for worker
               tc.add_task(t0);
               tc.add_task(t1);
               tc.add_task(t2);
               tc.wait();
               workersDir_   = t0.get_result<saga::advert::directory>();
               binariesDir_  = t1.get_result<saga::advert::directory>();
               FilesDir_     = t2.get_result<saga::advert::directory>();
               saga::advert::entry address = t3.get_result<saga::advert::entry>();
               std::cout << "About to store our version of servuer url (string) " << serverURL_.get_string() << std::endl;
               address.store_string(serverURL_.get_string());
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
            unsigned int successCounter = 0;
            std::vector<std::vector<CompareDescription> > compareDescriptions = cfgFileParser_.getCompareList();
            std::vector<std::vector<CompareDescription> >::iterator it = compareDescriptions.begin();
            std::vector<std::vector<CompareDescription> >::iterator end = compareDescriptions.end();
            int assignmentChunkID = 0;
            int fileID = 0;
            while(it != end)
            {
               std::string message("Reading assignment " + boost::lexical_cast<std::string>(assignmentChunkID) + "...");
               log->write(message, LOGLEVEL_INFO);
               AssignmentChunk temp(assignmentChunkID);

               std::vector<CompareDescription>::iterator innerIt = it->begin();
               std::vector<CompareDescription>::iterator innerEnd = it->end();
               while(innerIt != innerEnd)
               {
                  message.clear();
                  message =  "    (" + innerIt->source + ", ";
                  message += innerIt->target + ")";
                  int sourceID, targetID;
                  bool found = false;
                  std::map<int, std::vector<saga::url> >::iterator FilesIt  = Files_.begin();
                  std::map<int, std::vector<saga::url> >::iterator FilesEnd = Files_.end();
                  while(FilesIt != FilesEnd)
                  {
                     if(FilesIt->second[0].get_string() == innerIt->source)
                     {
                        found = true;
                        break;
                     }
                     ++FilesIt;
                  }
                  if(found == false)
                  {
                     std::vector<saga::url> tempVector;
                     tempVector.push_back(innerIt->source);
                     std::pair<int, std::vector<saga::url> > temp(fileID, tempVector);
                     Files_.insert(temp);
                     sourceID = fileID;
                     fileID++;
                     successCounter++;
                  }
                  else
                  {
                     sourceID = FilesIt->first;
                  }
                  found = false;
                  FilesIt  = Files_.begin();
                  FilesEnd = Files_.end();
                  while(FilesIt != FilesEnd)
                  {
                     if(FilesIt->second[0].get_string() == innerIt->target)
                     {
                        found = true;
                        break;
                     }
                     ++FilesIt;
                  }
                  if(found == false)
                  {
                     std::vector<saga::url> tempVector;
                     tempVector.push_back(innerIt->target);
                     std::pair<int, std::vector<saga::url> > temp(fileID, tempVector);
                     Files_.insert(temp);
                     targetID = fileID;
                     fileID++;
                     successCounter++;
                  }
                  else
                  {
                     targetID = FilesIt->first;
                  }
                  Assignment assignmentTemp(sourceID, targetID);
                  //Assignment assignmentTemp(innerIt->fragments, innerIt->bases);
                  temp.push_back(assignmentTemp);
                  log->write(message, LOGLEVEL_INFO);
                  ++innerIt;
               }
               //Try to describe chunk by locations
               assignments_.push_back(temp);
               ++assignmentChunkID;
               ++it;
            }
            if(successCounter == 0) {
               log->write("No files added for this session. Aborting", LOGLEVEL_FATAL);
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
                        int instances = boost::lexical_cast<int>(hostListIT->instances);
                        for(int i = 0; i < instances; i++)
                        {
                           std::string command(binaryListIT->URL);
                           std::string loc(saga::url(hostListIT->rmURL).get_host());
                           std::vector<std::string> args;
                           args.push_back("--session");
                           args.push_back(uuid_);
                           args.push_back("--hostname");
                           std::cerr << "PUSHING BACK HOSTNAME FOR WORKER AS: " << loc << std::endl;
                           args.push_back(loc);
                           args.push_back("--database");
                           args.push_back(database_);
                           args.push_back("--log");
                           args.push_back(logURL_.get_string());
                           jd.set_attribute(saga::job::attributes::description_executable, command);
                           jd.set_vector_attribute(saga::job::attributes::description_arguments, args);
                           saga::job::service js(hostListIT->rmURL);
                           saga::job::job agentJob= js.create_job(jd);
                           std::cout << hostListIT->rmURL << " ";
                           for(unsigned int x=0;x<args.size();x++) {
                              std::cout << args[x] << " ";
                           }
                           std::cout << std::endl;
                           agentJob.run();
                           jobs_.push_back(agentJob);
                           message += "SUCCESS";
                           log->write(message, LOGLEVEL_INFO);
                           successCounter++;
                           break; //Found correct binary, move to next host
                        }
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

         Graph runStaging_(void) {
            log->write("Running Staging...", LOGLEVEL_INFO);
            std::vector<std::string> hosts;
            std::vector<HostDescription> hostList = cfgFileParser_.getTargetHostList();
            for(std::vector<HostDescription>::iterator hostIt = hostList.begin(); hostIt != hostList.end(); ++hostIt)
            {
               std::string currentHost(saga::url(hostIt->rmURL).get_host());
               if(std::find(hosts.begin(), hosts.end(), currentHost) == hosts.end())
               {
                  hosts.push_back(currentHost);
               }
            }
            for(std::map<int, std::vector<saga::url> >::iterator fileIt = Files_.begin(); fileIt!= Files_.end(); ++fileIt)
            {
               for(std::vector<saga::url>::iterator sit = fileIt->second.begin(); sit != fileIt->second.end(); ++sit)
               {
                  std::string currentHost(sit->get_host());
                  if(std::find(hosts.begin(), hosts.end(), currentHost) == hosts.end())
                  {
                     hosts.push_back(currentHost);
                  }
               }
            }
            std::vector<std::string>::iterator it = hosts.begin();
            while(it != hosts.end())
            {
               std::cerr << "host: " << *it << std::endl;
               ++it;
            }
            HandleStaging stage(serverURL_, hosts, log);
            log->write("Success", LOGLEVEL_INFO);
            return stage.getNetwork();
         }

         void runComparisons_(Graph networkGraph) {
            HandleComparisons comparisonHandler(networkGraph, staging_, assignments_, Files_, serverURL_, log);
            std::string message("Running Comparisons ...");
            log->write(message, LOGLEVEL_INFO);
            comparisonHandler.assignWork();
            log->write("Success", LOGLEVEL_INFO);
         }

         void sendQuit_(void) {
            std::vector<saga::job::job>::iterator it = jobs_.begin();
            while(it != jobs_.end())
            {
               if(it->get_state() != saga::job::Done ||
                  it->get_state() != saga::job::Failed ||
                  it->get_state() != saga::job::Canceled)
               {
                  it->signal(9);
               }
            }
            /*saga::url url(serverURL_);
            unsigned int successCounter = 0;
            std::vector<saga::url> list = workersDir_.list("*");
            unsigned int workersSize = list.size();
            try {
               while(successCounter < workersSize)
               {
                  saga::stream::server *service = new saga::stream::server(url);
                  saga::stream::stream worker = service->serve(25);
                  std::string message("Established connection to ");
                  message += worker.get_url().get_string();
                  log->write(message, LOGLEVEL_INFO);
                  worker.write(saga::buffer(WORKER_COMMAND_QUIT, 4));
                  std::string read(network::read(worker));
                  network::expect(read, WORKER_RESPONSE_ACKNOLEDGE);
                  successCounter++;
                  service->close();
                  delete service;
               }
            }
            catch(saga::exception const & e) {
               std::cerr << e.what() << std::endl;
            }*/
         }
      };
   } // namespace Master
} // namespace AllPairs

#endif //MR_MASTER_HPP

