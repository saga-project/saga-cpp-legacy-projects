/*
 *  resource_monitor.cpp
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

#include <faust/faust/resource_monitor.hpp>
#include <faust/impl/resource_monitor_impl.hpp>

using namespace faust;

////////////////////////////////////////////////////////////////////////////////
//
boost::shared_ptr <faust::impl::resource_monitor> resource_monitor::get_impl (void) const
{ 
  typedef faust::object base_type;
  
  return boost::static_pointer_cast 
  <faust::impl::resource_monitor> (this->base_type::get_impl ());
}

////////////////////////////////////////////////////////////////////////////////
//
void resource_monitor::setupAttributes()
{
  using namespace boost::assign;
  std::vector<std::string> valid_keys;
  valid_keys += 
  /*VECTOR*/ attributes::resource_monitor::dir_id,
  /*VECTOR*/ attributes::resource_monitor::dir_path,
  /*VECTOR*/ attributes::resource_monitor::dir_dev_space_total,
  /*VECTOR*/ attributes::resource_monitor::dir_dev_space_used,
  /*VECTOR*/ attributes::resource_monitor::dir_dev_space_free,
  /*VECTOR*/ attributes::resource_monitor::dir_quota_total,
  /*VECTOR*/ attributes::resource_monitor::dir_quota_used,
  /*VECTOR*/ attributes::resource_monitor::dir_quota_free,
  
  /*VECTOR*/ attributes::resource_monitor::queue_id,
  /*VECTOR*/ attributes::resource_monitor::queue_name,
  /*VECTOR*/ attributes::resource_monitor::queue_nodes_total,
  /*VECTOR*/ attributes::resource_monitor::queue_nodes_used,
  /*VECTOR*/ attributes::resource_monitor::queue_nodes_free
  ;
  
  // initialize list of valid keys          
  this->init_keynames(valid_keys);
  
  strmap_type attributes_scalar_rw;
  //insert(attributes_scalar_rw)
  //(attributes::resource_monitor::desc01, "")
  //;
  
  strmap_type attributes_vector_rw;
  insert(attributes_vector_rw)
  (attributes::resource_monitor::dir_id, "")
  (attributes::resource_monitor::dir_path, "")
  (attributes::resource_monitor::dir_dev_space_total, "")
  (attributes::resource_monitor::dir_dev_space_used, "")
  (attributes::resource_monitor::dir_dev_space_free, "")
  (attributes::resource_monitor::dir_quota_total, "")
  (attributes::resource_monitor::dir_quota_used, "")
  (attributes::resource_monitor::dir_quota_free, "")
  
  (attributes::resource_monitor::queue_id, "")
  (attributes::resource_monitor::queue_name, "")
  (attributes::resource_monitor::queue_nodes_total, "")
  (attributes::resource_monitor::queue_nodes_used, "")
  (attributes::resource_monitor::queue_nodes_free, "")
  
  ;
  
  
  // initialize attribute implementation
  this->init (strmap_type(), attributes_scalar_rw, 
              strmap_type(), attributes_vector_rw);
  this->init (false, true);   // cache only implementation  
}


////////////////////////////////////////////////////////////////////////////////
//
resource_monitor::resource_monitor() :
faust::object (new faust::impl::resource_monitor(), object::ResourceMonitor)
{
  this->setupAttributes();
}

////////////////////////////////////////////////////////////////////////////////
//
resource_monitor::~resource_monitor() 
{
  
}

/////////////////////////////////////////////////////////////////////////////
//  implement the attribute functions (we need to explicitly specialize 
//  the template because the functions are not implemented inline)
template struct saga::detail::attribute<faust::resource_monitor>;

template struct saga::detail::attribute_priv<faust::resource_monitor, saga::task_base::Sync>;
template struct saga::detail::attribute_priv<faust::resource_monitor, saga::task_base::Async>;
template struct saga::detail::attribute_priv<faust::resource_monitor, saga::task_base::Task>;
