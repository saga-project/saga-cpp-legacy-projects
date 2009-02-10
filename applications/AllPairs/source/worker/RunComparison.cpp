#include "RunComparison.hpp"
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
      filesIT_ = files_.begin();
   }
/*********************************************************
 * ~RunComparison destructor returns the state of the worker to *
 * what is expected by the master after a mapping is done*
 * ******************************************************/
   RunComparison::~RunComparison() {
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

