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

#include <boost/thread/thread.hpp>

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
			// everything that is needed for the resource api
      friend class faust::impl::resource_monitor;
      faust::resource_description description_;
			faust::resource_monitor monitor_;
      
			// advert service handles that are used quite frequently
      saga::advert::directory advert_base_;
      saga::advert::entry cmd_;
      saga::advert::entry args_;
      saga::advert::entry rm_;
      saga::advert::entry rd_;
      
      std::string resource_id_;
			std::string endpoint_str_;
      std::string agent_uuid_;
      
      bool init_from_id_;
      bool persistent_;
			
			// outsourced functions that are used in multiple places
      void launch_agent(unsigned int timeout=30);
      void send_command(std::string cmd, unsigned int timeout=30);
			
			// the main eventloop thread & entry-point fucntion
			boost::thread service_thread_;
			static void main_event_loop();
      
			// private default constructor 
			resource() : object(faust::object::Resource) {};
      
    public:
      
			// c'tors d'tor
      explicit resource(std::string resource_identifiers);
      explicit resource(faust::resource_description resource_desc, bool persistent);
      ~resource();
      
      faust::resource_description get_description(); 
			faust::resource_monitor get_monitor();
      
      bool is_persistent(void);
      void set_persistent(bool yesno);
    };
    //
    //////////////////////////////////////////////////////////////////////////
  }
}

#endif /* FAUST_IMPL_RESOURCE_IMPL_HPP */
