/*
 *  job.cpp 
 *  FAUST - Framework for Adaptive Ubiquitous Scalable Tasks
 *
 *  Created by Ole Weidner <oweidner@cct.lsu.edu> on 11/30/08.
 *  Copyright 2008-2009 Center for Computation & Technology. 
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying 
 *  LICENSE file or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include <faust/impl/job_impl.hpp>

using namespace faust::impl;

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR
job::job() 
: object(faust::object::Job)
{
  std::string jobid("faust://");
  jobid.append(saga::uuid().string());
  jobid_ = jobid;          
}

////////////////////////////////////////////////////////////////////////////////
//
void job::run()
{
}

////////////////////////////////////////////////////////////////////////////////
//
bool job::wait(double timeout)
{
  return false;
}

////////////////////////////////////////////////////////////////////////////////
//
bool job::cancel(double timeout)
{
  return false;
}

////////////////////////////////////////////////////////////////////////////////
//
bool job::suspend()
{
  return false;
}

////////////////////////////////////////////////////////////////////////////////
//
bool job::resume()
{
  return false;
}

////////////////////////////////////////////////////////////////////////////////
//
std::string job::get_job_id()
{
  return jobid_;
}

////////////////////////////////////////////////////////////////////////////////
//
faust::state job::get_state()
{
  return state_;
}

////////////////////////////////////////////////////////////////////////////////
//
faust::description job::get_description()
{
  return description_;
}
