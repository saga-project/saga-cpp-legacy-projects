/*
 *  job_group.cpp
 *  FAUST - Framework for Adaptive Ubiquitous Scalable Tasks
 *
 *  Created by Ole Weidner <oweidner@cct.lsu.edu> on 11/22/08.
 *  Copyright 2008-2009 Center for Computation & Technology. 
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying 
 *  LICENSE file or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include <faust/faust/job_group.hpp>
#include <faust/impl/job_group_impl.hpp>

using namespace faust;

////////////////////////////////////////////////////////////////////////////////
//
boost::shared_ptr <faust::impl::job_group> job_group::get_impl (void) const
{ 
  typedef faust::object base_type;
  return boost::static_pointer_cast <faust::impl::job_group> (
                                                             this->base_type::get_impl ());
}

////////////////////////////////////////////////////////////////////////////////
//
job_group::job_group() 
: faust::object (new faust::impl::job_group(), object::JobGroup)
{
}

////////////////////////////////////////////////////////////////////////////////
//
void job_group::run() 
{ 
  get_impl()->run(); 
}

////////////////////////////////////////////////////////////////////////////////
//
bool job_group::wait(double timeout) 
{ 
  return get_impl()->wait(timeout); 
}

////////////////////////////////////////////////////////////////////////////////
//
bool job_group::cancel(double timeout) 
{ 
  return get_impl()->cancel(timeout); 
}

////////////////////////////////////////////////////////////////////////////////
//
bool job_group::suspend() 
{ 
  return get_impl()->suspend(); 
}

////////////////////////////////////////////////////////////////////////////////
//
bool job_group::resume() 
{ 
  return get_impl()->suspend(); 
}

////////////////////////////////////////////////////////////////////////////////
//
std::string job_group::get_job_id() 
{ 
  return get_impl()->get_job_id(); 
}      

////////////////////////////////////////////////////////////////////////////////
//
std::vector<std::string> job_group::list_jobs() 
{ 
  return get_impl()->list_jobs(); 
}  
