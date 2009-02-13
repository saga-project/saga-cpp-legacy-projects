/*
 *  service_impl.cpp 
 *  FAUST - Framework for Adaptive Ubiquitous Scalable Tasks
 *  Website: https://macpro01.cct.lsu.edu/trac/faust
 *
 *  Created by Ole Weidner <oweidner@cct.lsu.edu> on 11/30/08.
 *  Copyright 2008-2009 Center for Computation & Technology. 
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying 
 *  LICENSE file or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include <faust/faust/defines.hpp>
#include <faust/faust/object.hpp>
#include <faust/faust/job.hpp>
#include <faust/faust/job_group.hpp>
#include <faust/faust/resource.hpp>
#include <faust/faust/description.hpp>
#include <faust/faust/exception.hpp>

#include <faust/impl/service_impl.hpp>

#include <saga/saga.hpp>

#include <boost/utility/addressof.hpp>

using namespace faust::impl;

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR
//
service_impl::service_impl (std::vector<faust::resource_description> resource_descriptions, int num_jobs)
: object(faust::object::Service)
{
  using namespace saga::job;
  
  // Initialize log service for this instance
  std::string identifier(FW_NAME);
  identifier.append(" faust::service");
  log_ = new detail::logwriter(identifier, std::cout);
  
  std::string msg("Starting new job servie instance." );
  log_->write(msg, LOGLEVEL_INFO);
  
  // check if the given hosts, queues, projects are valid!
  unsigned int fails = 0;
  
  std::vector<faust::resource_description>::iterator i;
  for(i = resource_descriptions.begin(); i != resource_descriptions.end(); ++i)
  {
    saga::url contact((*i).get_attribute("resource_description"));
    saga::job::service sjs;
    
    // Try to initialize the job service - if it fails, remove this entry
    // from the list.
    try {
      std::string msg("Checking resource_description availability: " + contact.get_url());
      log_->write(msg, LOGLEVEL_INFO);
      
      sjs = saga::job::service(contact);
    }
    catch(saga::exception const & e)
    {
      ++fails;
      
      if(DEBUG) log_->write(e.what(), LOGLEVEL_DEBUG);
      std::string msg("Cannot connect with resource_description: " + contact.get_url());
      msg.append(". Removing entry from resource_description list." );
      log_->write(msg, LOGLEVEL_ERROR); 
      
      continue; // we don't want to try queueing if this stage already fails!
    }
    
    // Try to queue a sample dummy job to see if the queue and project 
    // informations a valid. If it fails, remove this entry from the list.
    try {
      /*std::string msg("Queuing sample job on: " + (*i).contact.get_url());
      msg.append(" (queue="+(*i).queue+", project="+(*i).project+")");
      log_->write(msg, LOGLEVEL_INFO);
      
      saga::job::description jd;
      jd.set_attribute (saga::job::attributes::description_executable,  "/bin/date");
      jd.set_attribute (saga::job::attributes::description_interactive, saga::attributes::common_false);
      jd.set_attribute (saga::job::attributes::description_queue, (*i).queue);
      std::vector<std::string> project; project.push_back((*i).project);
      jd.set_vector_attribute (saga::job::attributes::description_job_project, project);
      
      saga::job::job j = sjs.create_job(jd);
      j.run(); 
      sleep(2); // GRAM needs some time otherwise it will reply with an error! 
      //std::cout << j.get_job_id() << std::endl;
      j.cancel();
      
      // Host description seems to work properly. Add it to our
      // internal host list.
      resource_descriptions_.insert(resource_descriptions_pair((*i).contact.get_url(), (*i)));*/
    }
    catch(saga::exception const & e)
    {
     /* ++fails;
      
      if(DEBUG) log_->write(e.what(), LOGLEVEL_DEBUG);
      std::string msg("Cannot queue a sample job on: " + (*i).contact.get_url());
      msg.append(". Removing entry from resource_description list." );
      log_->write(msg, LOGLEVEL_ERROR);  */
    }
  }
  
  // if we don't have any working execution hosts, abort!
  if(fails == resource_descriptions.size()) {
    log_->write("No usable resource_descriptions available. Aborting.", LOGLEVEL_FATAL); 
    exit(-1);
    // FATAL -> THROW Exception! 
  }
  
}

////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR
service_impl::~service_impl ()
{
  delete log_;
}

////////////////////////////////////////////////////////////////////////////////
// 
std::vector<std::string> service_impl::list_jobs(void)
{
  std::vector<std::string> job_ids;
  
  joblist_map_t::const_iterator ci;
  for(ci = joblist_.begin(); ci != joblist_.end(); ++ci)
  {
    job_ids.push_back(ci->first);
  }
  
  return job_ids;
}

