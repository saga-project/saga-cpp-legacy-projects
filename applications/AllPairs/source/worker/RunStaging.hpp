//  Copyright (c) 2008 Michael Miceli and Christopher Miceli
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef AP_RUN_STAGING_HPP
#define AP_RUN_STAGING_HPP

#include <saga/saga.hpp>
#include "../utils/LogWriter.hpp"

namespace AllPairs {
   class RunStaging {
     public:
      RunStaging(const std::vector<std::string> &hosts,
                 const std::string &location, AllPairs::LogWriter *log);
      std::vector<double> getResults();
      ~RunStaging();

     private:
      std::vector<std::string> hosts_;
      std::string              location_;
      AllPairs::LogWriter     *log_;
   }; // class RunStaging
} // namespace AllPairs

#endif // MR_RUN_STAGING_HPP
