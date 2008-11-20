//  Copyright (c) 2008 Michael Miceli and Christopher Miceli
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef MF_MAPREDUCEBASE_HPP
#define MF_MAPREDUCEBASE_HPP

#include <vector>
#include <string>
#include <time.h>
#include <saga/saga.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>

#include "parseCommand.hpp"
#include "unorderedMap.hpp"
#include "../utils/LogWriter.hpp"
#include "../utils/defines.hpp"
#include "RunMap.hpp"
#include "RunReduce.hpp"
#include "SystemInfo.hpp"

namespace MapReduce {
   template<typename Derived>
   class MapReduceBase {
     public:
      /*********************************************************
       * Parses the arguments and pulls out the database       *
       * to use and the session to use.                        *
       * ******************************************************/
      MapReduceBase(int argCount, char **argList) {
         boost::program_options::variables_map vm;
         try {
            if(!parseCommand(argCount, argList, vm))
               throw saga::exception("Incorrect command line arguments", saga::BadParameter);
         }
         catch(saga::exception const & e) {
            throw;
         }
         sessionUUID_  = (vm["session"].as<std::string>());
         database_     = (vm["database"].as<std::string>());
         logURL_       = (vm["log"].as<std::string>());
         outputPrefix_ = (vm["output"].as<std::string>());
         uuid_         = saga::uuid().string();//"MICHAELCHRIS";
         logWriter_    = new LogWriter(MR_WORKER_EXE_NAME, logURL_);
         int mode = saga::filesystem::Write | saga::filesystem::Create;
         for(int x=0;x<NUM_MAPS;x++) {
            saga::url mapFile(outputPrefix_ + "/mapFile_" + boost::lexical_cast<std::string>(x) + "_" + uuid_);
            saga::filesystem::file f(mapFile, mode);
            mapFiles_.push_back(f);
            saga::url reduceFile(outputPrefix_ + "/mapFile-reduce_" + boost::lexical_cast<std::string>(x) + "_" + uuid_);
            saga::filesystem::file g(reduceFile, mode);
            reduceFiles_.push_back(g);
         }
      }
      ~MapReduceBase() {
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
            std::cerr << "MapReduceBase::init : Exception caught : " << e.what() << std::endl;
            return -1;
         }   
         catch (...) {
            std::cerr << "MapReduceBase::init : Unknown exception occurred" << std::endl;
            return -1;
         }
         return 0;
      }
      /*********************************************************
       * The default hash function to split keys into different*
       * files after mapping.  No real reason to use this one, *
       * but it does the job.                                  *
       * ******************************************************/
      int hash(std::string const &input, unsigned int limit) {
         int sum = 0, retval;
         std::size_t length = input.length();
         for(std::size_t count = 0; count < length; count++) {
            sum = sum + input[count];
         }
         retval = (sum % limit);
         return retval;
      }


