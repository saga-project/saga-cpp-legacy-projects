#include "RunComparison.hpp"
#include "../utils/getElements.hpp"
#include "../utils/defines.hpp"

namespace AllPairs {

/*********************************************************
 * RunComparison constructor sets up the state and other info   *
 * for the worker.                                       *
 * ******************************************************/
   RunComparison::RunComparison(saga::advert::directory workerDir,
                  std::vector<saga::url> files, LogWriter *log) :
      workerDir_(workerDir), files_(files), log_(log)
   {
      try {
         filesIT_ = files_.begin();
         workerDir_.set_attribute("STATE", WORKER_STATE_COMPARING);
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
   }

/*********************************************************
 * getFile retreives the chunk that was posted for this  *
 * worker to use from the advert database.               *
 * ******************************************************/
   bool RunComparison::hasComparisons()
   {
      if(filesIT_ != files_.end())
         return true;
      return false;
   }
   saga::url RunComparison::getComparisons() {
      saga::url file(*filesIT_);
      filesIT_++;
      return file;
   }
} // namespace AllPairs 

