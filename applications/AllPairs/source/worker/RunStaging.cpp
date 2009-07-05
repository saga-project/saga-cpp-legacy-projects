#include "RunStaging.hpp"
#include <stdio.h>

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

   std::vector<double> RunStaging::getResults() {
      std::vector<double> retval;
      for(std::vector<std::string>::const_iterator it = hosts_.begin();
          it != hosts_.end(); ++it) 
      {
         if(*it == location_) {
            //std::cerr << "location same, so no ping, return 0" << std::endl;
            retval.push_back(0);
         }
         else {
            //Ping the host
            std::string command;
            const int buff_size = 10;
            double answer;

            command = "ping " + *it  + " -c 1 | grep -e \"=[0-9]*\\.[0-9]*\" -o | tail -c +2 | head -c -1"; 
            FILE *results = popen(command.c_str(), "r");
            char buffer[buff_size];
            int read = fread(buffer, 1, buff_size, results);
            if(read < buff_size)
               buffer[read] = '\0';
            else
               buffer[buff_size-1] = '\0';
            sscanf(buffer, "%lf", &answer);
            //std::cerr << "ping of host (" << *it << "): " << "(" << answer << ")" << std::endl;
            retval.push_back(answer);
            pclose(results);
         }
      }
      return retval;
   }
} // namespace AllPairs 

