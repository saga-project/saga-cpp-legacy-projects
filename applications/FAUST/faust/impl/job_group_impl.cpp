/*
 *  job_group_impl.cpp 
 *  FAUST - Framework for Adaptive Ubiquitous Scalable Tasks
 *  Website: https://macpro01.cct.lsu.edu/trac/faust
 *
 *  Created by Ole Weidner <oweidner@cct.lsu.edu> on 11/30/08.
 *  Copyright 2008-2009 Center for Computation & Technology. 
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying 
 *  LICENSE file or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include <faust/impl/job_group_impl.hpp>

using namespace faust::impl;

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR
//
job_group::job_group() 
: object(faust::object::JobGroup)
{
  std::string jobid("faust://");
  jobid.append(saga::uuid().string());
  jobid_ = jobid;          
}

////////////////////////////////////////////////////////////////////////////////
//
void job_group::run()
{
}

////////////////////////////////////////////////////////////////////////////////
//
bool job_group::wait(double timeout)
{
  return false;
}

////////////////////////////////////////////////////////////////////////////////
//
bool job_group::cancel(double timeout)
{
  return false;
}

////////////////////////////////////////////////////////////////////////////////
//
bool job_group::suspend()
{
  return false;
}

////////////////////////////////////////////////////////////////////////////////
//
bool job_group::resume()
{
  return false;
}

////////////////////////////////////////////////////////////////////////////////
//
std::string job_group::get_job_id()
{
  return jobid_;
}

std::vector<std::string> job_group::list_jobs()
{
  std::vector<std::string> ids;
  return ids;
}
