/*
 *  service.hpp
 *  FAUST - Framework for Adaptive Ubiquitous Scalable Tasks
 *
 *  Created by Ole Weidner on 11/22/08.
 *  Copyright 2008 Center for Computation & Technology. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying 
 *  LICENSE file or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef FAUST_MANYJOBS_SERVICE_HPP
#define FAUST_MANYJOBS_SERVICE_HPP

#include <boost/shared_ptr.hpp>

#include <faust/faust/exports.hpp>
#include <faust/faust/manyjobs/state.hpp>
#include <faust/faust/manyjobs/job.hpp>
#include <faust/faust/manyjobs/job_group.hpp>
#include <faust/faust/manyjobs/description.hpp>

namespace faust
{
  // fwd. decl. implementation class // 
  ///@cond - exclude from Doxygen
  namespace impl { namespace manyjobs { class service_impl; } }
  ///@endcond - exclude from Doxygen

  
  namespace manyjobs {
 
    /*! \brief This structure defines a computing %resource. A list of 
     *         resources is used to create a manyjob 
     *         %service instance.
     *  
     */
    struct resource {
      saga::url    contact;
      saga::url    workdir;
      std::string  queue;
      std::string  project;
    };
    
    class job; class job_group;
    
    /*! \brief A %job %service represents a %manyjobs %resource manager that
     *         uses a set of hosts an scheduling strategies to efficiently 
     *         create and schedule %job instances.
     * 
     */
    class FAUST_EXPORT service : public saga::object
    {
      
    private:
      
      typedef boost::shared_ptr<faust::impl::manyjobs::service_impl> impl_ptr;
      impl_ptr impl;

      
    public:
      
      /*! \brief  Creates a new %manyjobs %service instance that schedules 
       *          jobs on the hosts decribed in the provided list of resources.
       *
       *          Detailed %description goes here...
       *
       *          <b>Example:</b><br>
       *          <code> 
       *          std::vector<faust::manyjobs::resource> resources;<br>
       *
       *          faust::manyjobs::resource rd;<br>
       *          rd.contact = "gram://qb.loni.org:2119/jobmanager-pbs";<br>
       *          rd.project = "sample_project";<br>
       *          rd.queue   = "workq";<br>
       *          rd.workdir = "/tmp/";<br>
       * 
       *          resources.push_back(rd);<br>
       * 
       *          faust::manyjobs::service s(resources);
       *
       *          </code> 
       *
       *  \param  resources List of resource objects this %service instance
       *                    should work with.
       *  \param  num_jobs Total number of jobs this %service instance can
       *                   expect to run (scheduling hint).
       *  
       */
      explicit service (std::vector<resource> resources, int num_jobs);
      
      /*! \brief  Shuts down this %manyjobs %service instance.
       *
       *          Detailed %description goes here...
       *  
       */
      ~service();
      
      /*! \brief  Creates a new 'top-level-job' instance without dependencies 
       *          to other jobs. 
       *
       *          Detailed %description goes here...
       *
       *  \return A new job object pointer. 
       * 
       */
      job create_job(description job_desc);

      /*! \brief  Creates a new 'sub-job' instance which execution state depends
       *          on the state of the %job identified by job_id. 
       *
       *  \return A new job object pointer. 
       * 
       */
      job create_job(description job_desc, std::string job_id, state job_state);

      
      /*! \brief  Creates a new %job_group instance. All jobs in the %job_group
       *          are initially in <b>PENDING</b> state.
       *
       *          Detailed %description goes here...
       *          
       *          <b>Example:</b><br>
       *          <code> 
       *          std::vector<description> desc;<br>
       *          desc.push_back(jd1);<br>
       *          desc.push_back(jd2);<br>
       *          desc.push_back(jd3);<br>
       *          <br>
       *          faust::manyjobs::service s(resources);<br> 
       *          faust::manyjobs::job_group jg = s.create_job_group(desc);<br>
       *          </code>
       *
       *  \param  job_descs A list of %job %description objects describing the 
       *          %job properties.
       *
       *  \return A new job_group object containing one ore more jobs. 
       * 
       */
      job_group create_job_group(std::vector<description> job_descs);
      
      /*! \brief  Creates a new %job_group instance that depends on the state
       *          of another %job instance. All jobs in the %job_group
       *          are initially in <b>PENDING</b> state.
       *
       *          Detailed %description goes here...
       *
       *          <b>Example:</b><br>
       *          <code> 
       *          std::vector<description> desc;<br>
       *          desc.push_back(jd1);<br>
       *          desc.push_back(jd2);<br>
       *          desc.push_back(jd3);<br>
       *          <br>
       *          faust::manyjobs::service s(resources);<br> <br>
       *          //Creates a job_group that can't be scheduled before <br>
       *          //j1 has reached DONE state.<br>
       *          faust::manyjobs::job_group jg = s.create_job_group(desc, j1, Done);<br>
       *          </code>
       *
       *  \param  job_descs A list of %job %description objects describing the 
       *          %job properties.
       *  \param  dep_job The job object that provides the state on which the 
       *          scheduling of this %job_group depends on.
       *  \param  job_state The state 
       *  \return A new job_group object containing one ore more jobs. 
       * 
       */
      job_group create_job_group(std::vector<description> job_descs, 
                                 std::string dep_job, state job_state);
      
      /*! \brief  Lists the IDs of all jobs that are currently 
       *          associated with this %service instance.
       *
       *  \return List of job identifiers.
       * 
       */
      std::vector<std::string> list_jobs(void); 

      /*! \brief  Lists the contact strings of all resources that 
       *          are associated with this %service instance.
       *
       *   When the %manyjobs %service starts up, it iterates over the list
       *   of given %resouces and validates them. If validation 
       *   fails for a %resource, the %resource is removed from the internal
       *   list and hence won't show up in the %list_resources vector.  
       *
       *         <b>Usage example:</b><br><br>
       *         <code>
       *         faust::manyjobs::service s(resources);<br>
       *         std::vector<std::string> rl = s.list_resources();<br>
       *         std::vector<std::string>::const_iterator ci;<br>
       *         for(ci = rl.begin(); ci != rl.end(); ++ci) <br>
       *         {<br>
       *         &nbsp;&nbsp;std::cout << (*ci) << std::endl; <br>
       *         }
       *
       *         </code> 
       *  \return List of %resource contact strings.
       * 
       */
      std::vector<std::string> list_resources(void); 
      
      /*! \brief  Returns a %job object for a given %job ID.
       *         
       *  \return The job object.
       *
       */
      faust::manyjobs::job get_job(std::string job_id);

      /*! \brief  Returns a %resource %description for a given contact string.
       *         
       *  \return The job object.
       *
       */
      faust::manyjobs::resource get_resource(std::string contact);
      
    };
  }
}


#endif /* FAUST_MANYJOBS_DESCRIPTION_HPP */
