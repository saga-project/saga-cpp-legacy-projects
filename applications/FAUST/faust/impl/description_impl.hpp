/*
 *  description_impl.hpp 
 *  FAUST - Framework for Adaptive Ubiquitous Scalable Tasks
 *  Website: https://macpro01.cct.lsu.edu/trac/faust
 *
 *  Created by Ole Weidner <oweidner@cct.lsu.edu> on 11/30/08.
 *  Copyright 2008-2009 Center for Computation & Technology. 
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying 
 *  LICENSE file or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef FAUST_IMPL_DESCRIPTION_IMPL_HPP
#define FAUST_IMPL_DESCRIPTION_IMPL_HPP

#include <faust/faust/exports.hpp>
#include <faust/faust/description.hpp>
#include <faust/impl/object_impl.hpp>

#include <saga/impl/engine/attribute.hpp>

namespace faust
{
  namespace impl 
  {
    //////////////////////////////////////////////////////////////////////////
    //
    class job_description : public faust::impl::object, 
                        public saga::impl::attribute
    {
      
    public:
      
      job_description() : object(faust::object::Description) {};
      job_description(std::string XMLFileName) : object(faust::object::Description) {};
      
      saga::impl::attribute* get_attributes() { return this; }
      saga::impl::attribute const* get_attributes() const { return this; }
      
      // Generate a exact deep copy of this object
      saga::object clone() const;
    };
    //
    //////////////////////////////////////////////////////////////////////////
  }
}

#endif /* FAUST_IMPL_JOB_IMPL_HPP */

