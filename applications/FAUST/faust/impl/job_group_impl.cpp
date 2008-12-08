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

#include <faust/impl/job_group_impl.hpp>

using namespace faust::impl;

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR
//
job_group_impl::job_group_impl() 
{
  std::string jobid("faust://");
  jobid.append(saga::uuid().string());
  jobid_ = jobid;          
}

////////////////////////////////////////////////////////////////////////////////
//
void job_group_impl::run()
{
}

////////////////////////////////////////////////////////////////////////////////
//
bool job_group_impl::wait(double timeout)
{
  return false;
}

////////////////////////////////////////////////////////////////////////////////
//
bool job_group_impl::cancel(double timeout)
{
  return false;
}

////////////////////////////////////////////////////////////////////////////////
//
bool job_group_impl::suspend()
{
  return false;
}

////////////////////////////////////////////////////////////////////////////////
//
bool job_group_impl::resume()
{
  return false;
}

////////////////////////////////////////////////////////////////////////////////
//
std::string job_group_impl::get_job_id()
{
  return jobid_;
}

std::vector<std::string> job_group_impl::list_job_ids()
{
  std::vector<std::string> ids;
  return ids;
}
