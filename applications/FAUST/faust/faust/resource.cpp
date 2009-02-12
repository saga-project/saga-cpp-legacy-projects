/*
 *  resource_description.cpp
 *  FAUST - Framework for Adaptive Ubiquitous Scalable Tasks
 *
 *  Created by Ole Weidner <oweidner@cct.lsu.edu> on 11/22/08.
 *  Copyright 2008-2009 Center for Computation & Technology. 
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying 
 *  LICENSE file or copy at http://www.boost.org/LICENSE_1_0.txt)
 */


#include <boost/assign/list_inserter.hpp>
#include <boost/assign/std/vector.hpp>

#include <faust/faust/resource.hpp>
#include <faust/impl/resource_impl.hpp>

using namespace faust;

////////////////////////////////////////////////////////////////////////////////
//
boost::shared_ptr <faust::impl::resource_description> resource_description::get_impl (void) const
{ 
  typedef faust::object base_type;
  
  return boost::static_pointer_cast 
  <faust::impl::resource_description> (this->base_type::get_impl ());
}

////////////////////////////////////////////////////////////////////////////////
//
void resource_description::setupAttributes()
{
  using namespace boost::assign;
  std::vector<std::string> valid_keys;
  valid_keys += 
  attributes::resource_description::dir_id,
  attributes::resource_description::dir_path,
  attributes::resource_description::dir_dev_space_total_cmd,
  attributes::resource_description::dir_dev_space_used_cmd,
  attributes::resource_description::dir_quota_total_cmd,
  attributes::resource_description::dir_quota_used_cmd
  ;
  
  // initialize list of valid keys          
  this->init_keynames(valid_keys);
  
  strmap_type attributes_scalar_rw;
  //insert(attributes_scalar_rw)
  //(attributes::resource_description::desc01, "")
  //;
  
  strmap_type attributes_vector_rw;
  insert(attributes_vector_rw)
  (attributes::resource_description::dir_id, "")
  (attributes::resource_description::dir_path, "")
  (attributes::resource_description::dir_dev_space_total_cmd, "")
  (attributes::resource_description::dir_dev_space_used_cmd, "")
  (attributes::resource_description::dir_quota_total_cmd, "")
  (attributes::resource_description::dir_quota_used_cmd, "")
  ;
  
  
  // initialize attribute implementation
  this->init (strmap_type(), attributes_scalar_rw, 
              strmap_type(), attributes_vector_rw);
  this->init (false, true);   // cache only implementation  
}

////////////////////////////////////////////////////////////////////////////////
//
resource_description::resource_description() :
  faust::object (new faust::impl::resource_description(), object::Resource)
{
  this->setupAttributes();
}

////////////////////////////////////////////////////////////////////////////////
//
resource_description::resource_description(std::string XMLFileName) :
  faust::object (new faust::impl::resource_description(XMLFileName), object::Resource)
{
  this->setupAttributes();
}

////////////////////////////////////////////////////////////////////////////////
//
resource_description::~resource_description() 
{
  
}

