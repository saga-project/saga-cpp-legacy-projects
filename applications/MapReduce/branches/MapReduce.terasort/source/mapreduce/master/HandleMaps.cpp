//  Copyright (c) 2008 Michael Miceli and Christopher Miceli
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "HandleMaps.hpp"
#include "../protocol.hpp"
#include <sstream>
#include "saga_stream_utils.hpp"
/*********************************************************
 * The HandleMaps class handles all the details of       *
 * assigning map tasks to running workers.               *
 ********************************************************/
namespace mapreduce { namespace master {

HandleMaps::HandleMaps(const JobDescription& job,
                       std::vector<InputChunk*> &chunks,
                       std::map<std::string, saga::url>& committed_chunks,
                       RawInputFormat* input_format,
                       saga::url serverURL,
                       LogWriter *log)
  : job_(job), committed_chunks_(committed_chunks), input_format_(input_format),
    serverURL_(serverURL), log_(log)
{
  // Generate UUID for all chunks for easier identification.
  std::vector<InputChunk*>::iterator chunksIT = chunks.begin();
  std::vector<InputChunk*>::iterator end      = chunks.end();
  chunk_assignments_.clear();
  while(chunksIT != end) {
    std::string chunk_id = saga::uuid().string();
    chunk_assignments_[chunk_id] = *chunksIT;
    unassigned_.insert(chunk_id);
    ++chunksIT;
  }
  totalChunks_ = unassigned_.size();
  // Create stream server.
  try
  {
    service_ = new saga::stream::server(serverURL_);
  }
  catch(saga::exception const& e) {
    std::cerr << "saga::exception caught: " << e.what () << std::endl;
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
    std::stringstream message;
    message << finished_.size() << " chunks done out of " << totalChunks_;
    log_->write(message.str(),  MR_LOGLEVEL_INFO);
    message.str("");
    message << "Chunks unassigned/assigned: " << unassigned_.size() << "/" << assigned_.size();
    log_->write(message.str(), MR_LOGLEVEL_INFO);
    issue_command_();
  }
  return true;
}

/*********************************************************
 * issue_command_ finds and idle worker or finished      *
 * worker and tries to assign them a chunk.  If thye are *
 * finished, record their results in the finished_ vector*
 * ******************************************************/
#define MSG_BUFFER_SIZE 255
void HandleMaps::issue_command_() {
    bool assigned = false; //Describes status of current chunk (file)
    while(assigned == false) {
       try {
          saga::stream::stream worker = service_->serve();
          std::string message("Established connection to ");
          message += worker.get_url().get_string();
          log_->write(message, MR_LOGLEVEL_INFO);

          //Ask worker for state
          //worker.write(saga::buffer(MASTER_QUESTION_STATE, 6));
          size_t written = worker.write(saga::buffer(MASTER_QUESTION_STATE, 6));
          char buff[MSG_BUFFER_SIZE];
         // log_->write("qu " + boost::lexical_cast<std::string>(written), MR_LOGLEVEL_INFO);
          if (!TimedWaitForRead(worker, 10)) {
            LOG_DEBUG << "Worker didn't respond -- retrying.";
            delete service_;
            service_ = new saga::stream::server(serverURL_);
            continue;
          }

          saga::ssize_t read_bytes = worker.read(saga::buffer(buff));
          std::string state(buff, read_bytes);

          message.clear();
          message = "Worker: " + worker.get_url().get_string() + " has state " + state;
          log_->write(message, MR_LOGLEVEL_INFO);

          if(state == WORKER_STATE_IDLE)
          {
             if(finished_.size() == totalChunks_ || unassigned_.size() == 0)
             {
                //Prevent unneccessary work assignments
                worker.write(saga::buffer(MASTER_REQUEST_IDLE, 5));
                saga::ssize_t read_bytes = worker.read(saga::buffer(buff));
                if(std::string(buff, read_bytes) != WORKER_RESPONSE_ACKNOWLEDGE)
                {
                   log_->write(std::string("Misbehaving worker!"), MR_LOGLEVEL_WARNING);
                }
                return;
             }
             std::string chunk_id(getCandidate_(worker));
             //Worker is idle
             message.clear();
             message = "Attempting to issue worker ";
             message += worker.get_url().get_string();
             message += " to map " + chunk_id;
             message += " ...";
             log_->write(message, MR_LOGLEVEL_INFO);

             //ask where their advert is
             worker.write(saga::buffer(MASTER_QUESTION_ADVERT, 7));
             memset(buff, 0, MSG_BUFFER_SIZE);
             read_bytes = worker.read(saga::buffer(buff));
             saga::url advert = saga::url(std::string(buff, read_bytes));
             // Store URL->advert mapping.
             worker_adverts_[worker.get_url().get_string()] = advert;

             message.clear();
             message += worker.get_url().get_string();
             message += " <==> " + std::string(buff);
             message += " ... ";
             log_->write(message, MR_LOGLEVEL_INFO); 

             //Tell worker about data
             worker.write(saga::buffer(WORKER_COMMAND_MAP, 3));
             memset(buff, 0, MSG_BUFFER_SIZE);
             read_bytes = worker.read(saga::buffer(buff));
             if(std::string(buff, read_bytes) == WORKER_RESPONSE_ACKNOWLEDGE)
             {
                worker.write(saga::buffer(WORKER_CHUNK, 5));
                memset(buff, 0, MSG_BUFFER_SIZE);
                read_bytes = worker.read(saga::buffer(buff));
                if(std::string(buff, read_bytes) == WORKER_RESPONSE_ACKNOWLEDGE)
                {
                  // Get chunk pointer for ID.
                  InputChunk* chunk = chunk_assignments_[chunk_id];
                  // Give serialized JobDescription, chunk_id and chunk to worker.
                  // FIXME: shouldn't we store chunk ID with the chunk/jobdesc?
                  std::string command;
                  {
                    StringOutputStream sos(&command);
                    SerializationHandler<JobDescription>::Serialize(
                      const_cast<JobDescription*>(&job_), &sos);
                    SerializationHandler<std::string>::Serialize(&chunk_id, &sos);
                    input_format_->SerializeInputChunk(chunk, &sos);
                  }
                   worker.write(saga::buffer(command, command.size()));
                   memset(buff, 0, MSG_BUFFER_SIZE);
                   read_bytes = worker.read(saga::buffer(buff));
                   if (std::string(buff, read_bytes) == WORKER_RESPONSE_ACKNOWLEDGE)
                   {
                      // Add to assigned set.
                      assigned_.insert(chunk_id);
                      //Remove from unassigned.
                      unassigned_.erase(chunk_id);
                   }
                }
             }
             else
             {
                message = std::string("Worker did not accept chunk!");
                log_->write(message, MR_LOGLEVEL_WARNING);
                break;
             }

             message.clear();
             message += "Success: ";
             message += advert.get_string() + " is comparing chunk ";
             message += chunk_id;
             log_->write(message, MR_LOGLEVEL_INFO);

             assigned = true;
          }
          else if(state == WORKER_STATE_DONE_MAP)
          {
             worker.write(saga::buffer(MASTER_QUESTION_RESULT, 7));
             memset(buff, 0, MSG_BUFFER_SIZE);
             read_bytes = worker.read(saga::buffer(buff));
             std::string result_message(buff, read_bytes);
             worker.write(saga::buffer(MASTER_REQUEST_IDLE, 5));
             // Parse result message.
             size_t split_point = result_message.find_first_of(' ');
             std::string chunk_id = result_message.substr(0, split_point);
             std::string worker_advert = result_message.substr(split_point+1);
             message.clear();
             message += "Worker ";
             message += worker.get_url().get_string() + " finished chunk ";
             message += chunk_id;
             log_->write(message, MR_LOGLEVEL_INFO);

             // Note which worker completed this chunk.
             message.clear();
             message += "Noted " + worker_advert + " for " + chunk_id;
             committed_chunks_[chunk_id] = worker_advert;
             // If in assigned, remove it.
             assigned_.erase(chunk_id);
             // Put into finished set.
             finished_.insert(chunk_id);
          }
       }
       catch(saga::exception const & e) {
          std::string message(e.what());
          log_->write(message, MR_LOGLEVEL_ERROR);
       }
    }
}

/*********************************************************
 * getCandidate_ tries to find a chunk that is not       *
 * finished while trying to give priority to chunks that *
 * have been least recently assigned.                    *
 * ******************************************************/
std::string HandleMaps::getCandidate_(saga::stream::stream& worker) {
  if(unassigned_.size() > 0)
  {
    return *(unassigned_.begin());
  }
  else
  {
     std::cerr << "Stop asking for chunks!" << std::endl;
     APPLICATION_ABORT;
  }
}

} // namespace master
} // namespace mapreduce
