/*
 *  resource_monitor_impl.cpp 
 *  FAUST - Framework for Adaptive Ubiquitous Scalable Tasks
 *  Website: https://macpro01.cct.lsu.edu/trac/faust
 *
 *  Created by Ole Weidner <oweidner@cct.lsu.edu> on 02/13/09.
 *  Copyright 2008-2009 Center for Computation & Technology. 
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying 
 *  LICENSE file or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include <faust/impl/resource_monitor_impl.hpp>

using namespace faust::impl;

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR
resource_monitor::resource_monitor(saga::advert::entry & monitor_adv)
: object(faust::object::ResourceMonitor), monitor_adv_(monitor_adv)
{
  std::string identifier(FW_NAME); std::string msg("");
  identifier.append(" faust::resource_monitor ("+get_uuid()+")"); 
  log_ = new detail::logwriter(identifier, std::cout);
}

std::vector<std::string> resource_monitor::list_attributes()
{  
  return monitor_adv_.list_attributes();
}

std::string resource_monitor::get_attribute (std::string key) const
{
  return monitor_adv_.get_attribute(key);
}

void resource_monitor::set_attribute (std::string key, std::string value)
{
  return monitor_adv_.set_attribute(key, value);
}

std::vector<std::string> resource_monitor::get_vector_attribute (std::string key) const
{
  return monitor_adv_.get_vector_attribute(key);
}
void resource_monitor::set_vector_attribute (std::string key, strvec_type value)
{
  return monitor_adv_.set_vector_attribute(key, value);
}

bool resource_monitor::attribute_is_vector (std::string key) const
{
  std::cout << "IS V" << std::endl;
  return monitor_adv_.attribute_is_vector(key);
}