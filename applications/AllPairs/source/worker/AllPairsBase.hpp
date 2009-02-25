//  Copyright (c) 2008 Michael Miceli and Christopher Miceli
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef AP_ALLPAIRSBASE_HPP
#define AP_ALLPAIRSBASE_HPP

#include <vector>
#include <string>
#include <time.h>
#include <saga/saga.hpp>
#include <boost/lexical_cast.hpp>
#include "../utils/LogWriter.hpp"
#include "../utils/defines.hpp"
#include "RunComparison.hpp"
#include "SystemInfo.hpp"
#include "parseCommand.hpp"

namespace AllPairs {
   template <class Derived>
   class AllPairsBase {
     public:
      /*********************************************************
       * init parses the arguments and pulls out the database  *
       * to use and the session to use.                        *
       * ******************************************************/
      AllPairsBase(int argCount,char **argList) {
         boost::program_options::variables_map vm;
         try {
            if(!parseCommand(argCount, argList, vm))
               throw saga::exception("Incorrect command line arguments", saga::BadParameter);
         }
         catch(saga::exception const& e) {
           throw;
         }
         sessionUUID_ = (vm["session"].as<std::string>());
         database_    = (vm["database"].as<std::string>());
         logURL_      = (vm["log"].as<std::string>());
         uuid_        = saga::uuid().string();
         logWriter_   = new LogWriter(AP_WORKER_EXE_NAME, logURL_);
         state_       = WORKER_STATE_IDLE;
         lastFinishedChunk_ = -1;
      }
      /*********************************************************
       * starts the worker and begins all neccessary setup with*
       * the database.                                        *
       * ******************************************************/
      int run(void) {
         try {
           registerWithDB(); //Connect and create directories in database
           mainLoop();
         }
         catch (saga::exception const & e) {
            std::cerr << "AllPairs::run : Exception caught : " << e.what() << std::endl;
            std::string advertKey(database_ + "//" + sessionUUID_ + "/");
            state_ = WORKER_STATE_FAIL;
            throw;
         }   
         catch (...) {
            std::cerr << "AllPairs::run: Unknown exception occurred" << std::endl;
            throw;
         }
         return 0;
      }
      ~AllPairsBase() {}
      double compare(saga::url object1, saga::url object2) {
         Derived& d = derived();
         double value;
         value = d.compare(object1, object2);
         return value;
      };
     private:
      std::string uuid_;
      std::string sessionUUID_;
      std::string logURL_;
      std::string database_;
      std::string state_;
      int lastFinishedChunk_;
      saga::url   serverURL_;
   
      time_t startupTime_;
      SystemInfo systemInfo_;
   
