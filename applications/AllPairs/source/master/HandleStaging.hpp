//  Copyright (c) 2008 Michael Miceli and Christopher Miceli
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef AP_HANDLE_STAGING_HPP
#define AP_HANDLE_STAGING_HPP

#include <string>
#include <vector>
#include <saga/saga.hpp>
#include <algorithm>
#include "../utils/LogWriter.hpp"
#include "../utils/network.hpp"
#include "../utils/Graph.hpp"
#include "../utils/defines.hpp"
#include "ConfigFileParser.hpp"

namespace AllPairs {
   class HandleStaging {
     public:
      HandleStaging(const std::string &serverURL_,
                    const std::vector<Master::HostDescription> &hostList,
                    const std::vector<Master::FileDescription> &files,
                    LogWriter *log);
      ~HandleStaging();
      Graph getNetwork(void);
     private:
      void assignStages_();

      unsigned int             numWorkers_;
      std::string              serverURL_;
      LogWriter               *log_;
      saga::stream::server    *service_;
      std::vector<std::string> hosts_;
      std::vector<std::string> finishedHosts_;
      Graph                    networkGraph_;
   };                                    
} //Namespace AllPairs

#endif // AP_HANDLE_STAGING_HPP

