/*
 *  resource_impl.hpp 
 *  FAUST - Framework for Adaptive Ubiquitous Scalable Tasks
 *  Website: https://macpro01.cct.lsu.edu/trac/faust
 *
 *  Created by Ole Weidner <oweidner@cct.lsu.edu> on 02/13/08.
 *  Copyright 2008-2009 Center for Computation & Technology. 
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying 
 *  LICENSE file or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef FAUST_IMPL_RESOURCE_IMPL_HPP
#define FAUST_IMPL_RESOURCE_IMPL_HPP

#include <faust/faust/exports.hpp>
#include <faust/faust/resource_description.hpp>
#include <faust/faust/resource_monitor.hpp>

#include <faust/impl/object_impl.hpp>

namespace faust
{
  namespace impl 
  {
    //////////////////////////////////////////////////////////////////////////
    //
    class  resource : public faust::impl::object
    {
      
    private:
      faust::resource_description description_;
			faust::resource_monitor monitor_;
      
      // Persistent advert instance - used by the monitor as well!
      friend class faust::impl::resource_monitor;
      saga::advert::directory advert_base_;
      std::string resource_id_;
      bool init_from_id_;
			
			resource() : object(faust::object::Resource) {};
      
    public:
      
      explicit resource(std::string resource_identifier);
      
      explicit resource(faust::resource_description RD);
      
      ~resource();
      
      faust::resource_description get_description(); 
			faust::resource_monitor get_monitor();
    };
    //
    //////////////////////////////////////////////////////////////////////////
  }
}

#endif /* FAUST_IMPL_RESOURCE_IMPL_HPP */
