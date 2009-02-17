/*
 *  object_iml.cpp
 *  FAUST - Framework for Adaptive Ubiquitous Scalable Tasks
 *  Website: https://macpro01.cct.lsu.edu/trac/faust
 *
 *  Created by Ole Weidner <oweidner@cct.lsu.edu> on 12/16/08.
 *  Copyright 2008-2009 Center for Computation & Technology. 
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying 
 *  LICENSE file or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include <faust/impl/object_impl.hpp>

using namespace faust::impl;

////////////////////////////////////////////////////////////////////////////
//
object::object (faust::object::type type)
: type_(type)
{
  
}

////////////////////////////////////////////////////////////////////////////
//
faust::object::type object::get_type() const
{
  return type_;
}

///////////////////////////////////////////////////////////////////////////
//
saga::impl::attribute* object::get_attributes()
{
  // TODO throw exception  
  return NULL;
}

///////////////////////////////////////////////////////////////////////////
//
saga::impl::attribute const* object::get_attributes() const
{
  // TODO throw exception
  return NULL;
}
