//  Copyright (c) 2009 Miklos Erdelyi
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef MAPREDUCE_FACTORY_IMPL_HPP_
#define MAPREDUCE_FACTORY_IMPL_HPP_

// Common code needed for the different class factories.

#include "factory.hpp"

using ::factory_ns::factory; 
using ::factory_ns::signatures;

#define DEFINE_CLASS_CREATOR_INTERFACE(name, classname) \
class name {  \
 public:  \
  virtual ~name() {}  \
  virtual classname* Create() = 0; \
};

#endif    // MAPREDUCE_FACTORY_IMPL_HPP_
