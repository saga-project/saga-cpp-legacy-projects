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
            //We are this host, return lowest value
            retval.push_back(0);
         }
         else {
            //Ping the host
            const int buff_size = 25;
            double answer;

            FILE *results = popen("/work/mmicel2/bin/netperf -H localhost | tail -n 1 | sed 's/[ ]*\\([0-9]*\\)[ ]*\\([0-9]*\\)[ ]*\\([0-9]*\\)[ ]*\\([0-9]*\\.[0-9]*\\)[ ]*\\([0-9]*\\.[0-9]*\\)/\\5/'", "r");
            char buffer[buff_size];
            int read = fread(buffer, 1, buff_size, results);
            pclose(results);
            std::cerr << "read  " << read << " bytes" << std::endl;
            std::cerr << "string result: " << buffer << std::endl;
            if(read < buff_size)
               buffer[read] = '\0';
            else
               buffer[buff_size-1] = '\0';
            std::cerr << "string result: " << buffer << std::endl;
            sscanf(buffer, "%lf", &answer);
            answer = 1 / answer;
            std::cerr << "netperf result of host (" << *it << "): " << "(" << answer << ")" << std::endl;
            retval.push_back(answer);
         }
      }
      return retval;
   }
} // namespace AllPairs 

