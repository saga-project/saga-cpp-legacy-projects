//  Copyright (c) 2009 Miklos Erdelyi
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  Copyright (c) 2008 Michael Miceli and Christopher Miceli
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef MF_MAPREDUCEBASE_HPP
#define MF_MAPREDUCEBASE_HPP

#include <time.h>
#include <vector>
#include <string>
#include <saga/saga.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>
#include "../../utils/logging.hpp"
#include "../protocol.hpp"
#include "MapTaskRunner.hpp"
#include "ReduceTaskRunner.hpp"
#include "SystemInfo.hpp"

namespace mapreduce { namespace worker {

// Buffer size used for communication with the master.
#define MSG_BUFFER_SIZE 2048

// Read timeout used when reading from the master.
#define WORKER_READ_TIMEOUT 20

class WorkerThread {
 public:
  /*********************************************************
   * Parses the arguments and pulls out the database       *
   * to use and the session to use.                        *
   * ******************************************************/
  WorkerThread(boost::program_options::variables_map& vm) {
     sessionUUID_  = (vm["session"].as<std::string>());
     database_     = (vm["database"].as<std::string>());
     logURL_       = (vm["log"].as<std::string>());
     uuid_         = saga::uuid().string();
     state_        = WORKER_STATE_IDLE;
  }
  ~WorkerThread() {
  }
  /*********************************************************
   * starts the worker and begins all neccessary setup with*
   * the database.                                        *
   * ******************************************************/
  int Run(void) {
     try {
       registerWithDB(); //Connect and create directories in database
       mainLoop();
     }
     catch (saga::exception const & e) {
        LOG_ERROR << "MapReduceBase::init : Exception caught : " << e.what();
        std::string advertKey(database_ + "//" + sessionUUID_ + "/");
        state_ = WORKER_STATE_FAIL;
        throw;
     }   
     catch (...) {
        LOG_ERROR << "MapReduceBase::init : Unknown exception occurred";
        throw;
     }
     return 0;
  }

 private:
  DECLARE_LOGGER(WorkerThread);
  std::string  uuid_;
  std::string  sessionUUID_;
  saga::url    logURL_;
  std::string  database_;
  std::string  state_;
  JobDescription* job_;   // Current job being executed.
  boost::scoped_ptr<TaskDescription> task_;   // Current task being executed.
  std::string  chunk_id_;  // Chunk ID to map, set in getFrontendCommand
  std::string* raw_chunk_;  // Raw chunk to map, set in getFrontendCommand
  int          reduce_partition_;
  std::string  reduceValueMessages_[NUM_MAPS];
  saga::url    serverURL_;

  time_t startupTime_;
  SystemInfo systemInfo_;

  saga::advert::directory workerDir_;
  saga::advert::directory intermediateDir_;
  saga::advert::directory chunksDir_;
  saga::advert::directory reduceInputDir_;

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
       LOG_ERROR << "FAILED (" << e.get_message() << ")";
       throw;
     }
     //(2) update the current load average
     try {
       workerDir_.set_attribute(ATTR_HOST_LOAD_AVG, 
                                systemInfo_.hostLoadAverage());
     }
     catch(saga::exception const & e) {
       LOG_ERROR << "FAILED (" << e.get_message() << ")";
       throw;
     }
     //(3) update execution status
     LOG_DEBUG << "STATUS SUCCESSFULLY UPDATED";
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
        serverURL_ = saga::url(server_name.retrieve_object<std::string>());
        LOG_DEBUG << "SERVER_URL = " << serverURL_.get_string();
     }
     catch(saga::exception const & e) {
        LOG_ERROR << "FAILED (" << e.get_message() << ")";
        throw;
     }
  }
  /*********************************************************
   * mainLoop loops constantly checking the advert database*
   * for commands and begins working when a proper command *
   * discovered.                                           *
   * ******************************************************/
  void mainLoop();

  /*********************************************************
   * getFrontendCommand_ retrieves the command from the    *
   * database that was posted by the master. The command   *
   * describes which action to take, this command is given *
   * after all necessary information has been posted by the*
   * master, such as input files, etc.                     *
   * ******************************************************/
  std::string getFrontendCommand_(void);

 };

}   // namespace worker
}   // namespace mapreduce

#endif //MF_MAPREDUCEBASE_HPP