////////////////////////////////////////////////////////////////////////////////
// 
faust::resource_description service_impl::get_resource_description(std::string contact)
{
  if(resource_descriptions_.find(contact) == resource_descriptions_.end())
  {
    throw faust::exception ("Contact string '"+contact+"' doesn't exisit." , 
                            faust::BadParameter);
  }
  
  return resource_descriptions_[contact];
}

////////////////////////////////////////////////////////////////////////////////
// 
std::vector<std::string> service_impl::list_resource_descriptions(void)
{
  std::vector<std::string> res;
  
  resource_descriptions_map::const_iterator ci;
  for(ci = resource_descriptions_.begin(); ci != resource_descriptions_.end(); ++ci)
  {
    res.push_back(ci->first);
  }
  
  return res;
}

////////////////////////////////////////////////////////////////////////////////
// 
faust::job_group 
service_impl::create_job_group(std::vector<faust::job_description> job_descs)
{
  faust::job_group ret;
  insert_job_into_job_list(ret.get_job_id(), ret);
  return ret;
}

////////////////////////////////////////////////////////////////////////////////
// 
faust::job_group 
service_impl::create_job_group(std::vector<faust::job_description> job_descs, 
                               std::string dep_job_id, 
                               dependency dep)
{
  faust::job_group ret;
  insert_job_into_job_list(ret.get_job_id(), ret);
  return ret;
}

////////////////////////////////////////////////////////////////////////////////
// 
faust::job_group 
service_impl::create_job_group(std::vector<faust::job_description> job_descs, 
                               faust::job job_obj, 
                               dependency dep)
{
  faust::job_group ret;
  insert_job_into_job_list(ret.get_job_id(), ret);
  return ret;
}

////////////////////////////////////////////////////////////////////////////////
// 
faust::job_group 
service_impl::create_job_group(std::vector<faust::job_description> job_descs, 
                               faust::job_group job_group_obj, 
                               dependency dep)
{
  faust::job_group ret;
  insert_job_into_job_list(ret.get_job_id(), ret);
  return ret;
}


////////////////////////////////////////////////////////////////////////////////
// 
faust::job
service_impl::create_job(faust::job_description job_descs)
{
  faust::job ret;  
  insert_job_into_job_list(ret.get_job_id(), ret);
  return ret;
}

////////////////////////////////////////////////////////////////////////////////
// 
faust::job
service_impl::create_job(faust::job_description job_descs,
                         std::string dep_job_id, 
                         faust::dependency dep)
{
  faust::job ret;  
  insert_job_into_job_list(ret.get_job_id(), ret);
  return ret;
}

////////////////////////////////////////////////////////////////////////////////
// 
faust::job
service_impl::create_job(faust::job_description job_descs,
                         faust::job job_obj, 
                         faust::dependency dep)
{
  faust::job ret;  
  insert_job_into_job_list(ret.get_job_id(), ret);
  return ret;
}

////////////////////////////////////////////////////////////////////////////////
// 
faust::job
service_impl::create_job(faust::job_description job_descs,
                         faust::job_group job_group_obj, 
                         faust::dependency dep)
{
  faust::job ret;  
  insert_job_into_job_list(ret.get_job_id(), ret);
  return ret;
}

////////////////////////////////////////////////////////////////////////////////
// 
void service_impl::insert_job_into_job_list(std::string jobid, faust::object obj) 
{
  joblist_.insert(joblist_pair_t(jobid, obj));
  std::string msg("Registering new " + faust::get_object_type_name(obj) + " instance: " + jobid);
  log_->write(msg, LOGLEVEL_INFO);  
}

////////////////////////////////////////////////////////////////////////////////
// 
faust::job
service_impl::get_job(std::string job_id)
{
  if( joblist_.find(job_id) == joblist_.end())
    throw faust::exception ("JobID doesn't exisit." , faust::BadParameter);
  
  else if( joblist_[job_id].get_type() != faust::object::Job )
    throw faust::exception ("TODO: Describe error" , faust::BadParameter);
  
  else
    return *static_cast<faust::job *> (boost::addressof(joblist_[job_id]));
}

////////////////////////////////////////////////////////////////////////////////
// 
faust::job_group
service_impl::get_job_group(std::string job_id)
{
  if( joblist_.find(job_id) == joblist_.end())
    throw faust::exception ("JobID doesn't exisit." , faust::BadParameter);
  
  else if( joblist_[job_id].get_type() != faust::object::JobGroup )
    throw faust::exception ("TODO: Describe error" , faust::BadParameter);
  
  else
    return *static_cast<faust::job_group *> (boost::addressof(joblist_[job_id]));
}

