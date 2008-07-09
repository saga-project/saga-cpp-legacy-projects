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
 HandleReduces::HandleReduces(int fileCount, saga::advert::directory workerDir)
    : fileCount_(fileCount), workerDir_(workerDir)
 {
    std::cerr << "start reduces with command of workers: " << std::endl;
    workers_ = workerDir_.list("?");
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
       std::cerr << "List of reduceInputs for " << counter << std::endl;
       for(std::vector<std::string>::iterator x = reduceInput.begin();x!=reduceInput.end();x++) {
          std::cerr << *x << std::endl;
       }
       issue_command_(reduceInput);
    }
//       sleep(10);
    return true;
 }
/*********************************************************
 * issue_command takes the grouped files and tries to    *
 * assign them to an idel worker.  If a worker is done,  *
 * the results are recorded                              *
 * ******************************************************/
 void HandleReduces::issue_command_(std::vector<std::string> &inputs)
 {
   int mode = saga::advert::ReadWrite;
   static std::vector<saga::url>::iterator workers_IT = workers_.begin();
   bool assigned = false;
   while(assigned == false)
   {
      try
      {
         saga::advert::directory possibleWorker(*workers_IT, mode);
         std::string state = possibleWorker.get_attribute("STATE");
         std::cerr << "State of worker " << *workers_IT << " is " << state << std::endl;
         if(state == WORKER_STATE_IDLE)
         {
            saga::advert::directory workerChunkDir(possibleWorker.open_dir(saga::url(ADVERT_DIR_REDUCE_INPUT), mode));
            for(unsigned int count = 0; count < inputs.size(); count++)
            {
               saga::advert::entry adv(workerChunkDir.open(saga::url("./input-"+boost::lexical_cast<std::string>(count)), mode | saga::advert::Create));
               adv.store_string(inputs[count]);
            }
            possibleWorker.set_attribute("COMMAND", WORKER_COMMAND_REDUCE);
            assigned = true;
         }
         else if(state == WORKER_STATE_DONE)
         {
            saga::advert::entry output(possibleWorker.open(saga::url("./output"), mode));
            std::string finishedFile = output.retrieve_string();
            finished_.push_back(finishedFile);
            possibleWorker.set_attribute("STATE", WORKER_STATE_IDLE);
            //Now that we have results, put them to work
/*            if(NUM_MAPS != finishedFile) //Didn't just finish this file
            {
               .store_string(file);
               possibleWorker.set_attribute("COMMAND", WORKER_COMMAND_MAP);
               assigned = true;
            }*/
            if(NUM_MAPS != finished_.size())
            {
               break;
            }
         }
      }
      catch(saga::exception const & e) {
 //        std::string message("Failure (" + e.what() + ")");
//         log->write(message, LOGLEVEL_ERROR);
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
             intermediateFiles.push_back(adv.retrieve_string());
          }
          workers_IT++;
       }
       catch(saga::exception const & e) {
          std::string message(e.what());
          std::cerr << message << std::endl;
//          log->write(message, LOGLEVEL_ERROR);
       }
    }
    return intermediateFiles;
 }
}//Namespace MapReduce
