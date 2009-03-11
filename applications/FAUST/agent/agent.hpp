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

namespace faust
{
  //////////////////////////////////////////////////////////////////////////
  //
  class agent   
  {
  private:
    faust::detail::logwriter * log_;
    std::string endpoint_;
    std::string uuid_;
    
    saga::advert::directory advert_base_;
    
  public:
    agent(std::string endpoint);
    void run(void);
      
  };
  //
  //////////////////////////////////////////////////////////////////////////
}

#endif /* FAUST_AGENT_HPP */