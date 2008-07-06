//  Copyright (c) 2008 Michael Miceli and Christopher Miceli
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "AllPairsBase.hpp"
#include "parseCommand.hpp"

using namespace AllPairs;
 
/*********************************************************
 * init parses the arguments and pulls out the database  *
 * to use and the session to use.                        *
 * ******************************************************/
int AllPairsBase::init(int argCount, char **argList) {
   boost::program_options::variables_map vm;

   if(!parseCommand(argCount, argList, vm))
      return -2;
   sessionUUID_ = (vm["session"].as<std::string>());
   database_    = (vm["database"].as<std::string>());
   uuid_        = "MICHAELCHRIS";//saga::uuid().string();
   try {
      run();
   }
   catch (saga::exception const & e) {
      std::cerr << "AllPairsBase::init : Exception caught : " << e.what() << std::endl;
      return -1;
   }   
   catch (...) {
      std::cerr << "AllPairsBase::init : Unknown exception occurred" << std::endl;
      return -1;
   }
   return 0;
}

/*********************************************************
 * The default hash function to split keys into different*
 * files after mapping.  No real reason to use this one, *
 * but it does the job.                                  *
 * ******************************************************/
int AllPairsBase::hash(std::string input,unsigned int limit) {
   int sum = 0;
   int retval;
   std::size_t length = input.length();
   for(std::size_t count = 0; count < length; count++){
      sum = sum + input[count];
   }
   retval = (sum % limit);
   return retval;
}

/*********************************************************
 * emitIntermediate is called inside the map function and*
 * handles writing the key value pairs to proper files   *
 * and advertising these files.                          *
 * ******************************************************/
void AllPairsBase::emitIntermediate(std::string key, std::string value) {
   static std::map<std::string,std::vector<std::string> > intermediate;
   int mode = saga::filesystem::ReadWrite | saga::filesystem::Create | saga::filesystem::Append;
   intermediate[key].push_back(value);
   if(intermediate.size() >= MAX_INTERMEDIATE_SIZE){
      std::map<std::string, std::vector<std::string> >::iterator it = intermediate.begin();
      while(it != intermediate.end()){
         std::string it_key = (*it).first;
         int mapFile = hash(it_key, NUM_MAPS);
         std::string filestring("/home/michael/mapFile-" + boost::lexical_cast<std::string>(mapFile));
         saga::filesystem::file f(saga::url(filestring), mode);
         it_key.append(" ");
         it_key.append((*it).second[0]);
         std::size_t size = (*it).second.size();
         for(unsigned int x = 1; x < size; x++){
            it_key.append(", ");
            it_key.append((*it).second[x]);
         }
         it++;
         it_key.append(";\n");
         f.write(saga::buffer(it_key, it_key.length()));
         f.close();
      }
      intermediate.clear();
   }
}
 
/*********************************************************
 * emit is called from inside the reduce function and    *
 * handles taking the output from reduce and writing it  *
 * to the proper file.                                   *
 * ******************************************************/
void AllPairsBase::emit(std::string key, std::string value) {
   int mode = saga::filesystem::ReadWrite | saga::filesystem::Create | saga::filesystem::Append;
   int mapFile = hash(key, NUM_MAPS);
   std::string filestring("/home/michael/mapFile-reduced-" + boost::lexical_cast<std::string>(mapFile));
   saga::filesystem::file f(saga::url(filestring), mode);
   std::string message(key);
   message += " " + value + "\n";
   f.write(saga::buffer(message, message.length()));
}

/*********************************************************
 * starts the worker and begins all neccessary setup with*
 * the database.                                        *
 * ******************************************************/
void AllPairsBase::run(void){
  registerWithDB(); //Connect and create directories in database

  mainLoop(5); //sleep interval of 5
}

/*********************************************************
 * registerWithDB connects to the advert database and    *
 * creates all necessary directories and creates         *
 * attributes describing this session.                   *
 * ******************************************************/
void AllPairsBase::registerWithDB(void){ 
   int mode = saga::advert::ReadWrite;
   std::cout << "Registering with OrchestratorDB: " << std::flush;
   //(1) connect to the orchestrator database
   std::string advertKey("advert://");
   advertKey += database_ + "//" + sessionUUID_ + "/";
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
      std::cout << "FAILED (" << e.get_error() << ")" << std::endl;
      throw;
   }
}

/*********************************************************
 * updateStatus_ updates the attributes in the database  *
 * to allow the master to know keepalive information.    *
 * ******************************************************/
void AllPairsBase::updateStatus_() {
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

/*********************************************************
 * getFrontendCommand_ retrieves the command from the    *
 * database that was posted by the master. The command   *
 * describes which action to take, this command is given *
 * after all necessary information has been posted by the*
 * master, such as input files, etc.                     *
 * ******************************************************/
std::string AllPairsBase::getFrontendCommand_() {
   std::string commandString;
   try {
     commandString = workerDir_.get_attribute("COMMAND");
     workerDir_.set_attribute("COMMAND", "");
   }
   catch(saga::exception const & e) {
     std::cout << "FAILED (" << e.get_error() << ")" << std::endl;
     throw;
   }
   // get command number & reset the attribute to "" 
   return commandString;
}

/*********************************************************
 * mainLoop loops constantly checking the advert database*
 * for commands and begins working when a proper command *
 * discovered.                                           *
 * ******************************************************/
void AllPairsBase::mainLoop(unsigned int updateInterval) {
   while(1) {
      std::string command(getFrontendCommand_());
      //(1) read command from orchestrator
      if(command == WORKER_COMMAND_MAP) {
         // Use the RunMap class to handle details of getting
         // and retrieving necessary information from the master.
         RunMap mapHandler(workerDir_, chunksDir_, intermediateDir_);
         map(mapHandler.getFile()); // Map the file given from the master
//         std::vector<std::string> output(mapHandler.getOutput());
      }
      else if(command == WORKER_COMMAND_REDUCE) {
         // Use the RunReduce class to handle details of getting
         // and retrieving necessary information from the master.
         RunReduce reduceHandler(workerDir_, reduceInputDir_);

         // Get a map of keys and a vector of the values
         std::map<std::string, std::vector<std::string> > keyValues(reduceHandler.getLines());
         std::map<std::string, std::vector<std::string> >::iterator keyValuesIT = keyValues.begin();
         // Iterate over these keys and their values and
         // reduce them by passing them to the user defined
         // reduce function
         while(keyValuesIT != keyValues.end())
         {
            reduce(keyValuesIT->first, keyValuesIT->second);
            keyValuesIT++;
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
         cleanup_();
         return;
      }
      //(2) write some statistics + ping signal 
      updateStatus_();
      //(3) sleep for a while
      sleep(updateInterval);
   }
}

void AllPairsBase::cleanup_() {

}

