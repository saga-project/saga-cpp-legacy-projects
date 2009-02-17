/*
 *  description.cpp
 *  FAUST - Framework for Adaptive Ubiquitous Scalable Tasks
 *  Website: https://macpro01.cct.lsu.edu/trac/faust
 *
 *  Created by Ole Weidner <oweidner@cct.lsu.edu> on 11/22/08.
 *  Copyright 2008-2009 Center for Computation & Technology. 
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying 
 *  LICENSE file or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include <saga/saga.hpp>
#include <saga/saga/detail/attribute_impl.hpp>

#include <boost/assign/list_inserter.hpp>
#include <boost/assign/std/vector.hpp>

#include <faust/faust/description.hpp>
#include <faust/impl/description_impl.hpp>

using namespace faust;

////////////////////////////////////////////////////////////////////////////////
//
boost::shared_ptr <faust::impl::job_description> job_description::get_impl (void) const
{ 
  typedef faust::object base_type;
  
  return boost::static_pointer_cast 
  <faust::impl::job_description> (this->base_type::get_impl ());
}

////////////////////////////////////////////////////////////////////////////////
//
void job_description::setupAttributes()
{
  using namespace boost::assign;
  std::vector<std::string> valid_keys;
  valid_keys += 
  attributes::job_description::desc01,
  attributes::job_description::desc02
  ;
  
  // initialize list of valid keys          
  this->init_keynames(valid_keys);
  
  strmap_type attributes_scalar_rw;
  insert(attributes_scalar_rw)
  (attributes::job_description::desc01, "")
  ;
  
  strmap_type attributes_vector_rw;
  insert(attributes_vector_rw)
  (attributes::job_description::desc02, "")
  ;
  
  
  // initialize attribute implementation
  this->init (strmap_type(), attributes_scalar_rw, 
              strmap_type(), attributes_vector_rw);
  this->init (false, true);   // cache only implementation  
}

////////////////////////////////////////////////////////////////////////////////
//
job_description::job_description() 
: faust::object (new faust::impl::job_description(), object::Description)
{
  this->setupAttributes();
}

////////////////////////////////////////////////////////////////////////////////
//
job_description::job_description(std::string XMLFile) 
: faust::object (new faust::impl::job_description(XMLFile), object::Description)
{
  this->setupAttributes();
}

////////////////////////////////////////////////////////////////////////////////
//
job_description::~job_description() 
{
  
}

/////////////////////////////////////////////////////////////////////////////
//  implement the attribute functions (we need to explicitly specialize 
//  the template because the functions are not implemented inline)
template struct saga::detail::attribute<faust::job_description>;

template struct saga::detail::attribute_priv<faust::job_description, saga::task_base::Sync>;
template struct saga::detail::attribute_priv<faust::job_description, saga::task_base::Async>;
template struct saga::detail::attribute_priv<faust::job_description, saga::task_base::Task>;

