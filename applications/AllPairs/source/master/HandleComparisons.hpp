//  Copyright (c) 2008 Michael Miceli and Christopher Miceli
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef AP_HANDLE_COMPARISONS_HPP
#define AP_HANDLE_COMPARISONS_HPP

#include <string>
#include <vector>
#include <saga/saga.hpp>
#include "../utils/LogWriter.hpp"
#include "version.hpp"

namespace AllPairs {
   class HandleComparisons {
     public:
      HandleComparisons(std::vector<saga::url> &rowFiles, std::vector<saga::url> &columnFiles, saga::advert::directory workerDir,
                        LogWriter *log);
      void assignWork();
     private:
      void issue_command_(saga::url file);
      saga::url get_file_();
    
      std::vector<saga::url> finished_;
      std::vector<saga::url> assigned_;
      std::vector<saga::url> baseFiles_;
      std::vector<saga::url> fragmentFiles_;
      std::vector<saga::url> workers_;
      //std::map<std::string, std::string> data_;

      saga::advert::directory workerDir_;
      LogWriter *log_;
      std::vector<saga::url>::iterator candidateIT_;
   };
} //Namespace AllPairs

#endif // AP_HANDLE_COMPARISONS_HPP

