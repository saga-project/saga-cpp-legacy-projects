#include "HandleComparisons.hpp"
#include <boost/lexical_cast.hpp>
#include "../utils/defines.hpp"

/*********************************************************
 * HandleComparisons tries to group together the proper files*
 * and assign them to a worker to reduce                 *
 * ******************************************************/
namespace AllPairs
{
 // fileCount is the total number of files possibly outputted by
 // the map function (NUM_MAPS)
 HandleComparisons::HandleComparisons(std::vector<saga::url> &baseFiles, std::vector<saga::url> &fragmentFiles, saga::advert::directory workerDir,
                                      LogWriter *log)
    : baseFiles_ (baseFiles), fragmentFiles_(fragmentFiles), workerDir_(workerDir), log_(log)
 {
    candidateIT_ = fragmentFiles_.begin();
    workers_     = workerDir_.list("?");
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
 void HandleComparisons::assignWork()
 {
    while(finished_.size() < fragmentFiles_.size()) {
       saga::url fragmentFile(get_file_());
       issue_command_(fragmentFile);  // Try to assign the chunk to someone
    }
 }
/*********************************************************
 * issue_command takes the grouped files and tries to    *
 * assign them to an idel worker.  If a worker is done,  *
 * the results are recorded                              *
 * ******************************************************/
 void HandleComparisons::issue_command_(saga::url fragmentFile) {
   int mode = saga::advert::ReadWrite;
   static std::vector<saga::url>::iterator workers_IT = workers_.begin();
   bool assigned = false; //Describes status of current file
   while(assigned == false) {
      try {
         saga::advert::directory possibleWorker(*workers_IT, mode);
         std::string state = possibleWorker.get_attribute("STATE");
         std::cout << workers_IT->get_string() << " state is " << state << std::endl;
         if(state == WORKER_STATE_IDLE) {
            if(possibleWorker.get_attribute("COMMAND") == WORKER_COMMAND_COMPARE) {
               break;
            }
            saga::task_container tc;
            tc.add_task(possibleWorker.set_attribute<saga::task_base::Sync>("COMMAND",  WORKER_COMMAND_COMPARE));
            saga::advert::entry adv(possibleWorker.open(saga::url("./fragmentFile"), mode | saga::advert::Create));
            tc.add_task(adv.store_string<saga::task_base::Sync>(fragmentFile.get_string()));
            std::string message("Assigned worker");
            message += possibleWorker.get_url().get_path() + " to compare fragment: " + fragmentFile.get_string();
            assigned_.push_back(fragmentFile);
            tc.wait();
            log_->write(message, LOGLEVEL_INFO);
            assigned = true;
         }
         else if(state == WORKER_STATE_DONE) {
            try{
               saga::task_container tc;
               tc.add_task(possibleWorker.set_attribute<saga::task_base::Sync>("STATE",    WORKER_STATE_IDLE));
               tc.add_task(possibleWorker.set_attribute<saga::task_base::Sync>("COMMAND",  ""));
               // ./fragmentFile is the fragmentFile the worker has just finished with
               saga::advert::entry adv(possibleWorker.open(saga::url("./fragmentFile"), mode));
               std::string finished_work(adv.retrieve_string());
               finished_.push_back(saga::url(finished_work));
               std::string message("Worker ");
               message += possibleWorker.get_url().get_path() + " finished fragment " + finished_work;
               log_->write(message, LOGLEVEL_INFO);
               //finishedFile is the data of the comparison
             }
             catch(saga::exception const &e) {
                std::cerr << "SAGA EXCEPTION:  " << e.what() << std::endl;
             }
          }
       }
       catch(saga::exception const & e) {
          std::string message(e.what());
          //log->write(message, LOGLEVEL_ERROR);
       }
       workers_IT++;
       if(workers_IT == workers_.end()) {
          //Update list in case more workers joined in
          workers_ = workerDir_.list("?");
          workers_IT = workers_.begin();
       }
    }
 }
 saga::url HandleComparisons::get_file_() {
    for(unsigned int count = 0; count < fragmentFiles_.size(); count++) {
       bool finished = false;
       bool assigned = false;
       std::string candidate = candidateIT_->get_string();
       std::vector<saga::url>::iterator finished_IT = finished_.begin();
       while(finished_IT != finished_.end()) {
          if(candidate == *finished_IT) {
             finished = true;
             break;
          }
          finished_IT++;
       }
       std::vector<saga::url>::iterator assigned_IT = assigned_.begin();
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
       else if(finished == false && assigned == true) {
          candidateIT_++;
          if(candidateIT_ == fragmentFiles_.end()) {
             candidateIT_ = fragmentFiles_.begin();
          }
       }
    }
    return candidateIT_->get_string();
 }
} // namespace AllPairs
