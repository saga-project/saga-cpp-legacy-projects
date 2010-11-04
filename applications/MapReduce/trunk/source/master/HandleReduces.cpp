#include "HandleReduces.hpp"
#include <boost/lexical_cast.hpp>
#include "../utils/defines.hpp"

/*********************************************************
 * HandleReduces tries to group together the proper files*
 * and assign them to a worker to reduce                 *
 * ******************************************************/
namespace MapReduce
{
 // fileCount is the total number of files possibly outputted by
 // the map function (NUM_MAPS)
 HandleReduces::HandleReduces(int fileCount,
                              saga::advert::directory workerDir, 
                              saga::url serverURL,
                              LogWriter *log)
    : fileCount_(fileCount), workerDir_(workerDir), 
      serverURL_(serverURL), log_(log)
 {
    try
    {
       service_ = new saga::stream::server(serverURL_);
    }
    catch(saga::exception const& e) {
       std::cerr << "saga::execption caught: " << e.what () << std::endl;
    }
    currentCount = 0;
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
    while(finished_.size() != (unsigned)fileCount_) {
       issue_command_();
    }
    return true;
 }
/*********************************************************
 * issue_command takes the grouped files and tries to    *
 * assign them to an idel worker.  If a worker is done,  *
 * the results are recorded                              *
 * ******************************************************/
 void HandleReduces::issue_command_()
 {
   bool assigned = false;
   while(assigned == false) {
      try {
         int mode = saga::advert::Create | saga::advert::ReadWrite;
         saga::stream::stream worker = service_->serve();
         std::string message("Established connection to ");
         message += worker.get_url().get_string();
         log_->write(message, LOGLEVEL_INFO);

         //Ask worker for state
         worker.write(saga::buffer(MASTER_QUESTION_STATE, 6));
         char buff[255];
         saga::ssize_t read_bytes = worker.read(saga::buffer(buff));
         std::string state(buff, read_bytes);

         if(state == WORKER_STATE_IDLE) {
            // group all files that were mapped to this counter
            std::vector<std::string> reduceInput(groupFiles_(currentCount));

            message.clear();
            std::string message("Issuing worker ");
            message += worker.get_url().get_string();
            message = message + " to reduce hash number ";
            message += boost::lexical_cast<std::string>(currentCount);
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

            worker.write(saga::buffer(WORKER_COMMAND_REDUCE, 6));
            memset(buff, 0, 255);
            read_bytes = worker.read(saga::buffer(buff));
            if(std::string(buff, read_bytes) != WORKER_RESPONSE_ACKNOLEDGE) {
                message = std::string("Worker did not accept chore!");
                log_->write(message, LOGLEVEL_WARNING);
                break;
            }
            std::string count(boost::lexical_cast<std::string>(currentCount));
            worker.write(saga::buffer(count, count.size()));
            memset(buff, 0, 255);
            read_bytes = worker.read(saga::buffer(buff));
            if(std::string(buff, read_bytes) != WORKER_RESPONSE_ACKNOLEDGE) {
                message = std::string("Worker did not accept chunk!");
                log_->write(message, LOGLEVEL_WARNING);
                break;
            }

            saga::advert::directory workerAdvert(advert, mode);
            saga::advert::directory workerChunkDir(workerAdvert.open_dir(saga::url(ADVERT_DIR_REDUCE_INPUT), mode));
            for(unsigned int counter = 0; counter < reduceInput.size(); counter++) {
              saga::advert::entry adv(workerChunkDir.open(saga::url("./input-"+boost::lexical_cast<std::string>(counter)), mode));
               adv.store_string(reduceInput[counter]);
            }
            assigned = true;
            if(currentCount == fileCount_) {
               currentCount = 0; //Allows reduces to be re-issued
            }
            else {
               currentCount++;
            }
            message.clear();
            message += "Success!";
            log_->write(message, LOGLEVEL_INFO);
         }
         else if(state == WORKER_STATE_DONE_REDUCE) {
            worker.write(saga::buffer(MASTER_QUESTION_RESULT, 7));
            memset(buff, 0, 255);
            read_bytes = worker.read(saga::buffer(buff));
            std::string result(buff, read_bytes);
            worker.write(saga::buffer(MASTER_REQUEST_IDLE, 5));

            message.clear();
            message += "Worker ";
            message += " finished reducing with output ";
            message += result;
            log_->write(message, LOGLEVEL_INFO);

            //If not finished, already, push back
            std::vector<std::string>::iterator finished_IT = finished_.begin();
            std::vector<std::string>::iterator end         = finished_.end();
            bool found = false;
            while(finished_IT != end) {
               if(*finished_IT == result) {
                  found = true;
                  break;
               }
               ++finished_IT;
            }
            if(found == false) {
               finished_.push_back(result);
            }
         }
         else if(state == WORKER_STATE_DONE_MAP) {
            worker.write(saga::buffer(MASTER_REQUEST_IDLE, 5));
            memset(buff, 0, 255);
            read_bytes = worker.read(saga::buffer(buff));
            std::string result(buff, read_bytes);
            if(result != WORKER_RESPONSE_ACKNOLEDGE) {
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
 std::vector<std::string> HandleReduces::groupFiles_(int counter)
 {
    std::vector<std::string> intermediateFiles;
    std::vector<saga::url>::iterator workers_IT = workers_.begin();
    int mode = saga::advert::ReadWrite;
    while(workers_IT != workers_.end()) {
       try {
          saga::advert::directory worker(workerDir_.open_dir(*workers_IT));
          saga::advert::directory data(worker.open_dir(saga::url(ADVERT_DIR_INTERMEDIATE), mode));
          if(data.exists(saga::url("./mapFile-" + boost::lexical_cast<std::string>(counter)))) {
             saga::advert::entry adv(data.open(saga::url("./mapFile-" + boost::lexical_cast<std::string>(counter)), mode));
             std::string path = adv.retrieve_string();
             intermediateFiles.push_back(adv.retrieve_string());
             std::string message("Added file " + path + " to input list");
             log_->write(message, LOGLEVEL_INFO);
             message.clear();
          }
          workers_IT++;
       }
       catch(saga::exception const & e) {
          throw;
       }
    }
    return intermediateFiles;
 }
} // Namespace MapReduce
