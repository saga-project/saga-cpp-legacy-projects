/*
 *  resource_impl.cpp 
 *  FAUST - Framework for Adaptive Ubiquitous Scalable Tasks
 *  Website: https://macpro01.cct.lsu.edu/trac/faust
 *
 *  Created by Ole Weidner <oweidner@cct.lsu.edu> on 02/13/09.
 *  Copyright 2008-2009 Center for Computation & Technology. 
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying 
 *  LICENSE file or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include <faust/exception.hpp>
#include <faust/resource_description.hpp>
#include <faust/impl/resource_impl.hpp>

using namespace saga;
using namespace faust::impl;
namespace FAR = faust::attributes::resource_description;

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR
resource::resource(faust::resource_description RD) 
: object(faust::object::Resource)
{
  description_ = RD;  
  
  // Initialize the logwriter
  std::string identifier(FW_NAME); std::string msg("");
  identifier.append(" faust::resource ("+get_uuid()+")"); 
  log_ = new detail::logwriter(identifier, std::cout);
  
  // CHECK IF ALL REQUIRED ATTRIBUTES ARE AVAILABLE
  //
  msg = ("Checking faust::resource_description for completeness. ");
  if(!description_.attribute_exists(FAR::agent_submit_url)) {
    msg += "FAILED. Missing required attribute 'agent_submit_url'.";
    log_->write(msg, LOGLEVEL_ERROR);
    throw faust::exception (msg, faust::BadParameter);
  }
  else if(!description_.attribute_exists(FAR::identifier)) {
    msg += "FAILED Missing required attribute 'identifier'.";
    log_->write(msg, LOGLEVEL_ERROR);
    throw faust::exception (msg, faust::BadParameter);
  }
  else {
    msg += "OK";
    log_->write(msg, LOGLEVEL_INFO);
  }
  
  // TRY TO CREATE ADVERT ENTRY FOR THIS RESOURCE
  //
  msg = "Creating advert endpoint ";
  try {
    int mode = advert::ReadWrite | advert::Create | advert::Recursive;
    resource_id_ = description_.get_attribute(FAR::identifier);
    
    std::string advert_key = object::faust_root_namesapce_;
    msg += advert_key + "RESOURCES/" + resource_id_ + "/";
    
    advert_base_ = advert::directory(advert_key, mode);
    advert_key += "RESOURCES/";
    advert_base_ = advert::directory(advert_key, mode);
    advert_key += resource_id_ + "/";
    advert_base_ = advert::directory(advert_key, mode);
    
    msg += ". SUCCESS ";
    log_->write(msg, LOGLEVEL_INFO);
  }
  catch(saga::exception const & e) {
    msg += ". FAILED " + std::string(e.what());
    log_->write(msg, LOGLEVEL_ERROR);
    throw faust::exception (msg, faust::NoSuccess);
  }
  
  // COPY ALL ATTRIBUTES TO THE ADVERT ENTRY
  //
  msg = "Populating advert endpoint ";
  msg += object::faust_root_namesapce_ + "RESOURCES/" + resource_id_ + "/";
  try {
    std::vector<std::string> attr_ = description_.list_attributes();
    std::vector<std::string>::const_iterator it;
    for(it = attr_.begin(); it != attr_.end(); ++it)
    {
      if(description_.attribute_is_vector(*it)) {
        advert_base_.set_vector_attribute((*it), description_.get_vector_attribute((*it)));
        //std::cout << "vector " << *it << std::endl;
      }
      else {
        advert_base_.set_attribute((*it), description_.get_attribute((*it)));
        //std::cout << "scalar " << *it << std::endl;
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
  
  msg = "Launching FAUST agent on: ";
  try {
    // DO JOB LAUNCH STUFF
    // IF OK log_->write(msg, LOGLEVEL_INFO);
  }
  catch(saga::exception const & e) {
    // IF FAILED log_->write(msg, LOGLEVEL_ERROR); and THROW
  }
  
  msg = "Waiting for FAUST agent to connect to endpoint (timeout: XXX): ";
  // WAIT FOR AGENT TO WRITE PING TO ADVERT DB
  // IF OK log_->write(msg, LOGLEVEL_INFO);
  // IF FAILED log_->write(msg, LOGLEVEL_ERROR); and THROW
  
  
  // AT THIS POINT EVERYTHING SHOULD BE OK - WE HAVE A RUNNING AGENT
  // ON THE HOST DESCRIBED BY (RD) WHICH SUCCESSFULLY RECONNECTED TO
  // THE ADVERT DATABASE. IT SHOULD START TO ADVERTISE ITS COLLECTED
  // MONITORING INFO IMMEDIATELY 
  
  // NOTE: IMPLEMENT TIMESTAMP + CACHING FOR RESOURCE_MONITORS TO 
  //       AVOID UNNECCESSARY TRAFFIC
  
}

////////////////////////////////////////////////////////////////////////////////
//
resource::~resource() 
{
}

////////////////////////////////////////////////////////////////////////////////
//
faust::resource_description resource::get_description()
{
  return description_;
}

////////////////////////////////////////////////////////////////////////////////
//
faust::resource_monitor resource::get_monitor()
{
  return monitor_;
}
