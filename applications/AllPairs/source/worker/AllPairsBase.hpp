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
         uuid_        = "MICHAELCHRIS";//saga::uuid().string();
         logWriter_ = new LogWriter(AP_WORKER_EXE_NAME, logURL_);
      }
      /*********************************************************
       * starts the worker and begins all neccessary setup with*
       * the database.                                        *
       * ******************************************************/
      int run(void) {
         try {
           registerWithDB(); //Connect and create directories in database
           mainLoop(5); //sleep interval of 5
         }
         catch (saga::exception const & e) {
            std::cerr << "AllPairs::run : Exception caught : " << e.what() << std::endl;
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
   
      time_t startupTime_;
      SystemInfo systemInfo_;
   
      saga::advert::directory workerDir_;
      saga::advert::directory sessionBaseDir_;
      saga::advert::directory filesDir_;
      std::vector<saga::url> files_;
      AllPairs::LogWriter * logWriter_;
      Derived& derived() {
         return static_cast<Derived&>(*this);
      }
      /*********************************************************
       * updateStatus_ updates the attributes in the database  *
       * to allow the master to know keepalive information.    *
       * ******************************************************/
      void updateStatus_(void) {
         std::cout << std::endl << "Updating agent status: " << std::flush;
         //(1) update the last seen (keep alive) timestamp 
         time_t timestamp; time(&timestamp);
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
      void cleanup_(void) {}
      /*********************************************************
       * registerWithDB connects to the advert database and    *
       * creates all necessary directories and creates         *
       * attributes describing this session.                   *
       * ******************************************************/
      void registerWithDB(void) {
         int mode = saga::advert::ReadWrite;
         std::cout << "Registering with OrchestratorDB: " << std::flush;
         //(1) connect to the orchestrator database
         std::string advertKey("advert://");
         advertKey += database_ + "//" + sessionUUID_ + "/";
         try {
            saga::advert::directory sessionBaseDir_(advertKey, mode);

            //(2a) create a directory for this agent
            advertKey += ADVERT_DIR_WORKERS;
            advertKey += "/" + uuid_ + "/";
            workerDir_    = saga::advert::directory(advertKey, mode | saga::advert::Create);
            workerDir_.set_attribute("COMMAND", "");
            workerDir_.set_attribute("STATE", WORKER_STATE_IDLE);
            //(3) add some initial system information
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
            //(4) set the last seen (keep alive) timestamp
            time_t timestamp; time(&timestamp);
            workerDir_.set_attribute(ATTR_LAST_SEEN, 
            boost::lexical_cast<std::string>(timestamp));
            saga::advert::directory filesDir_(sessionBaseDir_.open_dir(saga::url(ADVERT_DIR_FILES), saga::advert::ReadWrite));
            std::vector<saga::url> filesAdv(filesDir_.list());
            std::vector<saga::url>::iterator filesAdvIT = filesAdv.begin();
        /*    while(filesAdvIT != filesAdv.end())
            {
               saga::advert::entry adv(*filesAdvIT, saga::advert::ReadWrite);
               files_.push_back(saga::url(adv.retrieve_string()));
            }*/
            for(int counter = 0; counter < 100; counter++)
            {
               files_.push_back(saga::url("file://localhost//home/bane/saga-projects/applications/AllPairs/samples/files-"+
                                           boost::lexical_cast<std::string>(counter) + ".txt"));
               std::cerr << "Added file: " << files_[counter] << std::endl;
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
      void mainLoop(unsigned int updateInterval) {
         while(1) {
            std::string command(getFrontendCommand_());
            saga::url file;
            int counter = 0;;
            // read command from orchestrator
            if(command == WORKER_COMMAND_COMPARE) {
               saga::advert::entry adv(workerDir_.open(saga::url("./file"), saga::advert::ReadWrite));
               file = adv.retrieve_string();
               RunComparison ComparisonHandler = RunComparison(workerDir_, files_, logWriter_);
               while(ComparisonHandler.hasComparisons())
               {
                  saga::url item(ComparisonHandler.getComparisons());
                  std::cout << "temporary = " << item << std::endl;
                  compare(file, item);
                  std::cout << "Compared some" << std::endl;
                  counter++;
                  std::cout << "counter: " << counter << std::endl;
               }
            }
            // write some statistics + ping signal 
            updateStatus_();
            //(3) sleep for a while*/
            sleep(updateInterval);
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
         std::string commandString;
         try {
           commandString = workerDir_.get_attribute("COMMAND");
         }
         catch(saga::exception const & e) {
           std::cout << "FAILED (" << e.get_error() << ")" << std::endl;
           throw;
         }
         // get command number & reset the attribute to "" 
         return commandString;
      }
   };
}

#endif // AP_ALLPAIRSBASE_HPP

