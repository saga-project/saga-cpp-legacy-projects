/*
 *  job_group_impl.cpp 
 *  FAUST - Framework for Adaptive Ubiquitous Scalable Tasks
 *
 *  Created by Ole Weidner on 11/30/08.
 *  Copyright 2008 Center for Computation & Technology. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying 
 *  LICENSE file or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include <faust/impl/manyjobs/job_impl.hpp>

using namespace faust::manyjobs::impl;

job_impl::job_impl() 
{
  std::string jobid("manyjob://");
  jobid.append(saga::uuid().string());
  jobid_ = jobid;          
}

std::string job_impl::get_job_id()
{
  return jobid_;
}