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
 HandleReduces::HandleReduces(int fileCount, saga::advert::directory workerDir, 
                              LogWriter *log)
    : fileCount_(fileCount), workerDir_(workerDir), log_(log)
 {
    workers_ = workerDir_.list("?");
    while(workers_.size() == 0)
    {
       sleep(1);
       workers_ = workerDir_.list("?");
    }
 }
/*********************************************************
 * assignReduces is the only public function that tries  *
 * to assign reduce files to idle workers                *
 * ******************************************************/
 bool HandleReduces::assignReduces()
 {
    for(int counter = 0; counter < fileCount_; counter++)
    {
       // group all files that were mapped to this counter
       std::vector<std::string> reduceInput(groupFiles_(counter));
       issue_command_(reduceInput, counter);
    }
    //All were assigned, now wait for everyone to finish
    while(finished_.size() != (unsigned)fileCount_)
    {
       wait_for_results_();
    }
    return true;
 }
/*********************************************************
 * issue_command takes the grouped files and tries to    *
 * assign them to an idel worker.  If a worker is done,  *
 * the results are recorded                              *
 * ******************************************************/
 void HandleReduces::issue_command_(std::vector<std::string> &inputs, int count)
 {
   int mode = saga::advert::ReadWrite | saga::advert::Create;
   static std::vector<saga::url>::iterator workers_IT = workers_.begin();
   bool assigned = false;
   while(assigned == false)
   {
      try
      {
         saga::advert::directory possibleWorker(*workers_IT, mode);
         std::string state = possibleWorker.get_attribute("STATE");
         if(state == WORKER_STATE_IDLE)
         {
            if(possibleWorker.get_attribute("COMMAND") == WORKER_COMMAND_REDUCE) {
               //Assigned but never started working
               break;
            }
            std::string message("Issuing worker ");
            message += workers_IT->get_path();
            message = message + " to reduce hash number ";
            message += boost::lexical_cast<std::string>(count);
            log_->write(message, LOGLEVEL_INFO);

            saga::advert::directory workerChunkDir(possibleWorker.open_dir(saga::url(ADVERT_DIR_REDUCE_INPUT), mode));
            for(unsigned int counter = 0; counter < inputs.size(); counter++)
            {
               saga::advert::entry adv(workerChunkDir.open(saga::url("./input-"+boost::lexical_cast<std::string>(counter)), mode));
               adv.store_string(inputs[count]);
            }
            possibleWorker.set_attribute("COMMAND", WORKER_COMMAND_REDUCE);
            assigned = true;
         }
         else if(state == WORKER_STATE_DONE)
         {
            std::string message("Worker ");
            message += workers_IT->get_path();
            message = message + " finished reducing with output ";
            saga::advert::entry output(possibleWorker.open(saga::url("./output"), mode));
            std::string finishedFile = output.retrieve_string();
            message += finishedFile;
            log_->write(message, LOGLEVEL_INFO);
            finished_.push_back(finishedFile);
            message.clear();
            message = "Issuing worker ";
            message += workers_IT->get_string();
            message = message + " to reduce hash number ";
            message += boost::lexical_cast<std::string>(count);
            log_->write(message, LOGLEVEL_INFO);
            saga::advert::directory workerChunkDir(possibleWorker.open_dir(saga::url(ADVERT_DIR_REDUCE_INPUT), mode));
            for(unsigned int count = 0; count < inputs.size(); count++)
            {
               saga::advert::entry adv(workerChunkDir.open(saga::url("./input-"+boost::lexical_cast<std::string>(count)), mode | saga::advert::Create));
               adv.store_string(inputs[count]);
            }
            possibleWorker.set_attribute("COMMAND", WORKER_COMMAND_REDUCE);
            possibleWorker.set_attribute("STATE", WORKER_STATE_IDLE);
            assigned = true;
         }
      }
      catch(saga::exception const & e) {
         throw;
      }
      workers_IT++;
      if(workers_IT == workers_.end())
      {
         workers_ = workerDir_.list("?");
         workers_IT = workers_.begin();
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
    while(workers_IT != workers_.end())
    {
       try
       {
          saga::advert::directory worker(*workers_IT, mode);
          saga::advert::directory data(worker.open_dir(saga::url(ADVERT_DIR_INTERMEDIATE), mode));
          if(data.exists(saga::url("./mapFile-" + boost::lexical_cast<std::string>(counter))))
          {
             saga::advert::entry     adv(data.open(saga::url("./mapFile-" + boost::lexical_cast<std::string>(counter)), mode));
             std::string path = adv.retrieve_string();
             std::cerr << "added file " << path << " to input list" << std::endl;
             intermediateFiles.push_back(adv.retrieve_string());
          }
          workers_IT++;
       }
       catch(saga::exception const & e) {
          throw;
       }
    }
    return intermediateFiles;
 }
 
 /*********************************************************
 * wait_for_results waits for results :)
 * ******************************************************/
 void HandleReduces::wait_for_results_()
 {
   int mode = saga::advert::ReadWrite;
   static std::vector<saga::url>::iterator workers_IT = workers_.begin();
   while(workers_IT != workers_.end())
   {
      try
      {
         saga::advert::directory possibleWorker(*workers_IT, mode);
         std::string state = possibleWorker.get_attribute("STATE");
         if(state == WORKER_STATE_DONE)
         {
            std::string message("Worker ");
            message += workers_IT->get_string();
            message = message + " finished reducing with output ";
            saga::advert::entry output(possibleWorker.open(saga::url("./output"), mode));
            std::string finishedFile = output.retrieve_string();
            message += finishedFile;
            log_->write(message, LOGLEVEL_INFO);
            finished_.push_back(finishedFile);
            return;
         }
      }
      catch(saga::exception const & e) {
         throw;
      }
      workers_IT++;
      if(workers_IT == workers_.end())
      {
         workers_ = workerDir_.list("?");
         workers_IT = workers_.begin();
      }
   }
 }

}//Namespace MapReduce
