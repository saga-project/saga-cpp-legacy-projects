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

#include <faust/faust/exception.hpp>
#include <faust/faust/resource_description.hpp>
#include <faust/impl/resource_impl.hpp>

using namespace saga;
using namespace faust::impl;
namespace FAR = faust::attributes::resource_description;

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR
resource::resource(std::string resource_id, bool persistent)
: object(faust::object::Resource), init_from_id_(true), 
resource_id_(resource_id), persistent_(persistent)
{
  // Initialize the logwriter
  std::string identifier(FW_NAME); std::string msg("");
  identifier.append(" faust::resource ("+get_uuid()+")"); 
  log_ = new detail::logwriter(identifier, std::cout);
  
  // TRY TO CONNECT TO THE ADVERT ENTRY FOR THIS RESOURCE
  //
  std::string advert_key = object::faust_root_namesapce_ 
  + "RESOURCES/" + resource_id_ + "/";
  
  msg = "Re-connecting to advert endpoint " + advert_key;
  try {
    int mode = advert::ReadWrite;
    advert_base_ = advert::directory(advert_key, mode);
    
    
    // SET THE PERSISTENT BIT FOR THIS RESOURCE ENTRY
    if( true == persistent_ )
      advert_base_.set_attribute("persistent", "TRUE");
    else
      advert_base_.set_attribute("persistent", "FALSE");
    
    // open "CMD" entry
    cmd_ = advert_base_.open("CMD", saga::advert::ReadWrite);
    
    // open "STATUS" entry
    status_ = advert_base_.open("STATUS", saga::advert::ReadWrite);
    
    msg += ". SUCCESS ";
    log_->write(msg, LOGLEVEL_INFO);
  }
  catch(saga::exception const & e) {
    msg += ". FAILED " + std::string(e.what());
    log_->write(msg, LOGLEVEL_ERROR);
    throw faust::exception (msg, faust::NoSuccess);
  }
  
  msg = "Checking if faust agent instance is still alive:";
  try {
    std::string status = status_.retrieve_string();
    if(status != "CONNECTED") {
      msg += " NO. Restart triggered.";
      log_->write(msg, LOGLEVEL_INFO);
      launch_agent();
      wait_for_agent_connect();
    }
    else {
      msg += " YES.";
      log_->write(msg, LOGLEVEL_INFO);
    }    
  }
  catch(saga::exception const & e) {
    msg += ". FAILED " + std::string(e.what());
    log_->write(msg, LOGLEVEL_ERROR);
    throw faust::exception (msg, faust::NoSuccess);
  }
  
  
  // RETRIEVE ATTRIBUTES OF THE ADVERT ENTRY AND GENERATE RESOURCE_DESCRIPTION
  //
  msg = "Retrieving resource description for " + resource_id_;
  
  try {
    //description_ = faust::resource_description();
    
    std::vector<std::string> attr_ = advert_base_.list_attributes();
    std::vector<std::string>::const_iterator it;
    for(it = attr_.begin(); it != attr_.end(); ++it)
    {
      // exclude these advert-specific attributes
      if((*it) == "utime" || (*it) == "ctime" || (*it) == "persistent")
        continue;
      
      if(advert_base_.attribute_is_vector(*it)) {
        description_.set_vector_attribute((*it), advert_base_.get_vector_attribute((*it)));
        //std::cout << "vector " << *it << std::endl;
      }
      else {
        description_.set_attribute((*it), advert_base_.get_attribute((*it)));
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
  
}

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR
resource::resource(faust::resource_description resource_desc, bool persistent) 
: object(faust::object::Resource), 
description_(resource_desc), init_from_id_(false), persistent_(persistent)
{
  // Initialize the logwriter
  std::string identifier(FW_NAME); std::string msg("");
  identifier.append(" faust::resource ("+get_uuid()+")"); 
  log_ = new detail::logwriter(identifier, std::cout);
  
  // CHECK IF ALL REQUIRED ATTRIBUTES ARE AVAILABLE
  //
  msg = ("Checking faust::resource_description for completeness. ");
	
	if(!description_.attribute_exists(FAR::identifier)) {
    msg += "FAILED Missing required attribute 'identifier'.";
    log_->write(msg, LOGLEVEL_ERROR);
    throw faust::exception (msg, faust::BadParameter);
	}
  else if(!description_.attribute_exists(FAR::faust_agent_submit_url)) {
    msg += "FAILED. Missing required attribute 'faust_agent_submit_url'.";
    log_->write(msg, LOGLEVEL_ERROR);
    throw faust::exception (msg, faust::BadParameter);
  }
  else if(!description_.attribute_exists(FAR::faust_agent_binary_path)) {
    msg += "FAILED Missing required attribute 'faust_agent_binary_path'.";
    log_->write(msg, LOGLEVEL_ERROR);
    throw faust::exception (msg, faust::BadParameter);
  }
	else if(!description_.attribute_exists(FAR::saga_root_path)) {
    msg += "FAILED Missing required attribute 'saga_root_path'.";
    log_->write(msg, LOGLEVEL_ERROR);
    throw faust::exception (msg, faust::BadParameter);
  }

  else {
    msg += "SUCCESS";
    log_->write(msg, LOGLEVEL_INFO);
  }
  
  // TRY TO CREATE ADVERT ENTRY FOR THIS RESOURCE
  //
  msg = "Creating advert endpoint ";
  try {
    int mode = advert::ReadWrite | advert::Create | advert::Recursive;
    resource_id_ = description_.get_attribute(FAR::identifier);
    
    std::string advert_key = object::faust_root_namesapce_;
    msg += "'"+advert_key + "RESOURCES/" + resource_id_ + "/'";
    
    advert_base_ = advert::directory(advert_key, advert::ReadWrite);
    
    // don't overwrite an existing entry if it has the persistency flag set!
    if(advert_base_.exists("RESOURCES/" + resource_id_ + "/")) {
      saga::advert::directory tmp = advert_base_.open_dir("RESOURCES/" + resource_id_ + "/");
      
      if(tmp.attribute_exists("persistent")) {
        std::string dbg(tmp.get_attribute("persistent"));
        if(tmp.get_attribute("persistent") == "TRUE") {
          msg += ". FAILED: endpoint '"+resource_id_+"' already exists and set to 'persistent'!";
          log_->write(msg, LOGLEVEL_ERROR);
          throw faust::exception (msg, faust::NoSuccess);
        }
      }
    }
    else {
      advert_key += "RESOURCES/";
      advert_base_ = advert::directory(advert_key, mode);
      advert_key += resource_id_ + "/";
      advert_base_ = advert::directory(advert_key, mode);
    }
    
    msg += ". SUCCESS ";
    log_->write(msg, LOGLEVEL_INFO);
  }
  catch(saga::exception const & e) {
    msg += ". FAILED: " + std::string(e.what());
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
      }
      else {
        advert_base_.set_attribute((*it), description_.get_attribute((*it)));
      }
    }
    
    // SET THE PERSISTENT BIT FOR THIS RESOURCE ENTRY
    if( true == persistent_ )
      advert_base_.set_attribute("persistent", "TRUE");
    else
      advert_base_.set_attribute("persistent", "FALSE");
    
    // create "CMD" entry
    cmd_ = advert_base_.open("CMD", saga::advert::ReadWrite | saga::advert::Create);
    cmd_.store_string("");
    
    // create "STATUS" entry
    status_ = advert_base_.open("STATUS", saga::advert::ReadWrite | saga::advert::Create);
    status_.store_string("");
    
    msg += ". SUCCESS ";
    log_->write(msg, LOGLEVEL_INFO);
  }
  catch(saga::exception const & e) {
    msg += ". FAILED " + std::string(e.what());
    log_->write(msg, LOGLEVEL_ERROR);
    throw faust::exception (msg, faust::NoSuccess);
  }
  
  launch_agent();
  
  wait_for_agent_connect();
  
}

////////////////////////////////////////////////////////////////////////////////
//
void resource::launch_agent(unsigned int timeout)
{
	namespace SJR = saga::job::attributes;
	
	std::vector<std::string> env;
	env.push_back("SAGA_LOCATION="+
								description_.get_attribute(FAR::saga_root_path));
	env.push_back("LD_LIBRARY_PATH="+
								description_.get_attribute(FAR::saga_root_path) +"/lib"); // Linux
	env.push_back("DYLD_LIBRARY_PATH="+
								description_.get_attribute(FAR::saga_root_path)+"/lib");  // MacOS 
	
	std::vector<std::string> args;
	args.push_back("--endpoint="+object::faust_root_namesapce_ 
								+"RESOURCES/"+resource_id_+"/");
	
	std::string msg = "Trying to launch agent via " + 
										description_.get_attribute(FAR::faust_agent_submit_url);
	
	try {
		saga::job::description jd;
		jd.set_vector_attribute(SJR::description_environment, env);
		jd.set_vector_attribute(SJR::description_arguments, args);
		jd.set_attribute(SJR::description_executable, 
										 description_.get_attribute(FAR::faust_agent_binary_path));
		
		saga::job::service js(description_.get_attribute(FAR::faust_agent_submit_url));
		saga::job::job j = js.create_job(jd);
		j.run();
		
		msg += ". SUCCESS ";
		log_->write(msg, LOGLEVEL_INFO);
	}
	catch(saga::exception const & e) {
		msg += ". FAILED " + std::string(e.what());
		log_->write(msg, LOGLEVEL_ERROR);
		throw faust::exception (msg, faust::NoSuccess);
	}
	
}

////////////////////////////////////////////////////////////////////////////////
//
void resource::wait_for_agent_connect(unsigned int timeout) 
{
  std::string msg = "Waiting for faust_agent instance to connect (timeout 30s)";
  try {
    int to = 0;
    std::string status("");
    while(status != "CONNECTED") {
      if(to > 30) {
        msg += ". FAILED (Timeout)";
        log_->write(msg, LOGLEVEL_ERROR);
        throw faust::exception (msg, faust::Timeout);
      }        
      
      status = status_.retrieve_string();
      sleep(1); ++to;
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

////////////////////////////////////////////////////////////////////////////////
//
resource::~resource() 
{
  if(false == persistent_) {
    // TODO: SEND "TERMINATE" COMMAND TO AGENT 
    std::string msg = "Removing advert endpoint ";
    msg += object::faust_root_namesapce_ + "RESOURCES/" + resource_id_ + "/";
    try {
      advert_base_.remove(saga::advert::Recursive);
      msg += ". SUCCESS ";
      log_->write(msg, LOGLEVEL_INFO);
    }
    catch(saga::exception const & e) {
      msg += ". FAILED " + std::string(e.what());
      log_->write(msg, LOGLEVEL_ERROR);
      throw faust::exception (msg, faust::NoSuccess);
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
//
bool resource::send_command(std::string cmd, unsigned int timeout)
{
  // sends a command and waits for an acknowledgement. 
  std::string msg("Writing command to endpoint ");
  msg += object::faust_root_namesapce_ + "RESOURCES/" + resource_id_ + "/";
  try {
    cmd_.store_string(cmd);
    msg += ". SUCCESS ";
  }
  catch(saga::exception const & e) {
    msg += ". FAILED " + std::string(e.what());
    log_->write(msg, LOGLEVEL_ERROR);
    throw faust::exception (msg, faust::NoSuccess);
  }
  
  msg += "Waiting for ACK... ";
  try {
    int to = 0; std::string result;
    while(to < timeout) {
      sleep(1); ++to;
      result = cmd_.retrieve_string();
      if(result == std::string("ACK "+cmd))
        break;
    }
    
    if(result == std::string("ACK "+cmd)) {
      msg += "SUCCESS ";
      log_->write(msg, LOGLEVEL_INFO);
      return true;
    }
    else {
      msg += " FAILED (Timeout) " ;
      log_->write(msg, LOGLEVEL_ERROR);
      return false;
    }
  }
  catch(saga::exception const & e) {
    msg += " FAILED " + std::string(e.what());
    log_->write(msg, LOGLEVEL_ERROR);
    throw faust::exception (msg, faust::NoSuccess);
  }  
  
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

////////////////////////////////////////////////////////////////////////////////
//
bool resource::is_persistent(void)
{
  return persistent_;
}

////////////////////////////////////////////////////////////////////////////////
//
void resource::set_persistent(bool yesno)
{
  std::string msg = "Setting persistentcy for endpoint '"+resource_id_+"' to ";
  if(true == yesno) msg += "true"; else msg += "false";
  
  try {
    // SET THE PERSISTENT BIT FOR THIS RESOURCE ENTRY
    if( true == yesno )
      advert_base_.set_attribute("persistent", "TRUE");
    else
      advert_base_.set_attribute("persistent", "FALSE");
    
    msg += ". SUCCESS ";
    log_->write(msg, LOGLEVEL_INFO);
    persistent_ = yesno;
  }
  catch(saga::exception const & e) {
    msg += ". FAILED " + std::string(e.what());
    log_->write(msg, LOGLEVEL_ERROR);
    throw faust::exception (msg, faust::NoSuccess);
  }
}
