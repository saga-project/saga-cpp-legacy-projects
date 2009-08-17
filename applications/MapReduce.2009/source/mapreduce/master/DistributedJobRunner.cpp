//  Copyright (c) 2009 Miklos Erdelyi
//  Copyright (c) 2008 Michael Miceli and Christopher Miceli
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "DistributedJobRunner.hpp"

#include <boost/lexical_cast.hpp>

#include "master/HandleMaps.hpp"
#include "master/HandleReduces.hpp"
#include "protocol.hpp"
#include "utils/LogWriter.hpp"

namespace mapreduce { namespace master {

void DistributedJobRunner::Initialize(const std::string& config_file_path) {
  boost::scoped_ptr<mapreduce::LogWriter> initialLogger(
    new mapreduce::LogWriter(std::string(MR_MASTER_EXE_NAME), *(new saga::url(""))));
  cfgFileParser_ = ConfigFileParser(config_file_path, *(initialLogger.get()));
  database_      = cfgFileParser_.getSessionDescription().orchestrator;
  serverURL_     = cfgFileParser_.getMasterAddress();
  // Modify job based on the configuration read.
  SetupJob();
  // create a UUID for this agent
  uuid_ = std::string("MapReduce-") + saga::uuid().string();

  saga::url advertKey(std::string(database_ + "//" + uuid_ + "/log"));
  logURL_ = advertKey;
  //create new LogWriter instance that writes to advert
  log = new mapreduce::LogWriter(std::string(MR_MASTER_EXE_NAME), advertKey);
}

void DistributedJobRunner::SetupJob() {
  // Set output base if output path have been specified by the user.
  const std::string& output_prefix = cfgFileParser_.getOutputBase();
  if (!FileOutputFormat::GetOutputPath(job_).empty() &&
    !output_prefix.empty()) {
    FileOutputFormat::SetOutputBase(job_, output_prefix);
  }
}

void DistributedJobRunner::Run(MapReduceResult* result) {
  // generate a startup timestamp 
  time(&startupTime_);

  //Generate log information
  std::string message(std::string(MR_MASTER_EXE_NAME));
  message = message + " " + SAGA_MAPREDUCE_FRAMEWORK_VERSION +
    " - creating new session.";
  log->write(message, MR_LOGLEVEL_INFO);

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

  // then advertise the chunks on the DB
  populateChunks_();

  // Launch all worker command on all
  // host defined in config file
  spawnAgents_();

  // Find workers that have registered back
  // with db, and try to give them some work.
  // After all maps are done, go through workers
  // and try to reduce output from mappping by
  // assigning tasks to some workers.
  executeJob();

  //Send quit to all workers
  sendQuit_();
  
  log->write("All done - exiting normally", MR_LOGLEVEL_INFO);
}

void DistributedJobRunner::registerWithDB_(void) {
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
     log->write(message, MR_LOGLEVEL_FATAL);
     APPLICATION_ABORT
  }
  message += "SUCCESS";
  log->write(message, MR_LOGLEVEL_INFO);
}

void DistributedJobRunner::createNewSession_(void) {
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
     saga::task t0 = sessionBaseDir_.open_dir<saga::task_base::ASync>(saga::url(ADVERT_DIR_WORKERS),  mode); //workersDir_
     saga::task t1 = sessionBaseDir_.open_dir<saga::task_base::ASync>(saga::url(ADVERT_DIR_BINARIES), mode); //binariesDir_
     saga::task t2 = sessionBaseDir_.open_dir<saga::task_base::ASync>(saga::url(ADVERT_DIR_CHUNKS),   mode); //chunksDir_
     saga::task t3 = sessionBaseDir_.open<saga::task_base::ASync>(saga::url(ADVERT_ENTRY_SERVER), mode);           //server address for worker
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
     log->write(message, MR_LOGLEVEL_FATAL);
     APPLICATION_ABORT
  }
  message += "SUCCESS";
  log->write(message, MR_LOGLEVEL_INFO);
}

void DistributedJobRunner::populateBinariesList_(void) {
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
      log->write(message, MR_LOGLEVEL_INFO);
      successCounter++;
    }
    catch(saga::exception const & e) {
      message += e.what();
      log->write(message, MR_LOGLEVEL_ERROR);
    }
    binaryListIT++;
  }
  if(successCounter == 0) {
     log->write("No binaries defined for this session. Aborting", MR_LOGLEVEL_FATAL);
       APPLICATION_ABORT;
  }
}

