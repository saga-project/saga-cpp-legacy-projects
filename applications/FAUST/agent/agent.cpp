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

#include <faust/faust/exception.hpp>

#include "agent.hpp"

using namespace saga;
using namespace faust;

agent::agent(std::string endpoint)
: endpoint_(endpoint)
{
  // create unique identifier
  uuid_ = saga::uuid().string();
  
  // Initialize the logwriter
  std::string identifier("faust_agent ("+uuid_+")"); std::string msg("");
  log_ = new detail::logwriter(identifier, std::cout);
  
  msg = "Connecting to advert endpoint " + endpoint_;
  try {
    int mode = advert::ReadWrite;
    advert_base_ = advert::directory(endpoint_, mode);
    
    msg += ". SUCCESS ";
    log_->write(msg, LOGLEVEL_INFO);
  }
  catch(saga::exception const & e) {
    msg += ". FAILED " + std::string(e.what());
    log_->write(msg, LOGLEVEL_ERROR);
    throw faust::exception (msg, faust::NoSuccess);
  }

}

void agent::run(void)
{

}