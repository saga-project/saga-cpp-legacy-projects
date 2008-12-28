/*
 *  object.cpp
 *  FAUST - Framework for Adaptive Ubiquitous Scalable Tasks
 *
 *  Created by Ole Weidner <oweidner@cct.lsu.edu> on 12/16/08.
 *  Copyright 2008 Center for Computation & Technology. All rights reserved.
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