void DistributedJobRunner::populateChunks_() {
  unsigned int successCounter = 0;
  int mode = saga::advert::Create | saga::advert::ReadWrite;
  // Instantiate input format.
  const std::string& format_name = job_.get_input_format();
  input_format_ = InputFormatFactory::get_by_key(format_name);
  if (input_format_ == NULL) {
    throw saga::exception("Invalid input format specified");
  }
  chunks_ = input_format_->GetChunks(job_);
  std::vector<InputChunk*>::const_iterator chunk_it = chunks_.begin();
  std::string chunk_string;
  // Advertise chunks.
  while (chunk_it != chunks_.end()) {
    std::string message("Adding new chunk of length " +
      boost::lexical_cast<std::string>((*chunk_it)->GetLength()) + "...");
    try {
      saga::advert::entry adv = chunksDir_.open(saga::url("chunk-" + boost::lexical_cast<std::string>(successCounter)), mode);
      // Serialize chunk.
      chunk_string.clear();
      StringOutputStream sos(&chunk_string);
      input_format_->SerializeInputChunk(*chunk_it, &sos);
      adv.store_string(chunk_string);
      message += "SUCCESS";
      log->write(message, MR_LOGLEVEL_INFO);
      successCounter++;
    }
    catch(saga::exception const & e) {
      message += e.what();
      log->write(message, MR_LOGLEVEL_ERROR);
    }
    ++chunk_it;
  }
  if (successCounter == 0) {
    log->write("No chunks made for this session. Aborting", MR_LOGLEVEL_FATAL);
    APPLICATION_ABORT;
  }
}

void DistributedJobRunner::spawnAgents_(void) {
  std::vector<BinaryDescription> binaryList               = cfgFileParser_.getExecutableList();
  std::vector<HostDescription>  hostList                  = cfgFileParser_.getTargetHostList();
  std::vector<HostDescription>::const_iterator hostListIT = hostList.begin();
  unsigned int successCounter = 0;

  while (hostListIT != hostList.end()) {
    std::string message("Launching agent on host " + hostListIT->rmURL + "... ");
   saga::job::description jd;
   std::vector<BinaryDescription>::const_iterator binaryListIT = binaryList.begin();
   try {
      while (binaryListIT != binaryList.end()) {
         // Now try to find a matching binary for this host
         if (hostListIT->hostArch == binaryListIT->targetArch
         && hostListIT->hostOS   == binaryListIT->targetOS) {
            // Found one, now try to launch it with proper arguments
            std::string command(binaryListIT->URL);
            std::vector<std::string> args;
            args.push_back("-w");   // Let host act as a 'worker'.
            args.push_back("--session");
            args.push_back(uuid_);
            args.push_back("--database");
            args.push_back(database_);
            args.push_back("--log");
            args.push_back(logURL_.get_string());
            jd.set_attribute(saga::job::attributes::description_executable, command);
            jd.set_vector_attribute(saga::job::attributes::description_arguments, args);
            saga::job::service js(hostListIT->rmURL);
            for ( int i = 0; i < JOBS_PER_SERVICE; i++ )
            {
              saga::job::job agentJob = js.create_job(jd);
              agentJob.run();
              saga::job::state state = agentJob.get_state();
              // Check if job submission was successful.
              if (state != saga::job::Running) {
                std::cerr << "Couldn't launch worker on "
                          << hostListIT->rmURL << std::endl;
              } else {
                log->write(agentJob.get_job_id (), MR_LOGLEVEL_INFO);
                jobs_.push_back(agentJob); // Hack to prevent destructor of job object from being called
              }
            }
            message += "SUCCESS";
            log->write(message, MR_LOGLEVEL_INFO);
            successCounter++;
            break; //Found correct binary, move to next host
         }
         binaryListIT++;
      }
   }
   catch(saga::exception const & e) {
      message += e.what();
      log->write(message, MR_LOGLEVEL_ERROR);
   }
   hostListIT++;
  }
  if(successCounter == 0) {
    log->write("Couldn't launch any agents. Aborting", MR_LOGLEVEL_FATAL);
    APPLICATION_ABORT
  }
}


void DistributedJobRunner::executeJob() {
  std::string message("Launching maps...");
  HandleMaps* map_handler = new HandleMaps(job_, chunks_, committed_chunks_,
    input_format_, serverURL_, log);
  log->write(message, MR_LOGLEVEL_INFO);
  map_handler->assignMaps();
  // Must be destroyed to release stream connection.
  delete map_handler;

  message = "Beginning reduces...";
  HandleReduces reduce_handler(job_, committed_chunks_, workersDir_, serverURL_,
    log);
  log->write(message, MR_LOGLEVEL_INFO);
  reduce_handler.assignReduces();
}


void DistributedJobRunner::sendQuit_(void) {
  char buff[255];
  int successCounter = 0;
  std::vector<saga::url> list = workersDir_.list("*");
  int workersSize = list.size();
  try {
    while(successCounter < workersSize) {
      saga::stream::server *service = new saga::stream::server(serverURL_);
      saga::stream::stream worker = service->serve(25);
      std::string message("Established connection to ");
      message += worker.get_url().get_string();
      log->write(message, MR_LOGLEVEL_INFO);
      worker.write(saga::buffer(WORKER_COMMAND_QUIT, 4));
      saga::ssize_t read_bytes = worker.read(saga::buffer(buff));
      if(std::string(buff, read_bytes) != WORKER_RESPONSE_ACKNOWLEDGE) {
        log->write(std::string("Misbehaving worker!"), MR_LOGLEVEL_WARNING);
      } else {
        successCounter++;
      }
      service->close();
      delete service;
    }
  }
  catch(saga::exception const& e) {
    std::cerr << e.what() << std::endl;
  }
}

}   // namespace master
}   // namespace mapreduce
