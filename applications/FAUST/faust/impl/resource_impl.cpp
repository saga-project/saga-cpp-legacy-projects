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

#include <faust/impl/detail/serialize.hpp>

#include <faust/faust/protocol.hpp>
#include <faust/faust/exception.hpp>
#include <faust/faust/resource_description.hpp>
#include <faust/impl/resource_impl.hpp>

#include <faust/impl/resource_monitor_impl.hpp>
#include <faust/impl/resource_description_impl.hpp>

using namespace saga;
using namespace faust::impl;
namespace FAR = faust::attributes::resource_description;


////////////////////////////////////////////////////////////////////////////////
//
void resource::init()
{
  // Initialize the logwriter
  std::string identifier(FW_NAME);
  identifier.append(" faust::resource ("+resource_id_+")"); 
  
  log_sptr_ = object::get_log();
  
  // Set the root namespace string
  std::string endpoint_str_(object::faust_root_namesapce_ + 
                            "RESOURCES/" + resource_id_ + "/");
}


////////////////////////////////////////////////////////////////////////////////
//
resource::resource(std::string resource_id)
: object(faust::object::Resource), init_from_id_(true), 
resource_id_(resource_id)
{
  init();
  SAGA_OSSTREAM msg;
  
  msg << "Re-connecting to advert endpoint. " << endpoint_str_;
  
  try 
  {
    int mode = advert::ReadWrite;
    advert_base_ = advert::directory(endpoint_str_, mode);
    
    cmd_adv_  = advert_base_.open(endpoint_str_+"CMD",  mode);
    args_adv_ = advert_base_.open(endpoint_str_+"ARGS", mode);
    desc_adv_ = advert_base_.open(endpoint_str_+"RD",   mode);
    mon_adv_  = advert_base_.open(endpoint_str_+"RM",   mode);
    
    detail::readAttributesFromDB<faust::resource_description>
      (description_, "faust::resource_description", desc_adv_, log_sptr_);    

    detail::readAttributesFromDB<faust::resource_monitor>
      (monitor_, "faust::resource_monitor", mon_adv_, log_sptr_);    
    
    LOG_WRITE_SUCCESS(msg);
  }
  catch(saga::exception const & e) 
  {
    LOG_WRITE_FAILED_AND_THROW(msg, e.what(), faust::NoSuccess);
  }
  
  try 
  {
		saga::advert::entry auuid(endpoint_str_+"AGENT_UUID", saga::advert::Read);
    agent_uuid_ = auuid.retrieve_string(); 
    
		if(agent_uuid_.length() == 0)
		{
			// something is wrong with the entry. Generating new UUID
			agent_uuid_ = saga::uuid().string();
			msg << "Something seems to be wrong with the 'AGENT_UUID' entry. Generating new one: " << agent_uuid_ << ".";
			log_sptr_->write(SAGA_OSSTREAM_GETSTRING(msg), LOGLEVEL_WARNING);
		}
		send_command(PROTO_V1_PING, 60);
		
  }
	catch(faust::exception const & e) 
  {
		if(e.get_error() == faust::Timeout) 
    {
			msg << "faust_agent instance'" << agent_uuid_ << "'seems to be dead. RESTARTING ";
			log_sptr_->write(SAGA_OSSTREAM_GETSTRING(msg), LOGLEVEL_INFO);
			launch_agent();
			send_command(PROTO_V1_PING, 60);
		}
		else {
			throw;
		}
	}
  catch(saga::exception const & e) 
  {
    msg << "Checking if faust_agent instance is still alive. ";
    LOG_WRITE_FAILED_AND_THROW(msg, e.what(), faust::NoSuccess);
  }
  
  /* starting the service thread */
  msg << "Starting service thread";
  
  try 
  {
    service_thread_ = boost::thread(&main_event_loop);
  }
  catch(...)
  {
    LOG_WRITE_FAILED_AND_THROW(msg, "Unknown Reson", faust::NoSuccess);
  }
  
  LOG_WRITE_SUCCESS(msg);
}


