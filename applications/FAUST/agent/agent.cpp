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

#include <agent/agent.hpp>
#include <faust/faust/exception.hpp>

#include <faust/impl/resource_description_impl.hpp>
#include <faust/impl/resource_monitor_impl.hpp>

#include <agent/monitor/monitor_group.hpp>



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
  // Initialize logwrite instance 
  std::string identifier("faust_agent ("+uuid_+")"); std::string msg("");
  log_sptr_ = boost::shared_ptr <faust::detail::logwriter>
    (new detail::logwriter(identifier, std::cout));
  
  msg = "Connecting to advert endpoint " + endpoint_;
  try {
    int mode = advert::ReadWrite;
    saga::advert::directory base(endpoint_, mode);
        
    cmd_adv_  = base.open("CMD", mode);
    args_adv_ = base.open("ARGS", mode);
    desc_adv_ = base.open("RD", mode);
    mon_adv_  = base.open("RM", mode);
    
    //////////////////
    //
    description_.get_impl()->set_advert_entry(desc_adv_);
    description_.get_impl()->read_attributes();
    
    monitor_.get_impl()->set_advert_entry(mon_adv_);
    monitor_.get_impl()->read_attributes();
    //
    //////////////////
        
    monitor_sptr_ = boost::shared_ptr <faust::agent::monitor::monitor>
      (new faust::agent::monitor::monitor(1, description_, monitor_, mon_adv_, log_sptr_));
        
    msg += ". SUCCESS ";
    log_sptr_->write(msg, LOGLEVEL_INFO);
  }
  catch(saga::exception const & e) {
    msg += ". FAILED " + std::string(e.what());
    log_sptr_->write(msg, LOGLEVEL_ERROR);
    throw faust::exception (msg, faust::NoSuccess);
  }
  
}

//////////////////////////////////////////////////////////////////////////
//
app::~app()
{
  std::string msg("Disconnecting from advert endpoint " + endpoint_);
  try {    
    cmd_adv_.close();
    args_adv_.close();
    desc_adv_.close();
    mon_adv_.close();
    
    msg += ". SUCCESS ";
    log_sptr_->write(msg, LOGLEVEL_INFO);
  }
  catch(saga::exception const & e) {
    msg += ". FAILED " + std::string(e.what());
    log_sptr_->write(msg, LOGLEVEL_ERROR);
    throw faust::exception (msg, faust::NoSuccess);
  }
  
}


//////////////////////////////////////////////////////////////////////////
//
std::string app::recv_command(std::string & cmd, std::string & args)
{
  std::string cmd_adv_str("");
  std::string args_adv_str("");
  std::string msg("Checking if a new command is waiting");
  try {
    cmd_adv_str = cmd_adv_.retrieve_string();
    args_adv_str = args_adv_.retrieve_string();
    
    if(cmd_adv_str.length() >= 1) {
      std::vector<std::string> tokens;
      ::tokenize(cmd_adv_str, tokens, ":");
      
      if(tokens.at(0) == "ACK") { 
        msg += ". NO";
        log_sptr_->write(msg, LOGLEVEL_INFO);
      }
      else {
        msg += ". YES: CMD='"+cmd_adv_str+"' ARGS='"+args_adv_str+"'";
        log_sptr_->write(msg, LOGLEVEL_INFO);
        
        if(tokens.at(0) != uuid_) 
        {
          // IF UUID doesn't match, I'm definitely a ZOMBIE agent and 
          // I should really kill myself!
          msg = "UUID of received command "+cmd_adv_str+" is INVALID. TERMINATING!";
          log_sptr_->write(msg, LOGLEVEL_ERROR);
          throw faust::exception (msg, faust::NoSuccess);
        }
        else {
          msg = "Sending acknowledgement for command '"+cmd_adv_str+"'";
          try {
            cmd_adv_.store_string("ACK:"+cmd_adv_str);
            
            msg += ". SUCCESS ";
            log_sptr_->write(msg, LOGLEVEL_INFO);
          }
          catch(saga::exception const & e) {
            msg += " FAILED " + std::string(e.what());
            log_sptr_->write(msg, LOGLEVEL_ERROR);
            throw faust::exception (msg, faust::NoSuccess);
          }  
        }
      }
    }
    else {
      msg += ". NO";
      log_sptr_->write(msg, LOGLEVEL_INFO);
    }
  }
  catch(saga::exception const & e) {
    msg += ". FAILED " + std::string(e.what());
    log_sptr_->write(msg, LOGLEVEL_ERROR);
    throw faust::exception (msg, faust::NoSuccess);
  }  
  
  cmd = cmd_adv_str;
  args = args_adv_str;
  
  return cmd_adv_str;
}

//////////////////////////////////////////////////////////////////////////
//
void app::run(void)
{

  // start monitor thread
  monitor_sptr_->run();
  
  std::string a("");
  std::string b("");
  while(1) 
  {
    std::string cmd = recv_command(a, b);
    if(cmd == uuid_+":TERMINATE") return;
    sleep(1);
  }
}

//////////////////////////////////////////////////////////////////////////
//
void app::run_tests(void)
{
  //monitor::monitor m(1, description_, monitor_, mon_adv_, log_sptr_);
  //m.run();

  //sleep(100);
  
  /*faust::impl::detail::writeAttributesToDB<faust::resource_monitor>
  (monitor_, "faust::resource_description", mon_adv_, log_sptr_); 
  
  std::vector<std::string> mal = monitor_.list_attributes();
  std::vector<std::string>::const_iterator it;
  for(it = mal.begin(); it != mal.end(); ++it)
    std::cout << (*it) << std::endl;*/

  
  
}

