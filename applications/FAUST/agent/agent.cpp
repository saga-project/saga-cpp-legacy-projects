/*
 *  agent.cpp 
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

#include <agent/agent.hpp>

using namespace saga;
using namespace faust;

agent::agent(std::string endpoint)
: endpoint_(endpoint)
{
  faust::resource_description rd;
  
  // create unique identifier
  uuid_ = saga::uuid().string();
  
  // Initialize the logwriter
  std::string identifier("faust_agent ("+uuid_+")"); std::string msg("");
  log_ = new detail::logwriter(identifier, std::cout);
  
  msg = "Connecting to advert endpoint " + endpoint_;
  try {
    int mode = advert::ReadWrite;
    advert_base_ = advert::directory(endpoint_, mode);
    
    // write status bit to announce availablility 
    status_ = advert_base_.open("STATUS", saga::advert::ReadWrite);
    status_.store_string("CONNECTED");
    
    msg += ". SUCCESS ";
    log_->write(msg, LOGLEVEL_INFO);
  }
  catch(saga::exception const & e) {
    msg += ". FAILED " + std::string(e.what());
    log_->write(msg, LOGLEVEL_ERROR);
    throw faust::exception (msg, faust::NoSuccess);
  }

  
  // RETRIEVING ATTRIBUTES FROM THE ADVERT SERVICE AND GENERATE RESOURCE_DESCRIPTION
  //
  msg = "Retrieving resource description";
  
  try {    
    std::vector<std::string> attr_ = advert_base_.list_attributes();
    std::vector<std::string>::const_iterator it;
    for(it = attr_.begin(); it != attr_.end(); ++it)
    {
      // exclude these advert-specific attributes
      if((*it) == "utime" || (*it) == "ctime" || (*it) == "persistent")
        continue;
      
      if(advert_base_.attribute_is_vector(*it)) {
        description_.set_vector_attribute((*it), advert_base_.get_vector_attribute((*it)));
      }
      else {
        description_.set_attribute((*it), advert_base_.get_attribute((*it)));
      }
    }
    msg += ". SUCCESS ";
    log_->write(msg, LOGLEVEL_INFO);
  }
  catch(saga::exception const & e) {
    msg += ". FAILED " + std::string(e.what());
    log_->write(msg, LOGLEVEL_ERROR);
    throw faust::exception (msg, faust::NoSuccess);
  }
  
}

agent::~agent()
{
  std::string msg("Disconnecting from advert endpoint " + endpoint_);
  try {    
    status_.store_string("DISCONNECTED");
    status_.close();
    advert_base_.close();
    
    
    msg += ". SUCCESS ";
    log_->write(msg, LOGLEVEL_INFO);
  }
  catch(saga::exception const & e) {
    msg += ". FAILED " + std::string(e.what());
    log_->write(msg, LOGLEVEL_ERROR);
    throw faust::exception (msg, faust::NoSuccess);
  }
  
}

bool agent::recv_command()
{
  
}

void agent::run(void)
{
  sleep(60);
}
