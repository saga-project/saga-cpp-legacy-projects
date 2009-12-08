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
            const int buff_size = 100;
            double answer;
            std::string command("/bin/ping -c 2 ");
            command += it->c_str();
            command += " | tail -n 1";
            std::cerr << "Netperf command: " << command.c_str() << std::endl;

            FILE *results = popen(command.c_str(), "r");
            char buffer[buff_size];
            int read = fread(buffer, 1, buff_size, results);
            pclose(results);
            if(read < buff_size)
               buffer[read] = '\0';
            else
               buffer[buff_size-1] = '\0';
            std::cerr << "string result: " << buffer << std::endl;
            sscanf(buffer, "rtt min/avg/max/mdev = %lf", &answer);
            std::cerr << "result of host (" << *it << "): " << "(" << answer << ")" << std::endl;
            retval.push_back(answer);
         }
      }
      return retval;
   }
} // namespace AllPairs 

