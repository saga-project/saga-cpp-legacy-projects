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
   HandleMaps::HandleMaps(std::vector<std::string> &chunks, saga::advert::directory workerDir) {
      workerDir_ = workerDir;
      sleep(20);
      workers_ = workerDir_.list("?");
      chunks_ = chunks;
      candidateIT_ = chunks_.begin();
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
      static std::vector<saga::url>::iterator workers_IT = workers_.begin();
      bool assigned = false; //Describes status of current chunk (file)
      while(assigned == false) {
         try {
            saga::advert::directory possibleWorker(*workers_IT, mode);
            std::string state = possibleWorker.get_attribute("STATE");
            std::cerr << "state = " << state << std::endl;
            if(state == WORKER_STATE_IDLE) {
               saga::advert::directory workerChunkDir(possibleWorker.open_dir(saga::url(ADVERT_DIR_CHUNKS), mode));
               saga::advert::entry adv(workerChunkDir.open(saga::url("./chunk"), mode | saga::advert::Create));
               adv.store_string(file);
               possibleWorker.set_attribute("COMMAND", WORKER_COMMAND_MAP);
               assigned = true;
            }
            else if(state == WORKER_STATE_DONE) {
               saga::advert::directory workerChunkDir(possibleWorker.open_dir(saga::url(ADVERT_DIR_CHUNKS), mode));
               saga::advert::entry adv(workerChunkDir.open(saga::url("./chunk"), mode ));
               std::string finished_file(adv.retrieve_string());
               finished_.push_back(finished_file);
               saga::task t0 = possibleWorker.set_attribute<saga::task_base::ASync>("STATE",   WORKER_STATE_IDLE);
               saga::task t1 = possibleWorker.set_attribute<saga::task_base::ASync>("COMMAND", "");
               t0.wait();
               t1.wait();
               //Now that we have results, put them to work
               if(file != finished_file) {  //Candidate did not just finish
                  adv.store_string(file);
                  possibleWorker.set_attribute("COMMAND", WORKER_COMMAND_MAP);
                  assigned = true;
               }
               else if(chunks_.size() == finished_.size()) {  //The last file just finished all
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
      bool finished = false;
      //Loop guarantees we don't look at the same file twice as a candidate
      for(unsigned count = 0; count < chunks_.size(); count++) {
         std::string candidate = *candidateIT_;
         std::vector<std::string>::iterator finished_IT = finished_.begin();
         while(finished_IT != finished_.end()) {
            if(candidate == *finished_IT) {
               finished = true;
               break;
            }
         }
         if(finished == false) {
            return *candidateIT_;
         }
         else {
            candidateIT_++;
            if(candidateIT_ == chunks_.end()) {
               candidateIT_ = chunks_.begin();
            }
         }
      }
      throw new std::exception;
   }
} //namespace MapReduce

