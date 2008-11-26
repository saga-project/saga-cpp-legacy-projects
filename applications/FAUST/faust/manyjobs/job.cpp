/*
 *  job.hpp
 *  FAUST - Framework for Adaptive Ubiquitous Scalable Tasks
 *
 *  Created by Ole Weidner on 11/22/08.
 *  Copyright 2008 Center for Computation & Technology. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying 
 *  LICENSE file or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include <saga/impl/exception.hpp>

#include <faust/faust/defines.hpp>
#include <faust/manyjobs/job.hpp>

using namespace faust;

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR 
//
manyjobs::job::job() 
: jobid_(saga::uuid().string()), state_(New)
{
  
}
////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR
//
manyjobs::job::~job() 
{
  
}

////////////////////////////////////////////////////////////////////////////////
// 
//
std::string manyjobs::job::get_job_id()
{
  return jobid_;
}

////////////////////////////////////////////////////////////////////////////////
// 
//
faust::manyjobs::state manyjobs::job::get_state()
{
  return state_;
}