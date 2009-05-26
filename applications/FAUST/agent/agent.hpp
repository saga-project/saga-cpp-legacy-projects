/*
 *  agent.hpp 
 *  FAUST - Framework for Adaptive Ubiquitous Scalable Tasks
 *  Website: https://macpro01.cct.lsu.edu/trac/faust
 *
 *  Created by Ole Weidner <oweidner@cct.lsu.edu> on 03/11/09.
 *  Copyright 2008-2009 Center for Computation & Technology. 
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying 
 *  LICENSE file or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef FAUST_AGENT_HPP
#define FAUST_AGENT_HPP

#include <saga/saga.hpp>
#include <faust/impl/logwriter.hpp>
#include <agent/monitor/monitor.hpp>

#include <faust/faust/resource_description.hpp>
#include <faust/faust/resource_monitor.hpp>

namespace faust
{
  namespace agent
  {
    class app   
      {
      private:
        
        std::string endpoint_;
        std::string uuid_;
                
        saga::advert::entry cmd_adv_;
        saga::advert::entry args_adv_;
        saga::advert::entry desc_adv_;
        saga::advert::entry mon_adv_;
        
        boost::shared_ptr <faust::detail::logwriter> log_sptr_;
        boost::shared_ptr <faust::agent::monitor::monitor> monitor_sptr_;
        
        faust::resource_description description_;
        faust::resource_monitor     monitor_;
        
        std::string recv_command(std::string & cmd, std::string & args);
        
      public:
        app(std::string endpoint, std::string uuid);
        ~app();
        
        void run(void);
        void run_tests(void);
        
      };
  }
}

#endif /* FAUST_AGENT_HPP */

