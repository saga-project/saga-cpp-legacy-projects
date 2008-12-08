/*
 *  job_group.cpp
 *  FAUST - Framework for Adaptive Ubiquitous Scalable Tasks
 *
 *  Created by Ole Weidner on 11/22/08.
 *  Copyright 2008 Center for Computation & Technology. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying 
 *  LICENSE file or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include <faust/faust/job_group.hpp>
#include <faust/impl/job_group_impl.hpp>

using namespace faust::manyjobs;

////////////////////////////////////////////////////////////////////////////////
//
job_group::job_group() 
: impl(new faust::impl::manyjobs::job_group_impl())
{
}

////////////////////////////////////////////////////////////////////////////////
//
void job_group::run() 
{ 
  impl->run(); 
}

////////////////////////////////////////////////////////////////////////////////
//
bool job_group::wait(double timeout) 
{ 
  return impl->wait(timeout); 
}

////////////////////////////////////////////////////////////////////////////////
//
bool job_group::cancel(double timeout) 
{ 
  return impl->cancel(timeout); 
}

////////////////////////////////////////////////////////////////////////////////
//
bool job_group::suspend() 
{ 
  return impl->suspend(); 
}

////////////////////////////////////////////////////////////////////////////////
//
bool job_group::resume() 
{ 
  return impl->suspend(); 
}

////////////////////////////////////////////////////////////////////////////////
//
std::string job_group::get_job_id() 
{ 
  return impl->get_job_id(); 
}      

////////////////////////////////////////////////////////////////////////////////
//
std::vector<std::string> job_group::list_job_ids() 
{ 
  return impl->list_job_ids(); 
}  
