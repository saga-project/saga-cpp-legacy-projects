//  Copyright (c) 2009 Miklos Erdelyi
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "WorkerThread.hpp"
#include "../../utils/saga_stream_utils.hpp"

namespace mapreduce { namespace worker {

void WorkerThread::mainLoop() {
  while(1) {
    std::string command(getFrontendCommand_());
    //(1) read command from orchestrator
    LOG_DEBUG << "Commmand: " << command;
    if(command == WORKER_COMMAND_MAP) {
       try {
          state_ = WORKER_STATE_MAPPING;
          // Map the chunk given from the master.
          MapTaskRunner runner(job_, chunk_id_, raw_chunk_);
          runner.Execute();
          LOG_DEBUG << "Storing places of intermediate data in advert";
          try {
            // Store intermediate output paths for this chunk in the advert.
            int mode = saga::advert::Create | saga::advert::ReadWrite;
            int num_partitions = job_->get_num_reduce_tasks();
            for (int partition = 0; partition < num_partitions; ++partition) {
              // Name of this particular partition for this particular chunk.
              std::string partition_name(
                FileOutputFormat::GetOutputPartitionName(job_, "map",
                chunk_id_, partition));
              saga::advert::entry adv = intermediateDir_.open(
                saga::url(partition_name), mode);
              // Construct output path for this particular partition.
              std::string full_path = FileOutputFormat::GetOutputPath(*job_);
              full_path.append(partition_name);
              LOG_DEBUG << "Intermediate data: " << full_path;
              adv.store_object<std::string>(full_path);
            }
            state_ = WORKER_STATE_DONE_MAP;
          }
          catch(saga::exception const& e) {
             throw;
          }
       }
       catch(saga::exception const& e) {
          LOG_ERROR << "FAILED (" << e.get_message() << ")";
          state_ = WORKER_STATE_FAIL;
          workerDir_.set_attribute("STATE", WORKER_STATE_FAIL);
       }
    }
    else if(command == WORKER_COMMAND_REDUCE) {
       try {
          state_ = WORKER_STATE_REDUCING;
          // Reduce the input files given by the master.
          ReduceTaskRunner runner(task_.get(), reduce_partition_,
            reduceInputDir_);
          runner.Execute();
          state_ = WORKER_STATE_DONE_REDUCE;
       }
       catch(saga::exception const& e) {
          LOG_ERROR << "FAILED (" << e.get_message() << ")";
          state_ = WORKER_STATE_FAIL;
       }
    }
    else if(command == WORKER_COMMAND_DISCARD) {
       cleanup_();
    }
    else if(command == WORKER_COMMAND_QUIT)
    {
       cleanup_();
       return;
    }
    LOG_DEBUG << "Updating status with state = " << state_;
    updateStatus_();
  }
}


std::string WorkerThread::getFrontendCommand_(void) {
  static int depth = 0;
  char buff[MSG_BUFFER_SIZE];
   try {
      saga::stream::stream server_(serverURL_);
      server_.connect();
      // Wait for master's question.
      if (!SagaStreamUtils::TimedWaitForRead(
            server_, WORKER_READ_TIMEOUT)) {
        return "";
      }
      // Read master's question.
      saga::ssize_t read_bytes = server_.read(saga::buffer(buff));
      std::string question(buff, read_bytes);
      LOG_DEBUG << "QUESTION = " << question;
      if(question == MASTER_QUESTION_STATE) {
         server_.write(saga::buffer(state_, state_.size()));
         if(state_ == WORKER_STATE_IDLE) {
            memset(buff, 0, MSG_BUFFER_SIZE);
            read_bytes = server_.read(saga::buffer(buff));
            question = std::string(buff, read_bytes);
            if(question == MASTER_REQUEST_IDLE) {
               server_.write(saga::buffer(WORKER_RESPONSE_ACKNOWLEDGE, 11));
               state_ = WORKER_STATE_IDLE;
               sleep(5);
               //server_.close();
               return std::string("");
            }
            else if(question == MASTER_QUESTION_ADVERT) {
               std::string advert(workerDir_.get_url().get_string());
               server_.write(saga::buffer(advert, advert.size()));
               memset(buff, 0, MSG_BUFFER_SIZE);
               read_bytes = server_.read(saga::buffer(buff));
               question = std::string(buff, read_bytes);
               if(question == WORKER_COMMAND_MAP) {
                  server_.write(saga::buffer(WORKER_RESPONSE_ACKNOWLEDGE, 11));
                  memset(buff, 0, MSG_BUFFER_SIZE);
                  read_bytes = server_.read(saga::buffer(buff));
                  question = std::string(buff, read_bytes);
                  if(question == WORKER_CHUNK) {
                     server_.write(saga::buffer(WORKER_RESPONSE_ACKNOWLEDGE, 11));
                     memset(buff, 0, MSG_BUFFER_SIZE);
                     // FIXME: could use different, larger buffer for this one.
                     read_bytes = server_.read(saga::buffer(buff));
                     // Create ArrayInputStream for deserialization.
                     boost::scoped_ptr<ArrayInputStream> message_stream(
                       new ArrayInputStream(&buff, read_bytes));
                     // Deserialize JobDescription.
                     job_ = new JobDescription();
                     SerializationHandler<JobDescription>::Deserialize(
                       message_stream.get(), job_);
                     // Deserialize chunk id.
                     SerializationHandler<std::string>::Deserialize(
                       message_stream.get(), &chunk_id_);
                     LOG_DEBUG << "just set chunk to " << chunk_id_;
                     server_.write(saga::buffer(WORKER_RESPONSE_ACKNOWLEDGE, 11));
                     //server_.close();
                     LOG_DEBUG << "Returning command map";
                     // Obtain raw chunk to be deserialized by MapRunner.
                     int consumed_bytes = message_stream->ByteCount();
                     raw_chunk_ = new std::string(reinterpret_cast<char*>(
                       &buff[consumed_bytes]), read_bytes - consumed_bytes);
                     return WORKER_COMMAND_MAP;
                  } else {
                    LOG_DEBUG << "Unknown question?";
                  }
               }
               else if(question == WORKER_COMMAND_REDUCE) {
                  server_.write(saga::buffer(WORKER_RESPONSE_ACKNOWLEDGE, 11));
                  // Read partition number to reduce.
                  memset(buff, 0, MSG_BUFFER_SIZE);
                  read_bytes = server_.read(saga::buffer(buff));
                  reduce_partition_ = boost::lexical_cast<int>(std::string(buff, read_bytes));
                  LOG_DEBUG << "Will reduce partition " << reduce_partition_;
                  server_.write(saga::buffer(WORKER_RESPONSE_ACKNOWLEDGE, 11));
                  // Read JobDescription.
                  memset(buff, 0, MSG_BUFFER_SIZE);
                  read_bytes = server_.read(saga::buffer(buff));
                  // Create ArrayInputStream for deserialization.
                  boost::scoped_ptr<ArrayInputStream> message_stream(
                    new ArrayInputStream(&buff, read_bytes));
                  // Deserialize JobDescription.
                  boost::scoped_ptr<JobDescription> job(new JobDescription());
                  SerializationHandler<JobDescription>::Deserialize(
                    message_stream.get(), job.get());
                  task_.reset(new TaskDescription(job.get()));
                  task_->set_id(reduce_partition_);
                  server_.write(saga::buffer(WORKER_RESPONSE_ACKNOWLEDGE, 11));
                  //server_.close();
                  LOG_DEBUG << "Returning command reduce";
                  return WORKER_COMMAND_REDUCE;
               }
            }
         }
         else if(state_ == WORKER_STATE_DONE_MAP) {
            memset(buff, 0, MSG_BUFFER_SIZE);
            read_bytes = server_.read(saga::buffer(buff));
            question = std::string(buff, read_bytes);
            if(question == MASTER_QUESTION_RESULT) {
               // Respond with the finished chunk's ID and our advert directory.
               std::string result_message(chunk_id_);
               result_message.append(1, ' ');
               result_message.append(workerDir_.get_url().get_string());
               server_.write(saga::buffer(result_message, result_message.size()));
               memset(buff, 0, MSG_BUFFER_SIZE);
               read_bytes = server_.read(saga::buffer(buff));
               question = std::string(buff, read_bytes);
               if(question == MASTER_REQUEST_IDLE) {
                  state_ = WORKER_STATE_IDLE;
                  //server_.close();
                  return std::string("");
               }
            }
            else if(question == MASTER_REQUEST_IDLE) {
               server_.write(saga::buffer(WORKER_RESPONSE_ACKNOWLEDGE, 11));
               state_ = WORKER_STATE_IDLE;
               return std::string("");
            }
         }
         else if(state_ == WORKER_STATE_DONE_REDUCE) {
            memset(buff, 0, MSG_BUFFER_SIZE);
            read_bytes = server_.read(saga::buffer(buff));
            question = std::string(buff, read_bytes);
            if(question == MASTER_QUESTION_RESULT) {
               std::string last(boost::lexical_cast<std::string>(reduce_partition_));
               server_.write(saga::buffer(last, last.size()));
               memset(buff, 0, MSG_BUFFER_SIZE);
               read_bytes = server_.read(saga::buffer(buff));
               question = std::string(buff, read_bytes);
               if(question == MASTER_REQUEST_IDLE) {
                  //server_.close();
                  state_ = WORKER_STATE_IDLE;
                  return std::string("");
               }
            }
         }
      }
      if(question == WORKER_COMMAND_QUIT) {
         LOG_INFO << "GOT COMMAND TO QUIT, YIP YIP!";
         server_.write(saga::buffer(WORKER_RESPONSE_ACKNOWLEDGE, 11));
         return WORKER_COMMAND_QUIT;
      }
   }
   catch(saga::exception const & e) {
      //Hope it was couldn't connect to stream server, then just wait and try again...
      sleep(1);
      if(depth < 20) {
         depth++;
         return getFrontendCommand_();
      }
      else throw;
   }
   return std::string("");
   // get command number & reset the attribute to "" 
}

}   // namespace worker
}   // namespace mapreduce