      void writeIntermediate(void) {
         unorderedMap::iterator mapIt = intermediate_.begin();
         unorderedMap::iterator end = intermediate_.end();
         std::string intermediateData[NUM_MAPS];
         while(mapIt != end) {
            std::string const &intermediateKey = mapIt->first;
            int hash_value = hash(intermediateKey, NUM_MAPS);
            intermediateData[hash_value].append(intermediateKey);
            intermediateData[hash_value].append(" ");
            intermediateData[hash_value].append((*(mapIt->second))[0]);
            std::size_t size = mapIt->second->size();
            for(unsigned int x = 1; x < size; x++) {
               intermediateData[hash_value].append(", ");
               intermediateData[hash_value].append((*(mapIt->second))[x]);
            }
            mapIt++;
            intermediateData[hash_value].append(";\n");
         }
         for(int counter = 0; counter < NUM_MAPS; counter++)
         {
            mapFiles_[counter].write(saga::buffer(intermediateData[counter], intermediateData[counter].length()));
         }
         intermediate_.clear();
      }
      /*********************************************************
       * emitIntermediate is called inside the map function and*
       * handles writing the key value pairs to proper files   *
       * and advertising these files.                          *
       * ******************************************************/
      void emitIntermediate(std::string const &key, std::string const &value) {
         unorderedMap::iterator mapIt = intermediate_.begin();
         unorderedMap::iterator end   = intermediate_.end();

         if(intermediate_.empty() == true) {
            intermediate_.rehash(MAX_INTERMEDIATE_SIZE);
         }
         if(intermediate_.find(key) == intermediate_.end()) {
            //Not in structure
            strVectorPtr initialValue(new std::vector<std::string>);
            initialValue->push_back(value);
            intermediate_[key] = initialValue;
         }
         else {
               //Contained in structure
               intermediate_[key]->push_back(value);
         }
         if(intermediate_.size() >= MAX_INTERMEDIATE_SIZE) {
            writeIntermediate();
         }
      }
      /*********************************************************
       * emit is called from inside the reduce function and    *
       * handles taking the output from reduce and writing it  *
       * to the proper file.                                   *
       * ******************************************************/
      void emit(std::string const &key, std::string const& value) {
         int hash_value = hash(key, NUM_MAPS);
         reduceValueMessages_[hash_value] += key;
         reduceValueMessages_[hash_value] += " " + value + "\n";
         for(int counter = 0; counter < NUM_MAPS; counter++) {
            if(reduceValueMessages_[counter].length() > 20000)
            {
               reduceFiles_[hash_value].write(saga::buffer(reduceValueMessages_[counter], reduceValueMessages_[counter].length()));
               reduceValueMessages_[counter].clear();
            }
         }
      }
     private:
      Derived& derived() {
         return static_cast<Derived&>(*this);
      }
      std::string uuid_;
      std::string sessionUUID_;
      std::string database_;
      std::string outputPrefix_;
      std::string reduceValueMessages_[3];
      saga::url   logURL_;
   
      time_t startupTime_;
      SystemInfo systemInfo_;
   
      std::vector<saga::filesystem::file> mapFiles_;
      std::vector<saga::filesystem::file> reduceFiles_;
      unorderedMap intermediate_;
      saga::advert::directory workerDir_;
      saga::advert::directory intermediateDir_;
      saga::advert::directory chunksDir_;
      saga::advert::directory reduceInputDir_;
      MapReduce::LogWriter * logWriter_;
   
