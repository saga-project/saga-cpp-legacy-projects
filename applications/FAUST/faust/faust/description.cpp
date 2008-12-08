/*
 *  description.cpp
 *  FAUST - Framework for Adaptive Ubiquitous Scalable Tasks
 *
 *  Created by Ole Weidner on 11/22/08.
 *  Copyright 2008 Center for Computation & Technology. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying 
 *  LICENSE file or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include <saga/impl/exception.hpp>

#include <faust/faust/defines.hpp>
#include <faust/faust/description.hpp>

using namespace faust::manyjobs;

description::description() : saga::job::description()
{
  
}

description::~description() 
{
  
}

void description::set_attribute(std::string key, std::string value)
{
  saga::job::description::set_attribute(key, value);
}

std::string description::get_attribute(std::string key)
{
  return saga::job::description::get_attribute(key);
}