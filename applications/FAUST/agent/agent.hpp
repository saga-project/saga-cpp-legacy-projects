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
#include <agent/system_monitor.hpp>
#include <faust/faust/resource_description.hpp>
#include <faust/faust/resource_monitor.hpp>

namespace faust
{
  //////////////////////////////////////////////////////////////////////////
  //
  namespace agent
  {
  class app   
  {
  private:
    faust::detail::logwriter * log_;
    std::string endpoint_;
    std::string uuid_;
    
    saga::advert::directory advert_base_;
    saga::advert::entry cmd_;
    saga::advert::entry args_;
    
    system_monitor m_;
    faust::resource_description description_;
    faust::resource_monitor     monitor_;
    
    std::string recv_command(std::string & cmd, std::string & args);
    
    void query();
    
  public:
    app(std::string endpoint, std::string uuid);
    ~app();
    
    void run(void);
    void run_tests(void);
    
  };
  }
  //
  //////////////////////////////////////////////////////////////////////////
}

#endif /* FAUST_AGENT_HPP */

