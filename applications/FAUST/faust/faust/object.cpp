/*
 *  object.cpp
 *  FAUST - Framework for Adaptive Ubiquitous Scalable Tasks
 *
 *  Created by Ole Weidner on 11/29/08.
 *  Copyright 2008 Center for Computation & Technology. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying 
 *  LICENSE file or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include <faust/faust/object.hpp>

////////////////////////////////////////////////////////////////////////////////
//
namespace faust {
  std::string get_object_type_name(faust::object t)
  {
    switch(t.get_type()) {
      case faust::object::Job:         return "Job";
      case faust::object::JobGroup:    return "JobGroup";
      case faust::object::Service:     return "Service";
      default:
        break;
    }
    return "<Unknown>";
  }  
}

////////////////////////////////////////////////////////////////////////////////
//
faust::object::object (faust::impl::object * init, faust::object::type obj_type)
: type_(obj_type), impl_(boost::shared_ptr<faust::impl::object>(init))

//impl_ (init->_internal_weak_this.use_count() ? 
//init->shared_from_this() : boost::shared_ptr<faust::impl::object>(init))
{
}

////////////////////////////////////////////////////////////////////////////////
//
faust::object::object (boost::shared_ptr<faust::impl::object> init, faust::object::type obj_type)
: impl_(init), type_(obj_type)
{
}

////////////////////////////////////////////////////////////////////////////////
//
faust::object::type faust::object::get_type() const
{
  return type_;
}