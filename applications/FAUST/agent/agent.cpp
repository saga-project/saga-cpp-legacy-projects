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

//////////////////////////////////////////////////////////////////////////
//
agent::agent(std::string endpoint, std::string uuid)
: endpoint_(endpoint), uuid_(uuid)
{
  faust::resource_description rd;
  
  // Initialize the logwriter
  std::string identifier("faust_agent ("+uuid_+")"); std::string msg("");
  log_ = new detail::logwriter(identifier, std::cout);
  
  msg = "Connecting to advert endpoint " + endpoint_;
  try {
    int mode = advert::ReadWrite;
    advert_base_ = advert::directory(endpoint_, mode);
    
    // write status bit to announce availablility 
    status_ = advert_base_.open("STATUS", saga::advert::ReadWrite);
    status_.store_string(uuid_+":CONNECTED");
    
    cmd_ = advert_base_.open("CMD", saga::advert::ReadWrite);
    
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

//////////////////////////////////////////////////////////////////////////
//
agent::~agent()
{
  std::string msg("Disconnecting from advert endpoint " + endpoint_);
  try {    
    status_.store_string(uuid_+":DISCONNECTED");
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

//////////////////////////////////////////////////////////////////////////
//
std::string agent::recv_command()
{
  std::string cmd_str("");
  std::string msg("Checking if a new command is waiting");
  try {
    cmd_str = cmd_.retrieve_string();
    if(cmd_str.length() >= 1)
      msg += ". YES: CMD='"+cmd_str+"'";
    else
      msg += ". NO";
    
    log_->write(msg, LOGLEVEL_INFO);
  }
  catch(saga::exception const & e) {
    msg += ". FAILED " + std::string(e.what());
    log_->write(msg, LOGLEVEL_ERROR);
    throw faust::exception (msg, faust::NoSuccess);
  }
  
  if(cmd_str.length() >= 1) 
  {
    msg += "Sending acknowledgement for command '"+cmd_str+"'";
    try {
      cmd_.store_string(uuid_+":ACK:"+cmd_str);
      
      msg += ". SUCCESS ";
      log_->write(msg, LOGLEVEL_INFO);
    }
    catch(saga::exception const & e) {
      msg += " FAILED " + std::string(e.what());
      log_->write(msg, LOGLEVEL_ERROR);
      throw faust::exception (msg, faust::NoSuccess);
    }  
  }
  return cmd_str;
}

//////////////////////////////////////////////////////////////////////////
//
void agent::run(void)
{
  while(1) {
    std::string cmd = recv_command();
    if(cmd == uuid_+":TERMINATE") exit(0);
    sleep(1);
  }
}
