/*
 *  resource_monitor_impl.cpp 
 *  FAUST - Framework for Adaptive Ubiquitous Scalable Tasks
 *  Website: https://macpro01.cct.lsu.edu/trac/faust
 *
 *  Created by Ole Weidner <oweidner@cct.lsu.edu> on 02/13/09.
 *  Copyright 2008-2009 Center for Computation & Technology. 
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying 
 *  LICENSE file or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include <faust/impl/resource_monitor_impl.hpp>

using namespace faust::impl;

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR
resource_monitor::resource_monitor()
: object(faust::object::ResourceMonitor)
{
  // Initialize the logwriter
  std::string identifier(FW_NAME); std::string msg("");
  identifier.append(" faust::resource_monitor ("+uuid_+")"); 
  log_ = new detail::logwriter(identifier, std::cout);
  
  msg = "Trying to connect to advert endpoint: (timeout: XXX)";
  try {
    // DO ADVERT STUFF (use uuid_ as key)
    // IF OK log_->write(msg, LOGLEVEL_INFO);
  }
  catch(saga::exception const & e) {
    // IF FAILED log_->write(msg, LOGLEVEL_ERROR); and THROW
  }
  
  msg = "Check if agent is still alive (timeout: XXX)";
  try {
    // DO ADVERT STUFF (use uuid_ as key)
    // IF OK log_->write(msg, LOGLEVEL_INFO);
  }
  catch(saga::exception const & e) {
    // IF FAILED log_->write(msg, LOGLEVEL_ERROR); and THROW
  }  
  
  // CACHING SHOULD HAPPEN ON A PER-ATTRIBUTE BASE. IF AN ATTRIBUTE IS 
  // REQUESTED DO:
  //   * CHECK IF EXISTS IN CACHE. IF NOT - RETRIEVE FROM DB
  //                               IF - CHECK TIMESTAMP RETRIEVE FROM DB
  //                                    ONLY IF POTENTIALLY NEW! 
  
}