      saga::advert::directory workerDir_;
      saga::advert::directory resultDir_;
      saga::advert::directory sessionBaseDir_;
      std::vector<saga::url>  baseFiles_;
      AllPairs::LogWriter*    logWriter_;
      RunComparison*          runComparison_;
      assignmentChunk         chunk_;
      Derived& derived() {
         return static_cast<Derived&>(*this);
      }
      /*********************************************************
       * updateStatus_ updates the attributes in the database  *
       * to allow the master to know keepalive information.    *
       * ******************************************************/
      void updateStatus_(void) {
         std::cout << "Updating agent status: " << std::endl;
         //(1) update the last seen (keep alive) timestamp 
         time_t timestamp;
         time(&timestamp);
         try {
             workerDir_.set_attribute(ATTR_LAST_SEEN, 
             boost::lexical_cast<std::string>(timestamp)); 
         }
         catch(saga::exception const & e) {
           std::cout << "FAILED (" << e.get_error() << ")" << std::endl;
           throw;
         }
         //(2) update the current load average
         try {
           workerDir_.set_attribute(ATTR_HOST_LOAD_AVG, 
             systemInfo_.hostLoadAverage());
         }
         catch(saga::exception const & e) {
           std::cout << "FAILED (" << e.get_error() << ")" << std::endl;
           throw;
         }
         //(3) update execution status
         std::cout << "SUCCESSFUL" << std::endl;
      }
      void cleanup_(void) {
      }
      /*********************************************************
       * registerWithDB connects to the advert database and    *
       * creates all necessary directories and creates         *
       * attributes describing this session.                   *
       * ******************************************************/
      void registerWithDB(void) {
         int mode = saga::advert::ReadWrite;
         std::cout << "Registering with OrchestratorDB: " << std::flush;
         //(1) connect to the orchestrator database
         std::string advertKey(database_ + "//" + sessionUUID_ + "/");
         try {
            sessionBaseDir_ = saga::advert::directory(advertKey, mode);
            advertKey  += ADVERT_DIR_WORKERS;
            advertKey  += "/" + uuid_ + "/";
            workerDir_ = saga::advert::directory(advertKey, mode | saga::advert::Create);
            resultDir_ = workerDir_.open_dir(saga::url(ADVERT_DIR_RESULTS), mode | saga::advert::Create);

            // add some initial system information
            workerDir_.set_attribute(ATTR_CPU_COUNT, 
              boost::lexical_cast<std::string>(systemInfo_.hardwareInfo().nCpu));
            workerDir_.set_attribute(ATTR_CPU_TYPE, 
              boost::lexical_cast<std::string>(systemInfo_.hardwareInfo().cpuType));
            workerDir_.set_attribute(ATTR_CPU_SUBTYPE, 
              boost::lexical_cast<std::string>(systemInfo_.hardwareInfo().cpuSubtype));
            workerDir_.set_attribute(ATTR_CPU_FREQ, 
              boost::lexical_cast<std::string>(systemInfo_.hardwareInfo().cpuFrequency));
            workerDir_.set_attribute(ATTR_MEM_TOTAL, 
              boost::lexical_cast<std::string>(systemInfo_.hardwareInfo().totalMemory));
            workerDir_.set_attribute(ATTR_HOST_NAME,     systemInfo_.hostName());
            workerDir_.set_attribute(ATTR_HOST_TYPE,     systemInfo_.hostType());
            workerDir_.set_attribute(ATTR_HOST_LOAD_AVG, systemInfo_.hostLoadAverage());

            // set the last seen (keep alive) timestamp
            time_t timestamp; time(&timestamp);
            workerDir_.set_attribute(ATTR_LAST_SEEN, boost::lexical_cast<std::string>(timestamp));

            saga::advert::entry server_name(sessionBaseDir_.open(ADVERT_ENTRY_SERVER, mode));
            serverURL_ = saga::url(server_name.retrieve_string());
            std::cerr << "SERVER_URL = " << serverURL_.get_string() << std::endl;

            saga::advert::directory baseFilesDir_(sessionBaseDir_.open_dir(saga::url(ADVERT_DIR_BASE_FILES), saga::advert::ReadWrite));
            std::vector<saga::url> baseFilesAdv(baseFilesDir_.list());
            std::vector<saga::url>::iterator baseFilesAdvIT = baseFilesAdv.begin();

            int counter = 0;
            while(baseFilesAdvIT != baseFilesAdv.end())
            {
               saga::advert::entry adv(baseFilesDir_.open(*baseFilesAdvIT, saga::advert::ReadWrite));
               baseFiles_.push_back(saga::url(adv.retrieve_string()));
               std::cerr << "Added file: " << baseFiles_[counter] << std::endl;
               baseFilesAdvIT++;
               counter++;
            }
         }
         catch(saga::exception const & e) {
            std::cout << "FAILED (" << e.get_error() << ")" << std::endl;
            throw;
         }
      }
      /*********************************************************
       * mainLoop loops constantly checking the advert database*
       * for commands and begins working when a proper command *
       * discovered.                                           *
       * ******************************************************/
      void mainLoop() {
         int mode = saga::advert::ReadWrite;
         while(1) {
            std::string command(getFrontendCommand_());
            // read command from orchestrator
            if(command == WORKER_COMMAND_COMPARE) {
               state_ = WORKER_STATE_COMPARING;
               std::string resultString;
               double val;
               while(runComparison_->hasAssignment()) {
                  assignment asn(runComparison_->getAssignment());
                  val = compare(asn.first, asn.second);
                  resultString += "(" + asn.first;
                  resultString += + ", " + asn.second + "): ";
                  resultString += boost::lexical_cast<std::string>(val) + '\n';
               }
               std::cout << resultString;
               lastFinishedChunk_ = runComparison_->getChunkID();
               saga::url result(std::string("result-") + boost::lexical_cast<std::string>(lastFinishedChunk_));
               delete runComparison_;
               saga::advert::entry fin_adv(resultDir_.open(result, mode | saga::advert::Create));
               //finished, now write data to advert
               fin_adv.store_string(boost::lexical_cast<std::string>(resultString));
               state_ = WORKER_STATE_DONE;
            }
            else if(command == WORKER_COMMAND_QUIT) {
               cleanup_();
               return;
            }
            // write some statistics + ping signal 
            updateStatus_();
         }
      }
      /*********************************************************
       * getFrontendCommand_ retrieves the command from the    *
       * database that was posted by the master. The command   *
       * describes which action to take, this command is given *
       * after all necessary information has been posted by the*
       * master, such as input files, etc.                     *
       * ******************************************************/
      std::string getFrontendCommand_(void) {
         static int depth = 0;
         std::string commandString;
         char buff[255];
         try {
            saga::stream::stream server_(serverURL_);
            server_.connect();
            memset(buff, 0, 255);
            saga::ssize_t read_bytes = server_.read(saga::buffer(buff));
            std::string question(buff, read_bytes);
            if(question == MASTER_QUESTION_STATE)
            {
               server_.write(saga::buffer(state_), sizeof(state_));
               memset(buff, 0, 255);
               read_bytes = server_.read(saga::buffer(buff));
               question = std::string(buff, read_bytes);
               if(question == MASTER_QUESTION_ADVERT)
               {
                  std::string advert(workerDir_.get_url().get_string());
                  server_.write(saga::buffer(advert, advert.size()));
                  memset(buff, 0, 255);
                  read_bytes = server_.read(saga::buffer(buff));
                  question = std::string(buff, read_bytes);
                  if(question == WORKER_COMMAND_COMPARE)
                  {
                     server_.write(saga::buffer(WORKER_RESPONSE_ACKNOLEDGE, 10));
                     runComparison_ = new RunComparison(server_, logWriter_);
                     chunk_ = runComparison_->getAssignmentChunk();
                     return WORKER_COMMAND_COMPARE;
                  }
                  else if(question == WORKER_COMMAND_QUIT)
                  {
                     return WORKER_COMMAND_QUIT;
                  }
               }
               else if(question == MASTER_QUESTION_RESULT)
               {
                  std::string lastString = boost::lexical_cast<std::string>(lastFinishedChunk_);
                  server_.write(saga::buffer(lastString, lastString.size()));
                  memset(buff, 0, 255);
                  read_bytes = server_.read(saga::buffer(buff));
                  question = std::string(buff, read_bytes);
                  if(question == MASTER_REQUEST_IDLE)
                  {
                     state_ = WORKER_STATE_IDLE;
                  }
                  return getFrontendCommand_();
               }
               if(question == MASTER_REQUEST_IDLE)
               {
                  state_ = WORKER_STATE_IDLE;
                  server_.write(saga::buffer(WORKER_RESPONSE_ACKNOLEDGE, 10));
                  return getFrontendCommand_();
               }
               else
               {
                  APPLICATION_ABORT;
               }
            }
            else if(question == WORKER_COMMAND_QUIT)
            {
               server_.write(saga::buffer(WORKER_RESPONSE_ACKNOLEDGE, 10));
               return WORKER_COMMAND_QUIT;
            }
            else
            {
               APPLICATION_ABORT;
            }
         }
         catch(saga::exception const & e) {
            sleep(1);
            std::cout << "Couldn't connect, try again" << std::endl;
            if(depth > 20)
            {
               return WORKER_COMMAND_QUIT;
            }
            return getFrontendCommand_();
         }
         // get command number & reset the attribute to "" 
         return commandString;
      }
   };
}

#endif // AP_ALLPAIRSBASE_HPP

