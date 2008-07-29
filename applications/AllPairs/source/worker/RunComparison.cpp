#include "RunComparison.hpp"
#include "../utils/getElements.hpp"
#include "../utils/defines.hpp"

namespace AllPairs {

/*********************************************************
 * RunComparison constructor sets up the state and other info   *
 * for the worker.                                       *
 * ******************************************************/
   RunComparison::RunComparison(saga::advert::directory workerDir,
                  LogWriter *log) :
      workerDir_(workerDir), log_(log)
   {
      try {
         workerDir_.set_attribute("STATE", WORKER_STATE_COMPARING);
         saga::advert::entry adv(workerDir_.open(saga::url("./file"), saga::advert::ReadWrite));
      }
      catch(saga::exception const & e) {
         throw;
      }
   }
/*********************************************************
 * ~RunComparison destructor returns the state of the worker to *
 * what is expected by the master after a mapping is done*
 * ******************************************************/
   RunComparison::~RunComparison() {
      try {
         workerDir_.set_attribute("STATE", WORKER_STATE_DONE);
         workerDir_.set_attribute("COMMAND", "");
      }
      catch(saga::exception const & e) {
         throw;
      }
      sleep(5);
   }

/*********************************************************
 * getFile retreives the chunk that was posted for this  *
 * worker to use from the advert database.               *
 * ******************************************************/
   std::vector<std::string> RunComparison::getComparisons() {
      std::vector<std::string> retVal;
      try {
         retVal = getElements(file_);
      }
      catch(saga::exception const & e) {
         throw;
      }
      return retVal;
   }
} // namespace AllPairs 