      /*********************************************************
       * updateStatus_ updates the attributes in the database  *
       * to allow the master to know keepalive information.    *
       * ******************************************************/
      void updateStatus_(void) {
         //(1) update the last seen (keep alive) timestamp 
         time_t timestamp; time(&timestamp);
         try {
           workerDir_.set_attribute(ATTR_LAST_SEEN, 
             boost::lexical_cast<std::string>(timestamp)); 
         }
         catch(saga::exception const & e) {
           std::cerr << "FAILED (" << e.get_message() << ")" << std::endl;
           throw;
         }
         //(2) update the current load average
         try {
           workerDir_.set_attribute(ATTR_HOST_LOAD_AVG, 
                                           systemInfo_.hostLoadAverage());
         }
         catch(saga::exception const & e) {
           std::cerr << "FAILED (" << e.get_message() << ")" << std::endl;
           throw;
         }
         //(3) update execution status
         std::cerr << "SUCCESSFUL" << std::endl;
      }
      /*********************************************************
       * Removes all temporary advert entries                  *
       * ******************************************************/
      void cleanup_(void) {

      }
      /*********************************************************
       * registerWithDB connects to the advert database and    *
       * creates all necessary directories and creates         *
       * attributes describing this session.                   *
       * ******************************************************/
      void registerWithDB(void) {
         int mode = saga::advert::ReadWrite;
         //(1) connect to the orchestrator database
         std::string advertKey(database_ + "//" + sessionUUID_ + "/");
         try {
            saga::advert::directory(advertKey, mode);
            //(2a) create a directory for this agent
            advertKey += ADVERT_DIR_WORKERS;
            advertKey += "/" + uuid_ + "/";
            workerDir_    = saga::advert::directory(advertKey, mode | saga::advert::Create);
            intermediateDir_ = workerDir_.open_dir(saga::url(ADVERT_DIR_INTERMEDIATE), mode | saga::advert::Create);
            chunksDir_       = workerDir_.open_dir(saga::url(ADVERT_DIR_CHUNKS)      , mode | saga::advert::Create);
            reduceInputDir_  = workerDir_.open_dir(saga::url(ADVERT_DIR_REDUCE_INPUT), mode | saga::advert::Create);
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
         }
         catch(saga::exception const & e) {
            std::cerr << "FAILED (" << e.get_message() << ")" << std::endl;
            throw;
         }
      }
      /*********************************************************
       * mainLoop loops constantly checking the advert database*
       * for commands and begins working when a proper command *
       * discovered.                                           *
       * ******************************************************/
      void mainLoop(unsigned int updateInterval) {
         Derived& d = derived();
         while(1) {
            std::string command(getFrontendCommand_());
            //(1) read command from orchestrator
            if(command == WORKER_COMMAND_MAP) {
               // Use the RunMap class to handle details of getting
               // and retrieving necessary information from the master.
               try {
                  RunMap mapHandler(workerDir_, chunksDir_, intermediateDir_, outputPrefix_, uuid_);
                  d.map(mapHandler.getFile()); // Map the file given from the master
                  writeIntermediate();
               }
               catch(saga::exception const& e) {
                  std::cerr << "FAILED (" << e.get_message() << ")" << std::endl;
                  workerDir_.set_attribute("STATE", WORKER_STATE_FAIL);
               }
               //std::vector<std::string> output(mapHandler.getOutput());
            }
            else if(command == WORKER_COMMAND_REDUCE) {
               try {
                  // Use the RunReduce class to handle details of getting
                  // and retrieving necessary information from the master.
                  closeMapFiles();
                  RunReduce reduceHandler(workerDir_, reduceInputDir_, outputPrefix_);
                 
                  // Get a map of keys and a vector of the values
                  unorderedMap keyValues;
                  reduceHandler.getLines(keyValues);
                  unorderedMap::const_iterator  keyValuesIT = keyValues.begin();
                  // Iterate over these keys and their values and
                  // reduce them by passing them to the user defined
                  // reduce function
                  while(keyValuesIT != keyValues.end()) {
                     d.reduce(keyValuesIT->first, *(keyValuesIT->second));
                     ++keyValuesIT;
                  }
                  for(int counter = 0; counter < NUM_MAPS; counter++) {
                     reduceFiles_[counter].write(saga::buffer(reduceValueMessages_[counter], reduceValueMessages_[counter].length()));
                     reduceValueMessages_[counter].clear();
                  }
               }
               catch(saga::exception const& e) {
                  std::cerr << "FAILED (" << e.get_message() << ")" << std::endl;
                  workerDir_.set_attribute("STATE", WORKER_STATE_FAIL);
               }
            }
            else if(command == WORKER_COMMAND_DISCARD) {
               cleanup_();
            }
            else if(command == WORKER_COMMAND_RESUME) {
            }
            else if(command == WORKER_COMMAND_PAUSE) {
            }
            else if(command == WORKER_COMMAND_QUIT)
            {
               closeReduceFiles();
               cleanup_();
               return;
            }
            //(2) write some statistics + ping signal 
            updateStatus_();
            //(3) sleep for a while
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
           std::cerr << "FAILED (" << e.get_error() << ")" << std::endl;
           throw;
         }
         // get command number & reset the attribute to "" 
         return commandString;
      }
      void closeMapFiles(void) {
         static bool closed = false;
         if(closed == false)
         {
            std::vector<saga::filesystem::file>::iterator IT = mapFiles_.begin();
            while(IT != mapFiles_.end()) {
               IT->close();
               IT++;
            }
         }
         closed = true;
      }
      void closeReduceFiles(void) {
         std::vector<saga::filesystem::file>::iterator IT = reduceFiles_.begin();
         while(IT != reduceFiles_.end()) {
            IT->close();
            IT++;
         }
      }
   };
}

#endif //MF_MAPREDUCEBASE_HPP

