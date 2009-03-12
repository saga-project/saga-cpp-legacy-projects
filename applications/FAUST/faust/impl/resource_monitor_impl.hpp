/*
 *  resource_monitor_impl.hpp 
 *  FAUST - Framework for Adaptive Ubiquitous Scalable Tasks
 *  Website: https://macpro01.cct.lsu.edu/trac/faust
 *
 *  Created by Ole Weidner <oweidner@cct.lsu.edu> on 02/13/09.
 *  Copyright 2008-2009 Center for Computation & Technology. 
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying 
 *  LICENSE file or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef FAUST_IMPL_RESOURCE_MONITOR_IMPL_HPP
#define FAUST_IMPL_RESOURCE_MONITOR_IMPL_HPP

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
    class resource_monitor : public faust::impl::object, 
    public saga::impl::attribute
    {

    public:
			resource_monitor();

      saga::impl::attribute* get_attributes() { return this; }
      saga::impl::attribute const* get_attributes() const { return this; }
      
			std::vector<std::string> list_attributes () const;
			
			// overloaded attribute-interface methods are required to implement
			// attribute caching
			std::string get_attribute (std::string key) const;
			void set_attribute (std::string key, std::string value);
			
			std::vector<std::string> get_vector_attribute (std::string key) const;
			void set_vector_attribute (std::string key, strvec_type value);
			
      // Generate a exact deep copy of this object
      //saga::object clone() const;
    };
    //
    //////////////////////////////////////////////////////////////////////////
  }
}

#endif /* FAUST_IMPL_JOB_IMPL_HPP */