////////////////////////////////////////////////////////////////////////////////
//
resource::resource(faust::resource_description resource_desc, bool persistent) 
: object(faust::object::Resource), description_(resource_desc), 
init_from_id_(false), persistent_(persistent)
{
  init();
  SAGA_OSSTREAM msg;
  agent_uuid_ = saga::uuid().string();
  
  
  /* check if the mandatory attributes to launch a faust_agent
   * are available */
  msg << "Checking faust::resource_description for completeness. ";
	
	if(!description_.attribute_exists(FAR::identifier)) {
    LOG_WRITE_FAILED_AND_THROW(msg, "Missing required attribute 'identifier'", faust::BadParameter);
	}
  else if(!description_.attribute_exists(FAR::faust_agent_submit_url)) {
    LOG_WRITE_FAILED_AND_THROW(msg, "Missing required attribute 'faust_agent_submit_url'", faust::BadParameter);
  }
  else if(!description_.attribute_exists(FAR::faust_agent_binary_path)) {
    LOG_WRITE_FAILED_AND_THROW(msg, "Missing required attribute 'faust_agent_binary_path'", faust::BadParameter);
  }
	else if(!description_.attribute_exists(FAR::saga_root_path)) {
    LOG_WRITE_FAILED_AND_THROW(msg, "Missing required attribute 'saga_root_path'", faust::BadParameter);
  }
	
  else {
    LOG_WRITE_SUCCESS(msg);
  }
  
  resource_id_ = description_.get_attribute(FAR::identifier);
  std::string endpoint_str_(object::faust_root_namesapce_ + 
														"RESOURCES/" + resource_id_ + "/");
  
  int mode = advert::ReadWrite | advert::Create | advert::Recursive;
	  
	/* create advert entry for this resource instance 
   */
  msg << "Creating advert endpoint '" << endpoint_str_ << "' ";
  try 
  {    
    advert_base_ = advert::directory(object::faust_root_namesapce_, mode);
    
    // don't overwrite an existing entry if it has the persistency flag set!
    if(advert_base_.exists("./"+resource_id_ + "/")) 
    {
      saga::advert::directory tmp = advert_base_.open_dir("RESOURCES/" + resource_id_ + "/");
      
      if(tmp.attribute_exists("persistent")) 
      {
        std::string dbg(tmp.get_attribute("persistent"));
        if(tmp.get_attribute("persistent") == "TRUE") 
        {
          LOG_WRITE_FAILED_AND_THROW(msg, "endpoint '"+resource_id_+"' already exists and set to 'persistent'!", faust::NoSuccess);
        }
      }
    }
    else 
    {
      advert_base_ = advert::directory(faust_root_namesapce_, mode);
      advert_base_ = advert::directory(faust_root_namesapce_+"/RESOURCES/", mode);
      advert_base_ = advert::directory(faust_root_namesapce_+"/RESOURCES/"+resource_id_+"/", mode);
    }
    LOG_WRITE_SUCCESS(msg);
  }
  catch(saga::exception const & e) 
  {
    LOG_WRITE_FAILED_AND_THROW(msg, e.what(), faust::NoSuccess);
  }
  
  // set the persistency bit
  if( true == persistent_ )
    advert_base_.set_attribute("persistent", "TRUE");
  else
    advert_base_.set_attribute("persistent", "FALSE");
  
  // create "CMD" entry
  cmd_adv_ = advert_base_.open(endpoint_str_+"CMD", mode);
  cmd_adv_.store_string(""); 
  
  args_adv_ = advert_base_.open(endpoint_str_+"ARGS", mode);
  args_adv_.store_string(""); 
  
  desc_adv_ = advert_base_.open(endpoint_str_+"RD", mode);
  desc_adv_.store_string(""); 
  
  mon_adv_ = advert_base_.open(endpoint_str_+"RM", mode);
  mon_adv_.store_string(""); 
  
  // create "AGENT_UUID" entry
  saga::advert::entry auuid(endpoint_str_+"AGENT_UUID", mode);
  auuid.store_string(agent_uuid_);  
  
  detail::writeAttributesToDB<faust::resource_description>
  (description_, "faust::resource_description", desc_adv_, log_sptr_); 
  
	launch_agent();
  send_command(PROTO_V1_PING, 120);
  
  /* launching service thread
   */
  msg << "Starting service thread";
  try {
    service_thread_ = boost::thread(&main_event_loop);
    LOG_WRITE_SUCCESS(msg);
  }
  catch(...)
  {
    LOG_WRITE_FAILED_AND_THROW(msg, "Unknown Reson", faust::NoSuccess);
  }
}


