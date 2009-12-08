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
#include "../utils/network.hpp"
#include "RunComparison.hpp"
#include "RunStaging.hpp"
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
      AllPairsBase(int argCount, char **argList) {
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
         location_    = (vm["hostname"].as<std::string>());
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
      std::string location_;
      int lastFinishedChunk_;
      saga::url   serverURL_;
   
      time_t startupTime_;
      SystemInfo systemInfo_;
   
      saga::advert::directory workerDir_;
      saga::advert::directory resultDir_;
      saga::advert::directory sessionBaseDir_;
      std::vector<double>     stageResult_;
      AllPairs::LogWriter*    logWriter_;
      RunComparison*          runComparison_;
      Derived& derived() {
         return static_cast<Derived&>(*this);
      }
      /*********************************************************
       * updateStatus_ updates the attributes in the database  *
       * to allow the master to know keepalive information.    *
       * ******************************************************/
      void updateStatus_(void) {
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
         std::cout << "Registering with OrchestratorDB" << std::flush;
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
            std::cerr << "SERVER_URL about to get" << std::endl;
            serverURL_ = saga::url(server_name.retrieve_string());
            std::cerr << "SERVER_URL = " << serverURL_.get_string() << std::endl;
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
         std::cout << " I get here in mainLoop()" << std::endl;
         int mode = saga::advert::ReadWrite;
         while(1) {
            std::string command(getFrontendCommand_());
            // read command from orchestrator
            if(command == WORKER_COMMAND_COMPARE) {
               std::cerr << "GOT COMMAND COMPARE!" << std::endl;
               state_ = WORKER_STATE_COMPARING;
               std::string resultString;
               //double val;
               while(runComparison_->hasAssignment()) {
                  assignment asn(runComparison_->getAssignment());
                  std::cerr << "BUILDING ASSIGNMENT AND COMMAND" << std::endl;
                  double val = compare(asn.first, asn.second);
                  //Here is where the headache beings
                  //run perl script with runs a worker to compare
                  /*std::string command("/work/mmicel2/saga/saga-projects/applications/AllPairs/samples/runner.pl");
                  command += " -u " + saga::uuid().string();
                  saga::url firstURL(asn.first);
                  saga::url secondURL(asn.second);
                  if(firstURL.get_scheme() == "gridftp")
                  {
                     if(location_ != firstURL.get_host())
                     {
                        //requires staging
                        firstURL.set_scheme("gsiftp");
                        command += " -y " + firstURL.get_string();
                        firstURL.set_scheme("file");
                        firstURL.set_host("localhost");
                        command += ' ' + firstURL.get_string();
                     }
                     else
                     {
                        firstURL.set_scheme("file");
                        firstURL.set_host("localhost");
                        command += " -n " + firstURL.get_string();
                     }
                  }
                  else if(firstURL.get_scheme() != "file")
                  {
                     //requires staging
                     command += " -y " + asn.first;
                     firstURL.set_scheme("file");
                     firstURL.set_host("localhost");
                     command += ' ' + firstURL.get_string();
                  }
                  else
                  {
                     //requires no staging
                     command += " -n " + asn.first;
                  }
                  if(secondURL.get_scheme() == "gridftp")
                  {
                     if(location_ != secondURL.get_host())
                     {
                        //requires staging
                        secondURL.set_scheme("gsiftp");
                        command += " -y " + secondURL.get_string();
                        secondURL.set_scheme("file");
                        secondURL.set_host("localhost");
                        command += ' ' + secondURL.get_string();
                     }
                     else
                     {
                        secondURL.set_scheme("file");
                        secondURL.set_host("localhost");
                        command += " -n " + secondURL.get_string();
                     }
                  }
                  else if(secondURL.get_scheme() != "file")
                  {
                     //requires staging
                     command += " -y " + asn.second;
                     secondURL.set_scheme("file");
                     secondURL.set_host("localhost");
                     command += ' ' + secondURL.get_string();
                  }
                  else
                  {
                     //requires no staging
                     command += " -n " + asn.second;
                  }*/
                  saga::url result(std::string("result-") + boost::lexical_cast<std::string>(runComparison_->getChunkID()));
                  saga::advert::entry fin_adv(resultDir_.open(result, mode | saga::advert::Create));
                  command += " -a " + fin_adv.get_url().get_string();
                  /*std::cerr << "ABOUT TO RUN PERL COMMAND: " << command << std::endl;
                  FILE *job = popen(command.c_str(), "r");
                  std::cerr << "WAITING FOR JOB TO FINISH NOW!" << std::endl;
                  pclose(job);*/
                  std::cout << "first: " << asn.first << std::endl;
                  resultString += "(" + asn.first;
                  std::cout << "second: " << asn.second << std::endl;
                  resultString += + ", " + asn.second + "): ";
                  std::cout << "value: " << boost::lexical_cast<std::string>(&val) << std::endl;
                  resultString += boost::lexical_cast<std::string>(&val) + '\n';
               }
               lastFinishedChunk_ = runComparison_->getChunkID();
               std::cout << "chunkId: " << runComparison_->getChunkID();
               delete runComparison_;
               //fin_adv.store_string(boost::lexical_cast<std::string>(resultString));
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
         std::string read;
         try {
            saga::stream::stream server_(serverURL_);
            server_.connect();
            read = network::read(server_);
            if(network::test(read, MASTER_QUESTION_STATE))
            {
               std::string state(state_);
               server_.write(saga::buffer(state, state.size()));
               read = network::read(server_);
               if(network::test(read, MASTER_QUESTION_LOCATION))
               {
                  server_.write(saga::buffer(location_, location_.size()));
                  read = network::read(server_);
                  if(network::test(read, MASTER_QUESTION_ADVERT))
                  {
                     std::string advert(workerDir_.get_url().get_string());
                     server_.write(saga::buffer(advert, advert.size()));
                     read = network::read(server_);
                     if(network::test(read, WORKER_COMMAND_COMPARE))
                     {
                        server_.write(saga::buffer(WORKER_RESPONSE_ACKNOLEDGE, 10));
                        runComparison_ = new RunComparison(server_, logWriter_);
                        return WORKER_COMMAND_COMPARE;
                     }
                     else if(network::test(read, WORKER_COMMAND_QUIT))
                     {
                        return WORKER_COMMAND_QUIT;
                     }
                  }
                  else if(network::test(read, WORKER_COMMAND_STAGE)) 
                  {
                     server_.write(saga::buffer(WORKER_RESPONSE_ACKNOLEDGE, 10));
                     read = network::read(server_);
                     if(network::test(read, START_CHUNK)) {
                        std::vector<std::string> hosts;
                        //Get lists of hosts to ping
                        server_.write(saga::buffer(WORKER_RESPONSE_ACKNOLEDGE, 10));
                        read = network::read(server_);
                        while(network::test(read, END_CHUNK) == false) {
                           server_.write(saga::buffer(WORKER_RESPONSE_ACKNOLEDGE, 10));
                           //push_back hosts read
                           hosts.push_back(read);
                           //Call handleStaging
                           read = network::read(server_);
                        }
                        server_.write(saga::buffer(WORKER_RESPONSE_ACKNOLEDGE, 10));
                        RunStaging runStaging(hosts, location_, logWriter_);
                        stageResult_ = runStaging.getResults();
                        state_ = WORKER_STATE_DONE_STAGING;
                        return getFrontendCommand_();
                     }
                  }
                  else if(network::test(read, MASTER_REQUEST_IDLE))
                  {
                     state_ = WORKER_STATE_IDLE;
                     server_.write(saga::buffer(WORKER_RESPONSE_ACKNOLEDGE, 10));
                     sleep(5);
                     return getFrontendCommand_();
                  }
                  else if(network::test(read, START_CHUNK)) {
                     //our state is WORKER_STATE_DONE_STAGING
                     server_.write(saga::buffer(WORKER_RESPONSE_ACKNOLEDGE, 10));
                     for(std::vector<double>::iterator it = stageResult_.begin();
                         it != stageResult_.end(); ++it)
                     {
                        read = network::read(server_);
                        if(network::test(read, MASTER_QUESTION_RESULT)) {
                           std::string result(boost::lexical_cast<std::string>(*it));
                           server_.write(saga::buffer(result, result.size()));
                        }
                     }
                     network::expect(END_CHUNK, network::read(server_));
                     server_.write(saga::buffer(WORKER_RESPONSE_ACKNOLEDGE, 10));
                     network::expect(MASTER_REQUEST_IDLE, network::read(server_));
                     server_.write(saga::buffer(WORKER_RESPONSE_ACKNOLEDGE, 10));
                     state_ = WORKER_STATE_IDLE;
                     sleep(5);
                     return getFrontendCommand_();
                  }
               }
               else if(network::test(read, MASTER_QUESTION_RESULT))
               {
                  std::string lastString = boost::lexical_cast<std::string>(lastFinishedChunk_);
                  server_.write(saga::buffer(lastString, lastString.size()));
                  read = network::read(server_);
                  if(network::test(read, MASTER_REQUEST_IDLE))
                  {
                     std::cerr << "SETTING STATE TO IDLE AFTER DONE!" << std::endl;
                     state_ = WORKER_STATE_IDLE;
                  }
                  return getFrontendCommand_();
               }
               if(network::test(read, MASTER_REQUEST_IDLE))
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
            else if(network::test(read, WORKER_COMMAND_QUIT))
            {
               server_.write(saga::buffer(WORKER_RESPONSE_ACKNOLEDGE, 10));
               return WORKER_COMMAND_QUIT;
            }
         }
         catch(network::networkException const &e) {
            std::cout << "Error: " << e.what() << std::endl;
            APPLICATION_ABORT;
         }
         catch(saga::exception const & e) {
            std::cout << "Couldn't connect, try again" << std::endl;
            sleep(1);
            if(depth > 20)
            {
               return WORKER_COMMAND_QUIT;
            }
            return getFrontendCommand_();
         }
         // get command number & reset the attribute to "" 
         return read;
      }
   };
}

#endif // AP_ALLPAIRSBASE_HPP

