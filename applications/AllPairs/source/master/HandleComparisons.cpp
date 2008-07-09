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
 HandleComparisons::HandleComparisons(saga::advert::directory workerDir)
    : workerDir_(workerDir)
 {
    workers_     = workerDir_.list("?");
    comparisons_ = 20;  //Completely made up at the moment
 }
/*********************************************************
 * assignReduces is the only public function that tries  *
 * to assign reduce files to idle workers                *
 * ******************************************************/
 bool HandleComparisons::assignWork()
 {
    //Until the number of finished chunks
    //equals the number of total chunks
    while(finished_.size() != comparisons_) {
       issue_command_();  // Try to assign the chunk to someone
    }
    return true;
 }
/*********************************************************
 * issue_command takes the grouped files and tries to    *
 * assign them to an idel worker.  If a worker is done,  *
 * the results are recorded                              *
 * ******************************************************/
 void HandleComparisons::issue_command_()
 {
    int mode = saga::advert::ReadWrite;
    static std::vector<saga::url>::iterator workers_IT = workers_.begin();
    bool assigned = false; //Describes status of current chunk (file)
    while(assigned == false) {
       try {
          saga::advert::directory possibleWorker(*workers_IT, mode);
          std::string state = possibleWorker.get_attribute("STATE");
          if(state == WORKER_STATE_IDLE) {
             possibleWorker.set_attribute("COMMAND", WORKER_COMMAND_COMPARE);
          }
          else if(state == WORKER_STATE_DONE) {
             saga::task t0 = possibleWorker.set_attribute<saga::task_base::ASync>("STATE",   WORKER_STATE_IDLE);
             saga::task t1 = possibleWorker.set_attribute<saga::task_base::ASync>("COMMAND", WORKER_COMMAND_COMPARE);
             t0.wait();
             t1.wait();
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
}//Namespace AllPairs
