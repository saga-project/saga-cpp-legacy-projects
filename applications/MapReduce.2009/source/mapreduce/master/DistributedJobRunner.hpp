//  Copyright (c) 2009 Miklos Erdelyi
//  Copyright (c) 2008 Michael Miceli and Christopher Miceli
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef MAPREDUCE_MASTER_DISTRIBUTEDJOBRUNNER_HPP_
#define MAPREDUCE_MASTER_DISTRIBUTEDJOBRUNNER_HPP_

#include <saga/saga.hpp>
#include "ConfigFileParser.hpp"
#include "../mapreduce.hpp"
#include "../job.hpp"
#include "../../utils/LogWriter.hpp"

#define JOBS_PER_SERVICE 1
#define MR_MASTER_EXE_NAME  "mr_master"

namespace mapreduce { namespace master {

class DistributedJobRunner {
 public:
  DistributedJobRunner(const JobDescription& job) : job_(job) {}
  void Initialize(const std::string& config_file_path);
  void Run(MapReduceResult* result);
 private:
  time_t startupTime_;
  std::string uuid_;
  std::string database_;
  std::string serverURL_;
  saga::url   logURL_;
  std::vector<InputChunk*>  chunks_;
  // Which worker completed which chunk.
  std::map<std::string, saga::url> committed_chunks_;
  RawInputFormat* input_format_;
  saga::advert::directory     sessionBaseDir_;
  saga::advert::directory     workersDir_;
  saga::advert::directory     binariesDir_;
  saga::advert::directory     chunksDir_;
  std::vector<saga::job::job> jobs_;

  mapreduce::LogWriter * log;
  ConfigFileParser cfgFileParser_;
  const JobDescription& job_;


 /*********************************************************
  * registerWithDB_ attempts to make a connection to the  *
  * database supplied in the config file, it does not     *
  * create anything.                                      *
  * ******************************************************/
  void registerWithDB_(void);

 /*********************************************************
  * createaNewSession_ connects to database, then proceeds*
  * to create all necessary directories (binaries, chunks,*
  * workers) and set all attributes describing this       *
  * session (MapReduce instance).                         *
  * ******************************************************/
  void createNewSession_(void);

 /*********************************************************
  * populateBinariesList_ reads all described binaries    *
  * from the xml file and stores them internally and on   *
  * the database under the directory ADVERT_DIR_BINARIES  *
  * ******************************************************/
  void populateBinariesList_(void);

 // populateChunks_ takes a list of input files from the
 // JobDescription and passes them to be broken up by the
 // InputFormat.
 // These are then advertised in the ADVERT_DIR_CHUNKS dir
 // of the database.
  void populateChunks_();

  void executeJob();

 /*********************************************************
  * spawnAgents_ takes the host list and the binary list  *
  * from the config file and tries to match the proper    *
  * binary image to a host depending of parameters such as*
  * architecture and operating system.  Once a match has  *
  * been made, the binary is launched on the host, where  *
  * the binary is the worker who will carry out chores for*
  * the master                                            *
  * ******************************************************/
  void spawnAgents_(void);
  void sendQuit_(void);
};

}   // namespace master
}   // namespace mapreduce

#endif  // MAPREDUCE_MASTER_DISTRIBUTEDJOBRUNNER_HPP_
