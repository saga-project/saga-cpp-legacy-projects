/*
 *  service.cpp
 *  FAUST - Framework for Adaptive Ubiquitous Scalable Tasks
 *
 *  Created by Ole Weidner on 11/23/08.
 *  Copyright 2008 Center for Computation & Technology. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying 
 *  LICENSE file or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include <saga/impl/exception.hpp>

#include <faust/faust/defines.hpp>
#include <faust/manyjobs/service.hpp>

using namespace faust;

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR
//
manyjobs::service::service(std::vector<host_description> hostlist)
{
  using namespace saga::job;
  
  // Initialize log service for this instance
  std::string identifier(FW_NAME);
  identifier.append(" manyjobs::service");
  log_ = new detail::logwriter(identifier, std::cout);
  
  // check if the given hosts, queues, projects are valid!
  unsigned int fails = 0;
  
  std::vector<host_description>::iterator i;
  for(i = hostlist.begin(); i != hostlist.end(); ++i)
  {
    saga::url contact((*i).contact);
    saga::job::service sjs;
    
    // Try to initialize the job service - if it fails, remove this entry
    // from the list.
    try {
      std::string msg("Checking job service availability: " + contact.get_url());
      log_->write(msg, LOGLEVEL_INFO);
      
      sjs = saga::job::service(contact);
    }
    catch(saga::exception const & e)
    {
      ++fails;
      
      if(DEBUG) log_->write(e.what(), LOGLEVEL_DEBUG);
      std::string msg("Cannot connect with job service: " + contact.get_url());
      msg.append(". Removing entry from host list." );
      log_->write(msg, LOGLEVEL_ERROR); 
      
      continue; // we don't want to try queueing if this stage already fails!
    }
    
    // Try to queue a sample dummy job to see if the queue and project 
    // informations a valid. If it fails, remove this entry from the list.
    try {
      std::string msg("Queuing a sample job on: " + (*i).contact.get_url());
      msg.append(" (queue="+(*i).queue+", project="+(*i).project+")");
      log_->write(msg, LOGLEVEL_INFO);
      
      saga::job::description jd;
      jd.set_attribute (attributes::description_executable,  "/bin/date");
      jd.set_attribute (attributes::description_interactive, saga::attributes::common_false);
      jd.set_attribute (attributes::description_queue,       (*i).queue);
      std::vector<std::string> project; project.push_back((*i).project);
      jd.set_vector_attribute (attributes::description_job_project, project);
      
      saga::job::job j = sjs.create_job(jd);
      j.run();
      j.cancel();
    }
    catch(saga::exception const & e)
    {
      ++fails;
      
      if(DEBUG) log_->write(e.what(), LOGLEVEL_DEBUG);
      std::string msg("Cannot queue a sample job on: " + (*i).contact.get_url());
      msg.append(". Removing entry from host list." );
      std::cout << e.what() << std::endl;
      log_->write(msg, LOGLEVEL_ERROR);  
    }
    
    // Host description seems to work properly. Add it to our
    // internal host list.
    hostlist_.push_back((*i));
    
  }
  
  // if we don't have any working execution hosts, abort!
  if(fails == hostlist.size()) {
    log_->write("No usable job services available. Aborting.", LOGLEVEL_FATAL); 
    exit(-1);
    // FATAL -> THROW Exception! 
  }
  
  // start the persistant service instances for the remaining hosts
  //create_job_services();
  
}

////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR
//
manyjobs::service::~service()
{
  delete log_;
}

////////////////////////////////////////////////////////////////////////////////
// 
//
manyjobs::job * manyjobs::service::create_job(manyjobs::description jdesc)
{
  job_ptr jp( new manyjobs::job() );
  jp->set_service(this);
  
  joblist_.insert(joblist_pair(jp->get_job_id(), jp));
  
  return jp.get();
}
  
////////////////////////////////////////////////////////////////////////////////
// 
//
std::vector<std::string> manyjobs::service::list(void)
{
  std::vector<std::string> job_ids;
  
  joblist_map::const_iterator ci;
  for(ci = joblist_.begin(); ci != joblist_.end(); ++ci)
  {
    job_ids.push_back(ci->first);
  }

  return job_ids;
}

////////////////////////////////////////////////////////////////////////////////
// TEMPORARY (DEBUG) 
//
void manyjobs::service::debug_check()
{
  joblist_map::const_iterator ci;
  for(ci = joblist_.begin(); ci != joblist_.end(); ++ci)
  {
    std::cout << "id: " << ci->second->get_job_id() << " state: " 
    << ci->second->get_state() << std::endl;
  }
  
}

////////////////////////////////////////////////////////////////////////////////
// 
//
void manyjobs::service::create_job_services()
{
  std::vector<host_description>::iterator i;
  for(i = hostlist_.begin(); i != hostlist_.end(); ++i)
  {
    try {
      saga::url contact((*i).contact);
      std::string msg("Connecting to job service: " + (*i).contact.get_url());
      log_->write(msg, LOGLEVEL_INFO);
      servicelist_.push_back(saga::job::service((*i).contact));
      

      //servicelist_.push_back(saga::job::service((*i).contact));
    }
    catch(saga::exception const & e)
    {
      //hostlist.erase(i);
      std::string msg("Cannot connect with job service: " + (*i).contact.get_url());
      log_->write(msg, LOGLEVEL_ERROR);  
    }
  }
  
}

