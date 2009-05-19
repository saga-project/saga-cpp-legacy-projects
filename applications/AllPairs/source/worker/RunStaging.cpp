#include "RunStaging.hpp"

namespace AllPairs {

/*********************************************************
 * RunStaging constructor sets up the state and other info   *
 * for the worker.                                       *
 * ******************************************************/
   RunStaging::RunStaging(const std::vector<std::string> &hosts, const std::string &location, AllPairs::LogWriter *log)
      : hosts_(hosts), location_(location), log_(log)
   {
   }
/*********************************************************
 * ~RunStaging destructor returns the state of the worker to *
 * what is expected by the master after a mapping is done*
 * ******************************************************/
   RunStaging::~RunStaging() {
   }

   std::vector<int> RunStaging::getResults() {
      std::vector<int> retval;
      for(std::vector<std::string>::const_iterator it = hosts_.begin();
          it != hosts_.end(); ++it) 
      {
         if(*it == location_) {
            retval.push_back(0);
         }
         else {
            retval.push_back(1);
         }
      }
      return retval;
   }
} // namespace AllPairs 

