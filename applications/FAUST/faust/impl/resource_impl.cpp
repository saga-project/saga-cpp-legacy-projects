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

#include <faust/faust/protocol.hpp>
#include <faust/faust/exception.hpp>
#include <faust/faust/resource_description.hpp>
#include <faust/impl/resource_impl.hpp>

using namespace saga;
using namespace faust::impl;
namespace FAR = faust::attributes::resource_description;

////////////////////////////////////////////////////////////////////////////////
//
//
resource::resource(std::string resource_id)
: object(faust::object::Resource), init_from_id_(true), 
resource_id_(resource_id)
{
	std::string endpoint_str_(object::faust_root_namesapce_ + 
														"RESOURCES/" + resource_id_ + "/");
	
  // Initialize the logwriter
  std::string identifier(FW_NAME); std::string msg("");
  identifier.append(" faust::resource ("+resource_id_+")"); 
  log_ = new detail::logwriter(identifier, std::cout);
  
  // TRY TO CONNECT TO THE ADVERT ENTRY FOR THIS RESOURCE
  //
  std::string advert_key(endpoint_str_);
  
  msg = "Re-connecting to advert endpoint " + advert_key;
  try {
    int mode = advert::ReadWrite;
    advert_base_ = advert::directory(advert_key, mode);
    
    // open "CMD" entry
    cmd_  = advert_base_.open(endpoint_str_+"CMD", saga::advert::ReadWrite);
    args_ = advert_base_.open(endpoint_str_+"ARGS", saga::advert::ReadWrite);
    rd_   = advert_base_.open(endpoint_str_+"RD", saga::advert::ReadWrite);
    rm_   = advert_base_.open(endpoint_str_+"RM", saga::advert::ReadWrite);
    
    // pass the resource monitor advert to the monitor
    monitor_ = faust::resource_monitor(rm_);
        
    msg += ". SUCCESS ";
    log_->write(msg, LOGLEVEL_INFO);
  }
  catch(saga::exception const & e) {
    msg += ". FAILED " + std::string(e.what());
    log_->write(msg, LOGLEVEL_ERROR);
    throw faust::exception (msg, faust::NoSuccess);
  }
  
  try {
		saga::advert::entry auuid(endpoint_str_+"AGENT_UUID", saga::advert::Read);
    agent_uuid_ = auuid.retrieve_string();  
		if(agent_uuid_.length() == 0)
		{
			// something is wrong with the entry. Generating new UUID
			agent_uuid_ = saga::uuid().string();
			msg = "Something seems to be wrong with the 'AGENT_UUID' entry. Generating new one: "+agent_uuid_+".";
			log_->write(msg, LOGLEVEL_WARNING);
		}
		send_command(PROTO_V1_PING, 60);
		
  }
	catch(faust::exception const & e) {
		if(e.get_error() == faust::Timeout) {
			msg = "faust_agent instance'"+agent_uuid_+"'seems to be dead. RESTARTING ";
			log_->write(msg, LOGLEVEL_INFO);
			launch_agent();
			send_command(PROTO_V1_PING, 60);
		}
		else {
			throw;
		}
	}
  catch(saga::exception const & e) {
    msg += "Checking if faust_agent instance is still alive. FAILED " + std::string(e.what());
    log_->write(msg, LOGLEVEL_ERROR);
    throw faust::exception (msg, faust::NoSuccess);
  }
  
  // RETRIEVE ATTRIBUTES OF THE ADVERT ENTRY AND GENERATE RESOURCE_DESCRIPTION
  //
  msg = "Retrieving resource description for " + resource_id_;
  
  try {    
    std::vector<std::string> attr_ = rd_.list_attributes();
    std::vector<std::string>::const_iterator it;
    for(it = attr_.begin(); it != attr_.end(); ++it)
    {
      // exclude these advert-specific attributes
      if((*it) == "utime" || (*it) == "ctime" || (*it) == "persistent")
        continue;
            
      if(rd_.attribute_is_vector(*it)) {
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
  
  //// STARTING THE SERVICE THREAD
  //
  msg = "Starting service thread";
  try {
    service_thread_ = boost::thread(&main_event_loop);
  }
  catch(...)
  {
    msg += ". FAILED ";
    log_->write(msg, LOGLEVEL_ERROR);
    throw faust::exception (msg, faust::NoSuccess);
  }
  msg += ". SUCCESS ";
  log_->write(msg, LOGLEVEL_INFO);
  //
  ////
}

////////////////////////////////////////////////////////////////////////////////
// 
//
resource::resource(faust::resource_description resource_desc, bool persistent) 
: object(faust::object::Resource), description_(resource_desc), 
init_from_id_(false), persistent_(persistent)
{
  agent_uuid_ = saga::uuid().string();
  
  //// INITIALIZE THE LOGWRITER
  //
  if(description_.attribute_exists(FAR::identifier))
		resource_id_ = description_.get_attribute(FAR::identifier);
  else resource_id_ = "";
  std::string identifier(FW_NAME); std::string msg("");
  identifier.append(" faust::resource ("+resource_id_+")"); 
  log_ = new detail::logwriter(identifier, std::cout);
  
  //// CHECK IF ALL REQUIRED ATTRIBUTES ARE AVAILABLE
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
  
  resource_id_ = description_.get_attribute(FAR::identifier);
  std::string endpoint_str_(object::faust_root_namesapce_ + 
														"RESOURCES/" + resource_id_ + "/");
  
  int mode = advert::ReadWrite | advert::Create;
	
	// TRY TO CREATE ADVERT ENTRY FOR THIS RESOURCE
  //
  msg = "Creating advert endpoint '"+endpoint_str_+"'";
  try {
    
    std::string advert_key = object::faust_root_namesapce_;
    
    advert_base_ = advert::directory(advert_key, mode);
    
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
  // SET THE PERSISTENT BIT FOR THIS RESOURCE ENTRY
  if( true == persistent_ )
    advert_base_.set_attribute("persistent", "TRUE");
  else
    advert_base_.set_attribute("persistent", "FALSE");
  
  // create "CMD" entry
  cmd_ = advert_base_.open(endpoint_str_+"CMD", mode);
  cmd_.store_string(""); 
  
  args_ = advert_base_.open(endpoint_str_+"ARGS", mode);
  args_.store_string(""); 

  rd_ = advert_base_.open(endpoint_str_+"RD", mode);
  rd_.store_string(""); 

  rm_ = advert_base_.open(endpoint_str_+"RM", mode);
  rm_.store_string(""); 
  
  // create "AGENT_UUID" entry
  saga::advert::entry auuid(endpoint_str_+"AGENT_UUID", mode);
  auuid.store_string(agent_uuid_);  
  
  msg = "Populating advert endpoint '"+endpoint_str_+"'";
  try {
    std::vector<std::string> attr_ = description_.list_attributes();
    std::vector<std::string>::const_iterator it;
    for(it = attr_.begin(); it != attr_.end(); ++it)
    {
      if(description_.attribute_is_vector(*it)) {
        rd_.set_vector_attribute((*it), description_.get_vector_attribute((*it)));
      }
      else {
        rd_.set_attribute((*it), description_.get_attribute((*it)));
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
	
	launch_agent();
  send_command(PROTO_V1_PING, 120);
  
  //// STARTING THE SERVICE THREAD
  //
  msg = "Starting service thread";
  try {
    service_thread_ = boost::thread(&main_event_loop);
  }
  catch(...)
  {
    msg += ". FAILED ";
    log_->write(msg, LOGLEVEL_ERROR);
    throw faust::exception (msg, faust::NoSuccess);
  }
  msg += ". SUCCESS ";
  log_->write(msg, LOGLEVEL_INFO);
  //
  ////	
}

////////////////////////////////////////////////////////////////////////////////
//
//
void resource::launch_agent(unsigned int timeout)
{
	namespace SJR = saga::job::attributes;
	
  
  std::vector<std::string> env;
  
  if(description_.attribute_exists(FAR::environment))
    env = description_.get_vector_attribute(FAR::environment);

	env.push_back("SAGA_LOCATION="+
								description_.get_attribute(FAR::saga_root_path));
	//env.push_back("LD_LIBRARY_PATH="+
	//							description_.get_attribute(FAR::saga_root_path) +"/lib"); // Linux
	env.push_back("DYLD_LIBRARY_PATH="+
								description_.get_attribute(FAR::saga_root_path)+"/lib");  // MacOS 
	
	std::vector<std::string> args;
	args.push_back("--endpoint="+object::faust_root_namesapce_ 
                 +"RESOURCES/"+resource_id_+"/");
	args.push_back("--identifier="+agent_uuid_);
  
	std::string msg("Trying to launch agent "+agent_uuid_+" via " + 
									description_.get_attribute(FAR::faust_agent_submit_url));
	
	try {
		saga::job::description jd;
		jd.set_vector_attribute(SJR::description_environment, env);
		jd.set_vector_attribute(SJR::description_arguments, args);
		jd.set_attribute(SJR::description_executable, 
										 description_.get_attribute(FAR::faust_agent_binary_path));
    jd.set_attribute(SJR::description_error,  "/tmp/faust_agent-"+agent_uuid_+".err"); 
    jd.set_attribute(SJR::description_output, "/tmp/faust_agent-"+agent_uuid_+".out"); 
		
		saga::job::service js(description_.get_attribute(FAR::faust_agent_submit_url));
		saga::job::job j = js.create_job(jd);
    
		j.run();
    saga::job::state state = j.get_state ();
    
    if ( state != saga::job::Running && state != saga::job::Done    )
    {
      msg += ". FAILED ";
      log_->write(msg, LOGLEVEL_INFO);
    }
    else
    {
      msg += ". SUCCESS ";
      log_->write(msg, LOGLEVEL_INFO);
    }
    
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
    
    send_command(PROTO_V1_TERMINATE);
    
    std::string msg("Removing advert endpoint '"+endpoint_str_+"'");
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
  
  std::string msg = "Shutting down service thread";
  try {
    service_thread_.join();
  }
  catch(...)
  {
    msg += ". FAILED ";
    log_->write(msg, LOGLEVEL_ERROR);
  }
  msg += ". SUCCESS ";
  log_->write(msg, LOGLEVEL_INFO);
  
}

////////////////////////////////////////////////////////////////////////////////
//
void resource::send_command(std::string cmd, unsigned int timeout)
{
  // sends a command and waits for an acknowledgement. 
	std::string msg("Sending command '"+cmd+"' to faust_agent instance");
  try {
    cmd_.store_string(agent_uuid_+":"+cmd);
    msg += ". SUCCESS ";
  }
  catch(saga::exception const & e) {
    msg += ". FAILED " + std::string(e.what());
    log_->write(msg, LOGLEVEL_ERROR);
    throw faust::exception (msg, faust::NoSuccess);
  }
  
  msg += "Waiting for acknowledgement ";
  try {
    int to = 0; std::string result;
    while(to < timeout) {
      ++to;
      sleep(1);
      result = cmd_.retrieve_string();
      if(result == std::string("ACK:"+agent_uuid_+":"+cmd))
        break;
    }
    
    if(result == std::string("ACK:"+agent_uuid_+":"+cmd)) {
      cmd_.store_string(""); // Reset CMD
      args_.store_string(""); // Reset ARGS
      msg += "SUCCESS ";
      log_->write(msg, LOGLEVEL_INFO);
    }
    else {
      cmd_.store_string(""); // Reset CMD
      args_.store_string(""); // Reset ARGS
      std::stringstream out; out << timeout;
      msg += std::string(" FAILED (Timeout - "+out.str()+" sec) ") ;
      log_->write(msg, LOGLEVEL_ERROR);
      throw faust::exception (msg, faust::Timeout);
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
void resource::main_event_loop()
{
  // This is the entry point for the service thread. The service thread's job
  // is it to periodically poll the advert database and retreive informations
  // that are required by the scheduling framework.
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
  std::string msg = "Setting persistency for endpoint '"+resource_id_+"' to ";
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
