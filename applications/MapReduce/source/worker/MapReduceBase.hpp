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
#include "unorderedMap.hpp"
#include "../utils/LogWriter.hpp"
#include "../utils/defines.hpp"
#include "RunReduce.hpp"
#include "SystemInfo.hpp"
#include "parseCommand.hpp"

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
         catch(saga::exception const& e) {
           throw;
         }
         sessionUUID_  = (vm["session"].as<std::string>());
         database_     = (vm["database"].as<std::string>());
         logURL_       = (vm["log"].as<std::string>());
         outputPrefix_ = (vm["output"].as<std::string>());
         uuid_         = saga::uuid().string();
         logWriter_    = new LogWriter(MR_WORKER_EXE_NAME, logURL_);
         state_        = WORKER_STATE_IDLE;
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
           mainLoop();
         }
         catch (saga::exception const & e) {
            std::cerr << "MapReduceBase::init : Exception caught : " << e.what() << std::endl;
            std::string advertKey(database_ + "//" + sessionUUID_ + "/");
            state_ = WORKER_STATE_FAIL;
            throw;
         }   
         catch (...) {
            std::cerr << "MapReduceBase::init : Unknown exception occurred" << std::endl;
            throw;
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
      std::string  uuid_;
      std::string  sessionUUID_;
      saga::url    logURL_;
      std::string  database_;
      std::string  state_;
      std::string  chunk_;  //File to map, set in getFrontendCommand
      int          lastReduce_;
      std::string  outputPrefix_;
      std::string  reduceValueMessages_[NUM_MAPS];
      saga::url    serverURL_;
      unorderedMap intermediate_;
      MapReduce::LogWriter *logWriter_;
   
      time_t startupTime_;
      SystemInfo systemInfo_;
   
      saga::advert::directory workerDir_;
      saga::advert::directory intermediateDir_;
      saga::advert::directory chunksDir_;
      saga::advert::directory reduceInputDir_;
      std::vector<saga::filesystem::file> mapFiles_;
      std::vector<saga::filesystem::file> reduceFiles_;
      Derived& derived() {
         return static_cast<Derived&>(*this);
      }
   
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
            saga::advert::directory master = saga::advert::directory(advertKey, mode);
            //(2a) create a directory for this agent
            advertKey += ADVERT_DIR_WORKERS;
            advertKey += "/" + uuid_ + "/";
            workerDir_       = saga::advert::directory(advertKey, mode | saga::advert::Create);
            intermediateDir_ = workerDir_.open_dir(saga::url(ADVERT_DIR_INTERMEDIATE), mode | saga::advert::Create);
            chunksDir_       = workerDir_.open_dir(saga::url(ADVERT_DIR_CHUNKS)      , mode | saga::advert::Create);
            reduceInputDir_  = workerDir_.open_dir(saga::url(ADVERT_DIR_REDUCE_INPUT), mode | saga::advert::Create);
            state_ = WORKER_STATE_IDLE;
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

            saga::advert::entry server_name(master.open(ADVERT_ENTRY_SERVER, mode));
            serverURL_ = saga::url(server_name.retrieve_string());
            std::cerr << "SERVER_URL = " << serverURL_.get_string() << std::endl;
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
      void mainLoop() {
         Derived& d = derived();
         while(1) {
            std::string command(getFrontendCommand_());
            //(1) read command from orchestrator
            if(command == WORKER_COMMAND_MAP) {
               try {
                  d.map(chunk_); // Map the file given from the master
                  writeIntermediate();
                  try {
                     int mode = saga::advert::Create | saga::advert::ReadWrite;
                     for(int count = 0; count < NUM_MAPS; count++) {
                        saga::advert::entry adv = intermediateDir_.open(saga::url("mapFile-"+boost::lexical_cast<std::string>(count)), mode);
                        saga::url fileurl(outputPrefix_ + "mapFile_" + boost::lexical_cast<std::string>(count) + "_" + uuid_);
                        adv.store_string(fileurl.get_string());
                     }
                     state_ = WORKER_STATE_DONE_MAP;
                  }
                  catch(saga::exception const & e) {
                     throw;
                  }
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
                  state_ = WORKER_STATE_DONE_REDUCE;
               }
               catch(saga::exception const& e) {
                  std::cerr << "FAILED (" << e.get_message() << ")" << std::endl;
                  state_ = WORKER_STATE_FAIL;
               }
            }
            else if(command == WORKER_COMMAND_DISCARD) {
               cleanup_();
            }
            else if(command == WORKER_COMMAND_QUIT)
            {
               closeReduceFiles();
               cleanup_();
               return;
            }
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
         char buff[255];
         try {
            saga::stream::stream server_(serverURL_);
            server_.connect();
            saga::ssize_t read_bytes = server_.read(saga::buffer(buff));
            std::string question(buff, read_bytes);
            std::cerr << "QUESTION = " << question << std::endl;
            if(question == MASTER_QUESTION_STATE) {
               server_.write(saga::buffer(state_, state_.size()));
               if(state_ == WORKER_STATE_IDLE) {
                  memset(buff, 0, 255);
                  read_bytes = server_.read(saga::buffer(buff));
                  question = std::string(buff, read_bytes);
                  if(question == MASTER_REQUEST_IDLE) {
                     server_.write(saga::buffer(WORKER_RESPONSE_ACKNOLEDGE, 10));
                     state_ = WORKER_STATE_IDLE;
                     //server_.close();
                     return std::string("");
                  }
                  else if(question == MASTER_QUESTION_ADVERT) {
                     std::string advert(workerDir_.get_url().get_string());
                     server_.write(saga::buffer(advert, advert.size()));
                     memset(buff, 0, 255);
                     read_bytes = server_.read(saga::buffer(buff));
                     question = std::string(buff, read_bytes);
                     if(question == WORKER_COMMAND_MAP) {
                        server_.write(saga::buffer(WORKER_RESPONSE_ACKNOLEDGE, 10));
                        memset(buff, 0, 255);
                        read_bytes = server_.read(saga::buffer(buff));
                        question = std::string(buff, read_bytes);
                        if(question == WORKER_CHUNK) {
                           server_.write(saga::buffer(WORKER_RESPONSE_ACKNOLEDGE, 10));
                           memset(buff, 0, 255);
                           read_bytes = server_.read(saga::buffer(buff));
                           //This is the actual chunk to read!
                           chunk_ = std::string(buff, read_bytes);
                           std::cerr << "just set chunk to " << chunk_ << std::endl;
                           server_.write(saga::buffer(WORKER_RESPONSE_ACKNOLEDGE, 10));
                           //server_.close();
                           std::cerr << "Returning command map" << std::endl;
                           return WORKER_COMMAND_MAP;
                        }
                     }
                     else if(question == WORKER_COMMAND_REDUCE) {
                        server_.write(saga::buffer(WORKER_RESPONSE_ACKNOLEDGE, 10));
                        memset(buff, 0, 255);
                        read_bytes = server_.read(saga::buffer(buff));
                        lastReduce_ = boost::lexical_cast<int>(std::string(buff, read_bytes));
                        server_.write(saga::buffer(WORKER_RESPONSE_ACKNOLEDGE, 10));
                        //server_.close();
                        std::cerr << "Returning command reduce" << std::endl;
                        return WORKER_COMMAND_REDUCE;
                     }
                  }
               }
               else if(state_ == WORKER_STATE_DONE_MAP) {
                  memset(buff, 0, 255);
                  read_bytes = server_.read(saga::buffer(buff));
                  question = std::string(buff, read_bytes);
                  if(question == MASTER_QUESTION_RESULT) {
                     server_.write(saga::buffer(chunk_, chunk_.size()));
                     memset(buff, 0, 255);
                     read_bytes = server_.read(saga::buffer(buff));
                     question = std::string(buff, read_bytes);
                     if(question == MASTER_REQUEST_IDLE) {
                        state_ = WORKER_STATE_IDLE;
                        //server_.close();
                        return std::string("");
                     }
                  }
                  else if(question == MASTER_REQUEST_IDLE) {
                     server_.write(saga::buffer(WORKER_RESPONSE_ACKNOLEDGE, 10));
                     state_ = WORKER_STATE_IDLE;
                     return std::string("");
                  }
               }
               else if(state_ == WORKER_STATE_DONE_REDUCE) {
                  memset(buff, 0, 255);
                  read_bytes = server_.read(saga::buffer(buff));
                  question = std::string(buff, read_bytes);
                  if(question == MASTER_QUESTION_RESULT) {
                     std::string last(boost::lexical_cast<std::string>(lastReduce_));
                     server_.write(saga::buffer(last, last.size()));
                     memset(buff, 0, 255);
                     read_bytes = server_.read(saga::buffer(buff));
                     question = std::string(buff, read_bytes);
                     if(question == MASTER_REQUEST_IDLE) {
                        //server_.close();
                        state_ = WORKER_STATE_IDLE;
                        return std::string("");
                     }
                  }
               }
            }
            if(question == WORKER_COMMAND_QUIT) {
               std::cerr << "GOT COMMAND TO QUIT, YIP YIP!" << std::endl;
               server_.write(saga::buffer(WORKER_RESPONSE_ACKNOLEDGE, 10));
               return WORKER_COMMAND_QUIT;
            }
         }
         catch(saga::exception const & e) {
            //Hope it was couldn't connect to stream server, then just wait and try again...
            sleep(1);
            if(depth < 20) {
               depth++;
               return getFrontendCommand_();
            }
            else throw;
         }
         return std::string("");
         // get command number & reset the attribute to "" 
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

