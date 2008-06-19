//  Copyright (c) 2007 Hartmut Kaiser (hartmut.kaiser@gmail.com)
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(CREATE_PERF_MATRIX_JUN_18_2007_1058AM)
#define CREATE_PERF_MATRIX_JUN_18_2007_1058AM

#include <string>
#include <vector>

///////////////////////////////////////////////////////////////////////////////
#define PERF_MATRIX_EXECUTABLE      "/home/oweidner/run_perf_matrix.sh"
#define PERF_MATRIX_TYPE            "netperf"
#define PERF_MATRIX_LOG_TO_ADVERT   1

///////////////////////////////////////////////////////////////////////////////
int 
create_perf_matrix(int level, std::string advert_key,
    std::vector<std::string> const& hosts);

int initial_job_spawn(std::string advert_key, 
    std::vector<std::string> const& hosts);

#endif

