/*
 *  job.cpp
 *  FAUST - Framework for Adaptive Ubiquitous Scalable Tasks
 *  Website: https://macpro01.cct.lsu.edu/trac/faust
 *
 *  Created by Ole Weidner <oweidner@cct.lsu.edu> on 11/22/08.
 *  Copyright 2008-2009 Center for Computation & Technology. 
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying 
 *  LICENSE file or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include <faust/faust/job.hpp>
#include <faust/impl/job_impl.hpp>

using namespace faust;

////////////////////////////////////////////////////////////////////////////////
//
boost::shared_ptr <faust::impl::job> job::get_impl (void) const
{ 
  typedef faust::object base_type;
  return boost::static_pointer_cast <faust::impl::job> (
                                                        this->base_type::get_impl ());
}

////////////////////////////////////////////////////////////////////////////////
//
job::job() 
: faust::object (new faust::impl::job(), object::Job)
{
}

////////////////////////////////////////////////////////////////////////////////
//
void job::run() 
{ 
  get_impl()->run(); 
}

////////////////////////////////////////////////////////////////////////////////
//
bool job::wait(double timeout) 
{ 
  return get_impl()->wait(timeout); 
}

////////////////////////////////////////////////////////////////////////////////
//
bool job::cancel(double timeout) 
{ 
  return get_impl()->cancel(timeout); 
}

////////////////////////////////////////////////////////////////////////////////
//
bool job::suspend() 
{ 
  return get_impl()->suspend(); 
}

////////////////////////////////////////////////////////////////////////////////
//
bool job::resume() 
{ 
  return get_impl()->suspend(); 
}

////////////////////////////////////////////////////////////////////////////////
//
std::string job::get_job_id() 
{ 
  return get_impl()->get_job_id(); 
}      

////////////////////////////////////////////////////////////////////////////////
//
faust::state job::get_state()
{
  return get_impl()->get_state();
}

////////////////////////////////////////////////////////////////////////////////
//
faust::job_description job::get_description()
{
  return get_impl()->get_description();
}
