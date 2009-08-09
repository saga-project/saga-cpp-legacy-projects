//  Copyright (c) 2008 Michael Miceli and Christopher Miceli
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef MR_HANDLE_MAPS_HPP
#define MR_HANDLE_MAPS_HPP

#include <string>
#include <vector>
#include <set>
#include <saga/saga.hpp>
#include "../input_output.hpp"
#include "../../utils/LogWriter.hpp"
#include "../version.hpp"

namespace mapreduce {

class HandleMaps {
   public:
    HandleMaps(const JobDescription& job,
               std::vector<InputChunk*> &chunks,
               std::map<std::string, saga::url>& committed_chunks_,
               RawInputFormat* input_format,
               saga::url serverURL,
               LogWriter *log);
   ~HandleMaps();
    bool assignMaps();

   private:
    void issue_command_();
    std::string getCandidate_(saga::stream::stream& worker);

    saga::stream::server    *service_;
    const JobDescription&    job_;
    std::map<std::string, InputChunk*> chunk_assignments_;
    std::map<std::string, saga::url>& committed_chunks_;
    std::map<std::string, saga::url> worker_adverts_;
    RawInputFormat*          input_format_;
    std::set<std::string> unassigned_;
    std::set<std::string> assigned_;
    std::set<std::string> finished_;
    saga::url                serverURL_;
    LogWriter               *log_;
    std::vector<std::string>::size_type totalChunks_;
};

} // namespace mapreduce

#endif // MR_HANDLE_MAPS_HPP
