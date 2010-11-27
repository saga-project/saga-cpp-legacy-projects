//  Copyright (c) 2009 Miklos Erdelyi
//  Copyright (c) 2008 Michael Miceli and Christopher Miceli
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "HandleReduces.hpp"
#include <boost/lexical_cast.hpp>
#include "protocol.hpp"
#include "output/FileOutputFormat.hpp"
#include <fstream>
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
	 	 	std::vector<std::string> reduceInput(groupFiles_());
			std::stringstream sserr;
			std::string redlist;
			log_->write("reduce input size:" + reduceInput.size(), MR_LOGLEVEL_WARNING);
			log_->write("opening file:", MR_LOGLEVEL_WARNING);
			std::ofstream myfile;
			sserr<< "/path/to/output/mr-list.txt";
			myfile.open(sserr.str().c_str());
			for(unsigned int counter = 0; counter < reduceInput.size(); counter++) {
				redlist += reduceInput[counter]  + "\n";
			}
			myfile << redlist;
			myfile.close();
			log_->write("closing file:", MR_LOGLEVEL_WARNING);
			redlist.clear();



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
       // Group all files that were mapped

    issue_command_();
  }
  return true;
}
bool TimedWaitForRead(saga::stream::stream& stream, int secs) {
  int n = 0;
  while(n < secs) {
    saga::stream::activity res = stream.wait(saga::stream::Read, 0.0);
    bool canread = res & saga::stream::Read;
    LOG_DEBUG << "res " <<  canread;
    if (res & saga::stream::Read) {
      break;
    }
    sleep(1);
    ++n;
  }
  return n < secs;
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
       // worker.write(saga::buffer(MASTER_QUESTION_STATE, 6));
         size_t written = worker.write(saga::buffer(MASTER_QUESTION_STATE, 6));
         log_->write("qu " + boost::lexical_cast<std::string>(written), MR_LOGLEVEL_INFO);
         char buff[MSG_BUFFER_SIZE];
         if (!TimedWaitForRead(worker, 10)) {
           LOG_DEBUG << "Worker didn't respond -- retrying.";
           delete service_;
           service_ = new saga::stream::server(serverURL_);
           continue;
         }
         saga::ssize_t read_bytes = worker.read(saga::buffer(buff));
         log_->write("readqu " + boost::lexical_cast<std::string>(written), MR_LOGLEVEL_INFO);
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

            saga::advert::entry adv(workerChunkDir.open(saga::url("./input-"+boost::lexical_cast<std::string>(0)), mode));
            adv.store_object<std::string>(boost::lexical_cast<std::string>(currentPartition_));

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
std::vector<std::string> HandleReduces::groupFiles_() {
  std::vector<std::string> intermediate_files;
  std::map<std::string, saga::url>::const_iterator chunk_it = committed_chunks_.begin();
  std::string output_base(FileOutputFormat::GetOutputBase(job_) + FileOutputFormat::GetOutputPath(job_));
while (chunk_it != committed_chunks_.end()) {
    try {
      // Get worker who processed this chunk.
     // log_->write("Chunk " + chunk_it->second.get_string() + " from " + chunk_it->first, MR_LOGLEVEL_INFO);
      std::string path = output_base + "replasce/map-"+ chunk_it->first;
      intermediate_files.push_back(path);
      log_->write(("Added file " + path + " to input list"), MR_LOGLEVEL_INFO);
     // log_->write((chunk_it->first), MR_LOGLEVEL_INFO);
      ++chunk_it;
    } catch (saga::exception const& e) {
        throw;
    }
  }
  return intermediate_files;
}

} // namespace master
} // namespace mapreduce
