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

#include <faust/faust/resource_description.hpp>
#include <faust/faust/resource_monitor.hpp>

////////////////////////////////////////////////////////////////////////////
//
namespace faust
{
  //////////////////////////////////////////////////////////////////////////
  //
  namespace agent { namespace monitor {
    
    enum query_type 
    {
      QueryAll          = 1,
      QueryDir          = 2,
      QueryQueues       = 3,
    };
    
    class monitor   
      {
      private:
        faust::detail::logwriter * log_;
        
        std::string sp_;
        boost::shared_ptr <faust::detail::logwriter> log_sptr_;
        boost::shared_ptr <faust::resource_description> desc_obj_sptr_;
        boost::shared_ptr <faust::resource_monitor> mon_obj_sptr_;
        
        /* directory attributes related stuff */
        bool directory_attributes_checked_;
        bool directory_attributes_ok_;
        bool check_directory_attributes();
        void query_directories();
        
        /* queue attributes related stuff */
        bool queue_attributes_checked_;
        bool queue_attributes_ok_;
        bool check_queue_attributes();
        void query_queues();
        
      public:
        //monitor() {}
        monitor (boost::shared_ptr <faust::resource_description> rd,
                 boost::shared_ptr <faust::resource_monitor>     rm,
                 boost::shared_ptr <faust::detail::logwriter>    lw);
        
        ~monitor() {}
        
        void query(query_type=QueryAll); // executes a query on all rd attributes
      };
    
  }}
  //
  //////////////////////////////////////////////////////////////////////////
}

#endif /* FAUST_AGENT_MONITOR_MONITOR_HPP */

