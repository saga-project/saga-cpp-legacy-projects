/*
 *  resource_description_impl.hpp 
 *  FAUST - Framework for Adaptive Ubiquitous Scalable Tasks
 *  Website: https://macpro01.cct.lsu.edu/trac/faust
 *
 *  Created by Ole Weidner <oweidner@cct.lsu.edu> on 11/30/08.
 *  Copyright 2008-2009 Center for Computation & Technology. 
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying 
 *  LICENSE file or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef FAUST_IMPL_RESOURCE_DESCRIPTION_IMPL_HPP
#define FAUST_IMPL_RESOURCE_DESCRIPTION_IMPL_HPP

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
    class resource_description : public faust::impl::object, 
    public saga::impl::attribute
    {
		private:
      
      saga::advert::entry desc_adv_;
      
    public:
      
      resource_description();
      resource_description(saga::advert::entry & desc_adv);
      resource_description(std::string filename);
      
      void write_to_file(std::string filename);
      void read_from_file(std::string filename);
      
      saga::impl::attribute* get_attributes() { return this; }
      saga::impl::attribute const* get_attributes() const { return this; }
      
      saga::impl::attribute_cache get_cache() {return attributes_;}
      
      // Generate a exact deep copy of this object
      // saga::object clone() const;
    };
    //
    //////////////////////////////////////////////////////////////////////////
  }
}

#endif /* FAUST_IMPL_JOB_IMPL_HPP */

