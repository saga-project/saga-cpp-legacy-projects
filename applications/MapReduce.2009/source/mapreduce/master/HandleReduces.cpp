//  Copyright (c) 2009 Miklos Erdelyi
//  Copyright (c) 2008 Michael Miceli and Christopher Miceli
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "HandleReduces.hpp"
#include <boost/lexical_cast.hpp>
#include "protocol.hpp"
#include "output/FileOutputFormat.hpp"

/*********************************************************
 * HandleReduces tries to group together the proper files*
 * and assign them to a worker to reduce                 *
 * ******************************************************/
namespace mapreduce { namespace master {

// fileCount is the total number of files possibly outputted by
// the map function
HandleReduces::HandleReduces(const JobDescription& job,
                             std::map<std::string, saga::url>& committed_chunks,
                             saga::advert::directory workerDir, 
                             saga::url serverURL,
                             LogWriter *log)
    : job_(job), committed_chunks_(committed_chunks), workerDir_(workerDir),
      serverURL_(serverURL), log_(log)
 {
    fileCount_ = committed_chunks_.size();
    try
    {
       service_ = new saga::stream::server(serverURL_);
    }
    catch(saga::exception const& e) {
       std::cerr << "saga::exception caught: " << e.what () << std::endl;
    }
    currentPartition_ = 0;
    numPartitions_ = job.get_num_reduce_tasks();
    // Get worker advert list.
    workers_ = workerDir_.list("*");
    while(workers_.size() == 0) {
       sleep(1);
       workers_ = workerDir_.list("*");
    }
 }

HandleReduces::~HandleReduces() {
  service_->close();
  delete service_;
}
/*********************************************************
 * assignReduces is the only public function that tries  *
 * to assign reduce files to idle workers                *
 * ******************************************************/
bool HandleReduces::assignReduces() {
  while(finished_.size() < static_cast<unsigned>(numPartitions_)) {
    log_->write("Finished " + boost::lexical_cast<std::string>(finished_.size())
        + " out of " + boost::lexical_cast<std::string>(numPartitions_)
        + " partitions",
        MR_LOGLEVEL_INFO);
    issue_command_();
  }
  return true;
}
/*********************************************************
 * issue_command takes the grouped files and tries to    *
 * assign them to an idel worker.  If a worker is done,  *
 * the results are recorded                              *
 * ******************************************************/
#define MSG_BUFFER_SIZE 2048
void HandleReduces::issue_command_() {
   bool assigned = false;
   while(assigned == false &&
         finished_.size() < static_cast<unsigned>(numPartitions_)) {
      try {
         int mode = saga::advert::Create | saga::advert::ReadWrite;
         saga::stream::stream worker = service_->serve();
         std::string message("Established connection to ");
         message += worker.get_url().get_string();
         log_->write(message, MR_LOGLEVEL_INFO);

         //Ask worker for state
         worker.write(saga::buffer(MASTER_QUESTION_STATE, 6));
         char buff[MSG_BUFFER_SIZE];
         saga::ssize_t read_bytes = worker.read(saga::buffer(buff));
         std::string state(buff, read_bytes);

         if(state == WORKER_STATE_IDLE) {
            if (currentPartition_ == numPartitions_) {
              //worker.write(saga::buffer(WORKER_COMMAND_QUIT, 4)); 
              worker.write(saga::buffer(MASTER_REQUEST_IDLE, 5));
              memset(buff, 0, MSG_BUFFER_SIZE);
              read_bytes = worker.read(saga::buffer(buff));
              std::string result(buff, read_bytes);
              if(result != WORKER_RESPONSE_ACKNOWLEDGE) {
                 //error here
              }
              sleep(1);
              log_->write("Requested worker to idle", MR_LOGLEVEL_INFO);
              break;
            }
            // Group all files that were mapped to this partition.
            std::vector<std::string> reduceInput(groupFiles_(currentPartition_));

            message.clear();
            std::string message("Issuing worker ");
            message += worker.get_url().get_string();
            message = message + " to reduce partition number ";
            message += boost::lexical_cast<std::string>(currentPartition_);
            log_->write(message, MR_LOGLEVEL_INFO);

            //ask where their advert is
            worker.write(saga::buffer(MASTER_QUESTION_ADVERT, 7));
            memset(buff, 0, MSG_BUFFER_SIZE);
            read_bytes = worker.read(saga::buffer(buff));
            saga::url advert = saga::url(std::string(buff, read_bytes));

            message.clear();
            message += worker.get_url().get_string();
            message += " <==> " + std::string(buff);
            message += " ... ";
            log_->write(message, MR_LOGLEVEL_INFO);

            worker.write(saga::buffer(WORKER_COMMAND_REDUCE, 6));
            memset(buff, 0, MSG_BUFFER_SIZE);
            read_bytes = worker.read(saga::buffer(buff));
            if(std::string(buff, read_bytes) != WORKER_RESPONSE_ACKNOWLEDGE) {
                message = std::string("Worker did not accept chore!");
                log_->write(message, MR_LOGLEVEL_WARNING);
                break;
            }
            std::string count(boost::lexical_cast<std::string>(currentPartition_));
            worker.write(saga::buffer(count, count.size()));
            memset(buff, 0, MSG_BUFFER_SIZE);
            read_bytes = worker.read(saga::buffer(buff));
            if(std::string(buff, read_bytes) != WORKER_RESPONSE_ACKNOWLEDGE) {
                message = std::string("Worker did not accept chunk!");
                log_->write(message, MR_LOGLEVEL_WARNING);
                break;
            }
            // Put reduce input files into worker's advert.
            saga::advert::directory workerAdvert(advert, mode);
            saga::advert::directory workerChunkDir(workerAdvert.open_dir(saga::url(ADVERT_DIR_REDUCE_INPUT), mode));
            for(unsigned int counter = 0; counter < reduceInput.size(); counter++) {
              saga::advert::entry adv(workerChunkDir.open(saga::url("./input-"+boost::lexical_cast<std::string>(counter)), mode));
              adv.store_object<std::string>(reduceInput[counter]);
            }
            // Serialize JobDescription.
            std::string command;
            StringOutputStream output_stream(&command);
            SerializationHandler<JobDescription>::Serialize(
              const_cast<JobDescription*>(&job_), &output_stream);
            worker.write(saga::buffer(command, command.size()));
            memset(buff, 0, MSG_BUFFER_SIZE);
            read_bytes = worker.read(saga::buffer(buff));
            if (std::string(buff, read_bytes) != WORKER_RESPONSE_ACKNOWLEDGE) {
              log_->write("Worker did not accept job description!",
                MR_LOGLEVEL_WARNING);
              break;
            }
            assigned = true;
            if(++currentPartition_ == numPartitions_) {
               // FIXME(miklos): should allow reduces to be re-issued.
               // Currently the problem is that the master cannot stop workers
               // which are in the process of reducing.
               //currentPartition_ = 0;   // Allows reduces to be re-issued.
            }
            message.clear();
            message += "Success!";
            log_->write(message, MR_LOGLEVEL_INFO);
         }
         else if(state == WORKER_STATE_DONE_REDUCE) {
            worker.write(saga::buffer(MASTER_QUESTION_RESULT, 7));
            memset(buff, 0, MSG_BUFFER_SIZE);
            read_bytes = worker.read(saga::buffer(buff));
            std::string result(buff, read_bytes);
            worker.write(saga::buffer(MASTER_REQUEST_IDLE, 5));

            message.clear();
            message += "Worker ";
            message += " finished reducing with output ";
            message += result;
            log_->write(message, MR_LOGLEVEL_INFO);

            // Note partition number as being finished.
            int finished_partition = boost::lexical_cast<int>(result);
            if (finished_.find(finished_partition) == finished_.end()) {
              finished_.insert(finished_partition);
            }
            break;
         }
         else if(state == WORKER_STATE_DONE_MAP) {
            worker.write(saga::buffer(MASTER_REQUEST_IDLE, 5));
            memset(buff, 0, MSG_BUFFER_SIZE);
            read_bytes = worker.read(saga::buffer(buff));
            std::string result(buff, read_bytes);
            if(result != WORKER_RESPONSE_ACKNOWLEDGE) {
               //error here
            }
         }
      }
      catch(saga::exception const & e) {
         std::cerr << "error in reduce" << std::endl;
         throw;
      }
   }
 }
/*********************************************************
 * groupFiles_ takes all files who correspond to the same*
 * hash function output from all workers and groups them *
 * in a vector that will eventually be passed to a worker*
 * to reduce the files into one output                   *
 * ******************************************************/
std::vector<std::string> HandleReduces::groupFiles_(int partition) {
  std::vector<std::string> intermediate_files;
  int mode = saga::advert::ReadWrite;
  std::map<std::string, saga::url>::const_iterator chunk_it =
    committed_chunks_.begin();
  while (chunk_it != committed_chunks_.end()) {
    try {
      // Get worker who processed this chunk.
      log_->write("Chunk " + chunk_it->second.get_string() + " from " +
        chunk_it->first, MR_LOGLEVEL_INFO);
      saga::advert::directory worker(chunk_it->second);
      // Problem: we don't know the UID of the worker since it does not communicate it to us.
      saga::advert::directory data(worker.open_dir(
        saga::url(ADVERT_DIR_INTERMEDIATE), mode));
      // Get output for this partition.
      saga::url fileurl(("./" + FileOutputFormat::GetOutputPartitionName(&job_,
        "map", chunk_it->first, partition)));
      if (data.exists(fileurl)) {
        saga::advert::entry adv(data.open(fileurl, mode));
        std::string path = adv.retrieve_object<std::string>();
        intermediate_files.push_back(path);
        log_->write(("Added file " + path + " to input list"), MR_LOGLEVEL_INFO);
      } else {
        log_->write(("Skipped chunk " + chunk_it->first + " missing partition "
          + boost::lexical_cast<std::string>(partition)), MR_LOGLEVEL_ERROR);
      }
      ++chunk_it;
    } catch (saga::exception const& e) {
        throw;
    }
  }
  return intermediate_files;
}

} // namespace master
} // namespace mapreduce
