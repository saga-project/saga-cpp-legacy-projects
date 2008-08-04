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
 HandleComparisons::HandleComparisons(std::vector<saga::url> &files, saga::advert::directory workerDir,
                                      LogWriter *log)
    : files_(files), workerDir_(workerDir), log_(log)
 {
    candidateIT_ = files_.begin();
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
 bool HandleComparisons::assignWork()
 {
    while(finished_.size() < files_.size()) {
       saga::url file(get_file_());
       issue_command_(file);  // Try to assign the chunk to someone
    }
    return true;
 }
/*********************************************************
 * issue_command takes the grouped files and tries to    *
 * assign them to an idel worker.  If a worker is done,  *
 * the results are recorded                              *
 * ******************************************************/
 void HandleComparisons::issue_command_(saga::url file)
 {
    int mode = saga::advert::ReadWrite;
    static std::vector<saga::url>::iterator workers_IT = workers_.begin();
    bool assigned = false; //Describes status of current file
    while(assigned == false) {
       try {
          saga::advert::directory possibleWorker(*workers_IT, mode);
          std::string state = possibleWorker.get_attribute("STATE");
          if(state == WORKER_STATE_IDLE) {
             if(possibleWorker.get_attribute("COMMAND") == WORKER_COMMAND_COMPARE)
             {
                break;
             }
             saga::task_container tc;
             tc.add_task(possibleWorker.set_attribute<saga::task_base::ASync>("STATE",    WORKER_STATE_IDLE));
             tc.add_task(possibleWorker.set_attribute<saga::task_base::ASync>("COMMAND",  WORKER_COMMAND_COMPARE));
             saga::advert::entry adv(possibleWorker.open(saga::url("./file"), mode | saga::advert::Create));
             tc.add_task(adv.store_string<saga::task_base::ASync>(file.get_string()));
             std::cerr << "Assigned worker " << possibleWorker.get_url().get_string() << " to compare all in " << file.get_string() << " to everything else" << std::endl;
             assigned_.push_back(file);
             tc.wait();
             assigned = true;
          }
          else if(state == WORKER_STATE_DONE) {
             saga::task_container tc;
             tc.add_task(possibleWorker.set_attribute<saga::task_base::ASync>("STATE",    WORKER_STATE_IDLE));
             tc.add_task(possibleWorker.set_attribute<saga::task_base::ASync>("COMMAND",  WORKER_COMMAND_COMPARE));
             saga::advert::entry adv(possibleWorker.open(saga::url("./file"), mode | saga::advert::Create));
             finished_.push_back(saga::url(adv.retrieve_string()));
             tc.add_task(adv.store_string<saga::task_base::ASync>(file.get_string()));
             assigned_.push_back(file);
             std::cerr << "Assigned worker " << possibleWorker.get_url().get_string() << " to compare all in " << file.get_string() << " to everything else" << std::endl;
             tc.wait();
             assigned = true;
          }
       }
       catch(saga::exception const & e) {
          std::string message(e.what());
//          log->write(message, LOGLEVEL_ERROR);
       }
       workers_IT++;
       if(workers_IT == workers_.end()) {
          //Update list in case more workers joined in
          workers_ = workerDir_.list("?");
          workers_IT = workers_.begin();
       }
    }
 }
 saga::url HandleComparisons::get_file_()
 {
    for(unsigned int count = 0; count < files_.size(); count++) {
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
       if(finished == false && assigned == true) {
          candidateIT_++;
          if(candidateIT_ == files_.end()) {
             candidateIT_ = files_.begin();
          }
       }
    }
    return candidateIT_->get_string();
 }
}//Namespace AllPairs
