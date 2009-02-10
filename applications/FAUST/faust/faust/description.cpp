/*
 *  description.cpp
 *  FAUST - Framework for Adaptive Ubiquitous Scalable Tasks
 *
 *  Created by Ole Weidner <oweidner@cct.lsu.edu> on 11/22/08.
 *  Copyright 2008-2009 Center for Computation & Technology. 
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying 
 *  LICENSE file or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include <saga/impl/exception.hpp>

#include <faust/faust/description.hpp>
#include <faust/impl/description_impl.hpp>

using namespace faust;

////////////////////////////////////////////////////////////////////////////////
//
boost::shared_ptr <faust::impl::description> description::get_impl (void) const
{ 
  typedef faust::object base_type;
  
  return boost::static_pointer_cast 
  <faust::impl::description> (this->base_type::get_impl ());
}

description::description() //: saga::job::description()
{
  
}

description::~description() 
{
  
}

/*void description::set_attribute(std::string key, std::string value)
{
  //saga::job::description::set_attribute(key, value);
}

std::string description::get_attribute(std::string key)
{
  //return saga::job::description::get_attribute(key);
}*/