////////////////////////////////////////////////////////////////////////////////
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
  
  SAGA_OSSTREAM msg;
  msg << "Launching agent " << agent_uuid_ << " via '" 
      << description_.get_attribute(FAR::faust_agent_submit_url) << "' ";
	
	try 
  {
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
      LOG_WRITE_FAILED_AND_THROW(msg, "Job is not running.", faust::NoSuccess);
    }
    else
    {
      LOG_WRITE_SUCCESS(msg);
    }
    
	}
	catch(saga::exception const & e) 
  {
		LOG_WRITE_FAILED_AND_THROW(msg, e.what(), faust::NoSuccess);
	}
}


////////////////////////////////////////////////////////////////////////////////
//
resource::~resource() 
{
  if(false == persistent_) {
    
    send_command(PROTO_V1_TERMINATE);
    
    SAGA_OSSTREAM msg;
    msg << "Removing advert endpoint '" << endpoint_str_ << "'";
    try 
    {
      advert_base_.remove(saga::advert::Recursive);
      LOG_WRITE_SUCCESS(msg);
    }
    catch(saga::exception const & e) 
    {
      LOG_WRITE_FAILED_AND_THROW(msg, e.what(), faust::NoSuccess);
    }
  }
  
  SAGA_OSSTREAM msg;
  msg <<  "Shutting down service thread";
  try 
  {
    service_thread_.join();
    LOG_WRITE_SUCCESS(msg);
  }
  catch(...)
  {
    LOG_WRITE_FAILED_AND_THROW(msg, "Unknown Reason", faust::NoSuccess);
  }
}


////////////////////////////////////////////////////////////////////////////////
//
void resource::send_command(std::string cmd, unsigned int timeout)
{
  /* send a command and waits for an acknowledgement */
  
  SAGA_OSSTREAM msg;
	msg << "Sending command '" << cmd << "' to faust_agent ";
  
  try 
  {
    cmd_adv_.store_string(agent_uuid_+":"+cmd);
    LOG_WRITE_SUCCESS(msg);
  }
  catch(saga::exception const & e) 
  {
    LOG_WRITE_FAILED_AND_THROW(msg, e.what(), faust::NoSuccess);
  }
  
  msg <<  "Waiting for acknowledgement ";
  
  try 
  {
    int to = 0; std::string result;
    while(to < timeout) 
    {
      ++to;
      sleep(1);
      result = cmd_adv_.retrieve_string();
      if(result == std::string("ACK:"+agent_uuid_+":"+cmd))
        break;
    }
    
    if(result == std::string("ACK:"+agent_uuid_+":"+cmd)) {
      cmd_adv_.store_string("");  // Reset CMD
      args_adv_.store_string(""); // Reset ARGS
      LOG_WRITE_SUCCESS(msg);
    }
    else 
    {
      cmd_adv_.store_string("");  // Reset CMD
      args_adv_.store_string(""); // Reset ARGS
      std::stringstream out; out << timeout;
      
      LOG_WRITE_FAILED_AND_THROW(msg, "(Timeout - "+out.str()+" sec)", faust::NoSuccess);
    }
  }
  catch(saga::exception const & e) 
  {
    LOG_WRITE_FAILED_AND_THROW(msg, e.what(), faust::NoSuccess);
  }  
}


////////////////////////////////////////////////////////////////////////////////
//
void resource::set_persistent(bool yesno)
{
  SAGA_OSSTREAM msg;
	msg <<  "Setting persistency for endpoint '" << resource_id_ << "' to ";
  if(true == yesno) msg << "true"; else msg << "false";
  
  try 
  {
    /* set persistency bit */
    if( true == yesno )
      advert_base_.set_attribute("persistent", "TRUE");
    else
      advert_base_.set_attribute("persistent", "FALSE");
    
    LOG_WRITE_SUCCESS(msg);
    persistent_ = yesno;
  }
  catch(saga::exception const & e) 
  {
    LOG_WRITE_FAILED_AND_THROW(msg, e.what(), faust::NoSuccess);
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


