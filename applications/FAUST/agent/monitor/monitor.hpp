/*
 *  monitor.hpp 
 *  FAUST - Framework for Adaptive Ubiquitous Scalable Tasks
 *  Website: https://macpro01.cct.lsu.edu/trac/faust
 *
 *  Created by Ole Weidner <oweidner@cct.lsu.edu> on 03/23/09.
 *  Copyright 2008-2009 Center for Computation & Technology. 
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying 
 *  LICENSE file or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef FAUST_AGENT_MONITOR_MONITOR_HPP
#define FAUST_AGENT_MONITOR_MONITOR_HPP

#include <saga/saga.hpp>
#include <faust/impl/logwriter.hpp>
#include <faust/faust/resource_description.hpp>
#include <faust/faust/resource_monitor.hpp>

#include <agent/monitor/monitor_group.hpp>

namespace faust { namespace agent { namespace monitor {
  
  //////////////////////////////////////////////////////////////////////////////
  //
  class monitor   
    {
    private:
      
      time_t last_update_;
      unsigned int update_interval_;
      
      faust::resource_description description_;
      faust::resource_monitor monitor_;
      
      boost::shared_ptr <faust::detail::logwriter> log_sptr_;
      
      std::vector<monitor_group> mgv_;
      
      // the main eventloop thread & entry-point fucntion
      boost::thread service_thread_;
      static void thread_entry_point_(monitor * mg);
      
      void init();
      
    public:
      
      monitor (unsigned int update_interval,
               faust::resource_description desc,
               faust::resource_monitor mon,
               boost::shared_ptr <faust::detail::logwriter> log_sptr);
      
      unsigned int get_update_interval();
      
      void run();
      
      ~monitor();
      
    };
  //
  //////////////////////////////////////////////////////////////////////////////
  
}}}

#endif /* FAUST_AGENT_MONITOR_MONITOR_HPP */

