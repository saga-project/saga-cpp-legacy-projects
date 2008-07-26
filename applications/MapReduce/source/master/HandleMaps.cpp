//  Copyright (c) 2008 Michael Miceli and Christopher Miceli
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "HandleMaps.hpp"
#include "../utils/defines.hpp"

/*********************************************************
 * The HandleMaps class handles all the details of       *
 * assigning map tasks to running workers.               *
 ********************************************************/
namespace MapReduce {
   HandleMaps::HandleMaps(std::vector<saga::url> &chunks, saga::advert::directory workerDir,
                          LogWriter *log) 
      : chunks_(chunks), workerDir_(workerDir), log_(log)
   {
      candidateIT_ = chunks_.begin();
      workers_ = workerDir_.list("?");
      while(workers_.size() == 0)
      {
         sleep(1);
         workers_ = workerDir_.list("?");
      }
   }

/*********************************************************
 * assignMaps is the only public function, it keeps iter-*
 * ating through chunks and assigning them to running    *
 * workers until all chunks have been finished mapping.  *
 * ******************************************************/
   bool HandleMaps::assignMaps() {
      //Until the number of finished chunks
      //equals the number of total chunks
      while(chunks_.size() != finished_.size()) {
         std::string candidate(getCandidate_()); //Find a candidate chunk
         issue_command_(candidate);  // Try to assign the chunk to someone
      }
      return true;
   }

/*********************************************************
 * issue_command_ finds and idle worker or finished      *
 * worker and tries to assign them a chunk.  If thye are *
 * finished, record their results in the finished_ vector*
 * ******************************************************/
   void HandleMaps::issue_command_(std::string file) {
      int mode = saga::advert::ReadWrite;
      static std::vector<saga::url>::const_iterator workers_IT = workers_.begin();
      bool assigned = false; //Describes status of current chunk (file)
      while(assigned == false) {
         try {
            saga::advert::directory possibleWorker(*workers_IT, mode);
            std::string state = possibleWorker.get_attribute("STATE");
            std::string message(workers_IT->get_path());
            message += (" state is " + state);
            log_->write(message, LOGLEVEL_INFO);
            if(state == WORKER_STATE_IDLE) {
               if(possibleWorker.get_attribute("COMMAND") == WORKER_COMMAND_MAP) {
                  break;
               }
               message.clear();
               message = "Issuing worker ";
               message += workers_IT->get_path();
               message = message + " to map " + file;
               log_->write(message, LOGLEVEL_INFO);

               saga::advert::directory workerChunkDir(possibleWorker.open_dir(saga::url(ADVERT_DIR_CHUNKS), mode));
               saga::advert::entry adv(workerChunkDir.open(saga::url("./chunk"), mode | saga::advert::Create));
               adv.store_string(file);
               assigned_.push_back(file);
               possibleWorker.set_attribute("COMMAND", WORKER_COMMAND_MAP);
               assigned = true;
            }
            else if(state == WORKER_STATE_DONE) {
               saga::advert::directory workerChunkDir(possibleWorker.open_dir(saga::url(ADVERT_DIR_CHUNKS), mode));
               saga::advert::entry     adv(workerChunkDir.open(saga::url("./chunk"), mode | saga::advert::Create));
               std::string finished_file(adv.retrieve_string());
               finished_.push_back(finished_file);
               saga::task t0 = possibleWorker.set_attribute<saga::task_base::Sync>("STATE",   WORKER_STATE_IDLE);
               saga::task t1 = possibleWorker.set_attribute<saga::task_base::Sync>("COMMAND", "");
               t0.wait();
               t1.wait();
               //Now that we have results, put them to work
               if(file != finished_file) {  //Candidate did not just finish
                  std::string("Issuing worker ");
                  message.clear();
                  message += workers_IT->get_path();
                  message = message + " to map " + file;
                  log_->write(message, LOGLEVEL_INFO);
                  adv.store_string(file);
                  assigned_.push_back(file);
                  possibleWorker.set_attribute("COMMAND", WORKER_COMMAND_MAP);
                  assigned = true;
               }
               else if(chunks_.size() == finished_.size()) {  //The last file just finished all
                  saga::task t0 = possibleWorker.set_attribute<saga::task_base::Sync>("STATE",   WORKER_STATE_IDLE);
                  saga::task t1 = possibleWorker.set_attribute<saga::task_base::Sync>("COMMAND", "");
                  assigned = true;
                  break;
               }
            }
         }
         catch(saga::exception const & e) {
            std::string message(e.what());
//            log->write(message, LOGLEVEL_ERROR);
         }
         workers_IT++;
         if(workers_IT == workers_.end()) {
            workers_ = workerDir_.list("?");
            workers_IT = workers_.begin();
            return;
         }
      }
   }

/*********************************************************
 * getCandidate_ tries to find a chunk that is not       *
 * finished while trying to give priority to chunks that *
 * have been least recently assigned.                    *
 * ******************************************************/
   std::string HandleMaps::getCandidate_() {
      //Check to see if current choice is in finished list
      //Loop guarantees we don't look at the same file twice as a candidate
      for(unsigned count = 0; count < chunks_.size(); count++) {
         bool finished = false;
         bool assigned = false;
         std::string candidate = candidateIT_->get_string();
         std::vector<std::string>::iterator finished_IT = finished_.begin();
         while(finished_IT != finished_.end()) {
            if(candidate == *finished_IT) {
               finished = true;
               break;
            }
            finished_IT++;
         }
         std::vector<std::string>::iterator assigned_IT = assigned_.begin();
         while(assigned_IT != assigned_.end()) {
            if(candidate == *assigned_IT) {
               assigned = true;
               break;
            }
            assigned_IT++;
         }
         if(finished == false && assigned == false) {
            return candidateIT_->get_string();
         }
         if(finished == false && assigned == true) {
            candidateIT_++;
            if(candidateIT_ == chunks_.end()) {
               candidateIT_ = chunks_.begin();
            }
         }
      }
      return candidateIT_->get_string();
   }
} //namespace MapReduce

