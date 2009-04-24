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
using namespace faust::agent;

//////////////////////////////////////////////////////////////////////////
//
namespace {
  
  inline void tokenize(const std::string& str,
                       std::vector<std::string>& tokens,
                       const std::string& delimiters = " ")
  {
    // Skip delimiters at beginning.
    std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter".
    std::string::size_type pos     = str.find_first_of(delimiters, lastPos);
    
    while (std::string::npos != pos || std::string::npos != lastPos)
    {
      // Found a token, add it to the vector.
      tokens.push_back(str.substr(lastPos, pos - lastPos));
      // Skip delimiters.  Note the "not_of"
      lastPos = str.find_first_not_of(delimiters, pos);
      // Find next "non-delimiter"
      pos = str.find_first_of(delimiters, lastPos);
    }
  }
}

//////////////////////////////////////////////////////////////////////////
//
app::app(std::string endpoint, std::string uuid)
: endpoint_(endpoint), uuid_(uuid)
{

  
  // Initialize the logwriter
  std::string identifier("faust_agent ("+uuid_+")"); std::string msg("");
  log_ = new detail::logwriter(identifier, std::cout);
 
  m_ = system_monitor("", description_, monitor_, uuid_, log_);
  
  msg = "Connecting to advert endpoint " + endpoint_;
  try {
    int mode = advert::ReadWrite;
    advert_base_ = advert::directory(endpoint_, mode);
        
    cmd_  = advert_base_.open("CMD", saga::advert::ReadWrite);
    args_ = advert_base_.open("ARGS", saga::advert::ReadWrite);
    rd_   = advert_base_.open("RD", saga::advert::ReadWrite);
    rm_   = advert_base_.open("RM", saga::advert::ReadWrite);
    
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
    std::vector<std::string> attr_ = rd_.list_attributes();
    std::vector<std::string>::const_iterator it;
    for(it = attr_.begin(); it != attr_.end(); ++it)
    {
      // exclude these advert-specific attributes
      if((*it) == "utime" || (*it) == "ctime" || (*it) == "persistent")
        continue;
      
      if(rd_.attribute_is_vector(*it)) {
        std::cout << "VA: " << (*it) << std::endl;
        description_.set_vector_attribute((*it), rd_.get_vector_attribute((*it)));
      }
      else {
        description_.set_attribute((*it), rd_.get_attribute((*it)));
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
app::~app()
{
  std::string msg("Disconnecting from advert endpoint " + endpoint_);
  try {    
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
void app::query()
{
  while(1) {
  m_.query();
    sleep(1);
  }
}

//////////////////////////////////////////////////////////////////////////
//
std::string app::recv_command(std::string & cmd, std::string & args)
{
  std::string cmd_str("");
  std::string args_str("");
  std::string msg("Checking if a new command is waiting");
  try {
    cmd_str = cmd_.retrieve_string();
    args_str = args_.retrieve_string();
    
    if(cmd_str.length() >= 1) {
      std::vector<std::string> tokens;
      ::tokenize(cmd_str, tokens, ":");
      
      if(tokens.at(0) == "ACK") { 
        msg += ". NO";
        log_->write(msg, LOGLEVEL_INFO);
      }
      else {
        msg += ". YES: CMD='"+cmd_str+"' ARGS='"+args_str+"'";
        log_->write(msg, LOGLEVEL_INFO);
        
        if(tokens.at(0) != uuid_) {
          // IF UUID doesn't match, I'm definitely a ZOMBIE agent and 
          // I should really kill myself!
          msg = "UUID of received command "+cmd_str+" is INVALID. TERMINATING!";
          log_->write(msg, LOGLEVEL_ERROR);
          throw faust::exception (msg, faust::NoSuccess);
        }
        else {
          msg = "Sending acknowledgement for command '"+cmd_str+"'";
          try {
            cmd_.store_string("ACK:"+cmd_str);
            
            msg += ". SUCCESS ";
            log_->write(msg, LOGLEVEL_INFO);
          }
          catch(saga::exception const & e) {
            msg += " FAILED " + std::string(e.what());
            log_->write(msg, LOGLEVEL_ERROR);
            throw faust::exception (msg, faust::NoSuccess);
          }  
        }
      }
    }
    else {
      msg += ". NO";
      log_->write(msg, LOGLEVEL_INFO);
    }
  }
  catch(saga::exception const & e) {
    msg += ". FAILED " + std::string(e.what());
    log_->write(msg, LOGLEVEL_ERROR);
    throw faust::exception (msg, faust::NoSuccess);
  }  
  
  cmd = cmd_str;
  args = args_str;
  
  return cmd_str;
}

//////////////////////////////////////////////////////////////////////////
//
void app::run(void)
{
  std::string a("");
  std::string b("");
  while(1) {
    std::string cmd = recv_command(a, b);
    if(cmd == uuid_+":TERMINATE") return;
    else if(cmd == uuid_+":UPDATE") { query(); }
    
    sleep(1);
  }
}

//////////////////////////////////////////////////////////////////////////
//
void app::run_tests(void)
{
}

