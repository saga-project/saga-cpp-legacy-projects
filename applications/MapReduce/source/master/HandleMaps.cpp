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
 HandleMaps::HandleMaps(std::vector<saga::url> &chunks, 
                        saga::url serverURL,
                        LogWriter *log) 
    : serverURL_(serverURL), log_(log)
 {
    std::vector<saga::url>::iterator chunksIT = chunks.begin();
    std::vector<saga::url>::iterator end      = chunks.end();
    while(chunksIT != end) {
       unassigned_.push_back(chunksIT->get_string());
       ++chunksIT;
    }
    totalChunks_ = unassigned_.size();
    try
    {
       service_ = new saga::stream::server(serverURL_);
    }
    catch(saga::exception const& e) {
       std::cerr << "saga::execption caught: " << e.what () << std::endl;
    }
 }

 HandleMaps::~HandleMaps()
 {
    service_->close();
    delete service_;
 }
/*********************************************************
 * assignMaps is the only public function, it keeps iter-*
 * ating through chunks and assigning them to running    *
 * workers until all chunks have been finished mapping.  *
 * ******************************************************/
 bool HandleMaps::assignMaps() {
    //Until the number of finished chunks
    //equals the number of total chunks
    while(totalChunks_ != finished_.size()) {
       issue_command_();
    }
    return true;
 }

/*********************************************************
 * issue_command_ finds and idle worker or finished      *
 * worker and tries to assign them a chunk.  If thye are *
 * finished, record their results in the finished_ vector*
 * ******************************************************/
 void HandleMaps::issue_command_() {
    bool assigned = false; //Describes status of current chunk (file)
    while(assigned == false) {
       try {
          saga::stream::stream worker = service_->serve();
          std::string message("Established connection to ");
          message += worker.get_url().get_string();
          log_->write(message, LOGLEVEL_INFO);

          //Ask worker for state
          worker.write(saga::buffer(MASTER_QUESTION_STATE, 6));
          char buff[255];
          saga::ssize_t read_bytes = worker.read(saga::buffer(buff));
          std::string state(buff, read_bytes);

          message.clear();
          message = "Worker: " + worker.get_url().get_string() + " has state " + state;
          log_->write(message, LOGLEVEL_INFO);

          if(state == WORKER_STATE_IDLE)
          {
             if(finished_.size() == totalChunks_)
             {
                //Prevent unneccessary work assignments
                worker.write(saga::buffer(MASTER_REQUEST_IDLE, 5));
                saga::ssize_t read_bytes = worker.read(saga::buffer(buff));
                if(std::string(buff, read_bytes) != WORKER_RESPONSE_ACKNOLEDGE)
                {
                   log_->write(std::string("Misbehaving worker!"), LOGLEVEL_WARNING);
                }
                return;
             }
             std::string file(getCandidate_());
             //Worker is idle
             message.clear();
             message = "Attempting to issue worker ";
             message += worker.get_url().get_string();
             message += " to map " + file;
             message += " ...";
             log_->write(message, LOGLEVEL_INFO);

             //ask where their advert is
             worker.write(saga::buffer(MASTER_QUESTION_ADVERT, 7));
             memset(buff, 0, 255);
             read_bytes = worker.read(saga::buffer(buff));
             saga::url advert = saga::url(std::string(buff, read_bytes));

             message.clear();
             message += worker.get_url().get_string();
             message += " <==> " + std::string(buff);
             message += " ... ";
             log_->write(message, LOGLEVEL_INFO); 

             //Tell worker about data
             worker.write(saga::buffer(WORKER_COMMAND_MAP, 3));
             memset(buff, 0, 255);
             read_bytes = worker.read(saga::buffer(buff));
             if(std::string(buff, read_bytes) == WORKER_RESPONSE_ACKNOLEDGE)
             {
                worker.write(saga::buffer(WORKER_CHUNK, 5));
                memset(buff, 0, 255);
                read_bytes = worker.read(saga::buffer(buff));
                if(std::string(buff, read_bytes) == WORKER_RESPONSE_ACKNOLEDGE)
                {
                   worker.write(saga::buffer(file, file.size()));
                   memset(buff, 0, 255);
                   read_bytes = worker.read(saga::buffer(buff));
                   if(std::string(buff, read_bytes) == WORKER_RESPONSE_ACKNOLEDGE)
                   {
                      //If not in assigned, add it
                      std::vector<std::string>::iterator end = assigned_.end();
                      bool found = false;
                      for(std::vector<std::string>::iterator assigned_IT = assigned_.begin();
                          assigned_IT != end;
                          ++assigned_IT)
                      {
                         if(file == *assigned_IT)
                         {
                            found = true;
                            break;
                         }
                      }
                      if(found == false)
                      {
                         //Not previously assigned
                         assigned_.push_back(file);
                      }
                      //Remove from unassigned if there
                      end = unassigned_.end();
                      for(std::vector<std::string>::iterator unassigned_IT = unassigned_.begin();
                          unassigned_IT != end;
                          ++unassigned_IT)
                      {
                         if(file == *unassigned_IT)
                         {
                            //already removed from unassigned
                            unassigned_.erase(unassigned_IT);
                            break;
                         }
                      }
                   }
                }
             }
             else
             {
                message = std::string("Worker did not accept chunk!");
                log_->write(message, LOGLEVEL_WARNING);
                break;
             }

             message.clear();
             message += "Success: ";
             message += advert.get_string() + " is comparing chunk ";
             message += file;
             log_->write(message, LOGLEVEL_INFO);

             assigned = true;
          }
          else if(state == WORKER_STATE_DONE_MAP)
          {
             worker.write(saga::buffer(MASTER_QUESTION_RESULT, 7));
             memset(buff, 0, 255);
             read_bytes = worker.read(saga::buffer(buff));
             std::string result(buff, read_bytes);
             worker.write(saga::buffer(MASTER_REQUEST_IDLE, 5));

             message.clear();
             message += "Worker ";
             message += worker.get_url().get_string() + " finished chunk ";
             message += result;
             log_->write(message, LOGLEVEL_INFO);

             //If in assigned, remove it
             std::vector<std::string>::iterator end = assigned_.end();
             for(std::vector<std::string>::iterator assigned_IT = assigned_.begin();
                 assigned_IT != end;
                 ++assigned_IT)
             {
                if(result == *assigned_IT)
                {
                   assigned_.erase(assigned_IT);
                   break;
                }
             }

             //Make sure not already inserted into finished list
             end = finished_.end();
             bool found = false;
             for(std::vector<std::string>::iterator finished_IT= finished_.begin();
                 finished_IT != end;
                 ++finished_IT)
             {
                if(result == *finished_IT)
                {
                   found = true;
                   break;
                }
             }
             if(found == false) {
                finished_.push_back(result);
             }
          }
       }
       catch(saga::exception const & e) {
          std::string message(e.what());
          log_->write(message, LOGLEVEL_ERROR);
       }
    }
 }

/*********************************************************
 * getCandidate_ tries to find a chunk that is not       *
 * finished while trying to give priority to chunks that *
 * have been least recently assigned.                    *
 * ******************************************************/
 std::string HandleMaps::getCandidate_() {
    if(unassigned_.size() > 0)
    {
       return unassigned_[0];
    }
    else if(assigned_.size() > 0)
    {
       return assigned_[rand() % assigned_.size()];
    }
    else if(finished_.size() > 0)
    {
       //No one should be asking!
       return finished_[rand() % finished_.size()];
    }
    else
    {
       std::cerr << "Stop asking for chunks!" << std::endl;
       APPLICATION_ABORT;
    }
 }
} // namespace MapReduce

