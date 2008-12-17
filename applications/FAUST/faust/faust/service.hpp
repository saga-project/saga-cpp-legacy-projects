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

#ifndef FAUST_SERVICE_HPP
#define FAUST_SERVICE_HPP

#include <boost/shared_ptr.hpp>

#include <faust/faust/object.hpp>
#include <faust/faust/exports.hpp>
#include <faust/faust/state.hpp>
#include <faust/faust/job.hpp>
#include <faust/faust/job_group.hpp>
#include <faust/faust/description.hpp>
#include <faust/faust/dependency.hpp>


namespace faust
{
  // fwd. decl. implementation class // 
  ///@cond - exclude from Doxygen
  namespace impl { class service_impl; }
  ///@endcond - exclude from Doxygen

   
    /*! \brief This structure defines a computing %resource. A list of 
     *         resources is used to create a %faust %job 
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
    
    /*! \brief A %job %service represents a %faust %resource manager that
     *         uses a set of hosts an scheduling strategies to efficiently 
     *         create and schedule %job instances.
     * 
     */
    class FAUST_EXPORT service : public faust::object
    {
      
    private:
      
      boost::shared_ptr <faust::impl::service_impl> get_impl (void) const;
      
    public:
      
      /*! \brief  Creates a new %faust %service instance that schedules 
       *          jobs on the hosts decribed in the provided list of resources.
       *
       *          Detailed %description goes here...
       *
       *          <b>Example:</b><br>
       *          <code> 
       *          std::vector<faust::resource> resources;<br>
       *
       *          faust::resource rd;<br>
       *          rd.contact = "gram://qb.loni.org:2119/jobmanager-pbs";<br>
       *          rd.project = "sample_project";<br>
       *          rd.queue   = "workq";<br>
       *          rd.workdir = "/tmp/";<br>
       * 
       *          resources.push_back(rd);<br>
       * 
       *          faust::service s(resources);
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
      
      /*! \brief  Shuts down this %faust %service instance.
       *
       *          Detailed %description goes here...
       *  
       */
      ~service();
      
      /*! \brief  Creates a new %job instance without dependencies 
       *          to other jobs. 
       *
       *          Detailed %description goes here...
       *
       *  \return A new %job object. 
       * 
       */
      job create_job(description job_desc);

      /*! \brief  Creates a new %job instance which depends on the execution
       *          state of the %job identified by job_id. The %job_id can
       *          point to a %job or a %job_group.
       *
       *  \return A new %job object. 
       * 
       */
      job create_job(description job_desc, std::string job_id, dependency dep);

      /*! \brief  Creates a new %job instance which depends on the execution
       *          state of the provided %job object.
       *
       *  \return A new %job object. 
       * 
       */
      job create_job(description job_desc, job job_obj, dependency dep);
      
      /*! \brief  Creates a new %job instance which depends on the execution
       *          state of the provided %job_group object.
       *
       *  \return A new job object. 
       * 
       */
      job create_job(description job_desc, job_group job_group_obj, dependency dep);
      
      
      /*! \brief  Creates a new %job_group instance without %dependecies to
       *          other jobs.
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
       *          faust::service s(resources);<br> 
       *          faust::job_group jg = s.create_job_group(desc);<br>
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
       *          of another %job instance. 
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
       *          faust::service s(resources);<br> <br>
       *          //Creates a job_group that can't be scheduled before <br>
       *          //j1 has reached DONE state.<br>
       *          faust::job_group jg = s.create_job_group(desc, j1, Data);<br>
       *          </code>
       *
       *  \param  job_descs A list of %job %description objects describing the 
       *          %job properties.
       *  \param  dep_job The job object that provides the state on which the 
       *          scheduling of this %job_group depends on.
       *  \param  dep The type of dependency 
       *  \return A new job_group object containing one ore more jobs. 
       * 
       */
      job_group create_job_group(std::vector<description> job_descs, 
                                 std::string dep_job, dependency dep);

      /*! \brief  Creates a new %job_group instance which depends on the 
       *          execution state of the provided %job object.
       */
      job_group create_job_group(std::vector<description> job_descs, 
                                 job job_obj, dependency dep);
      
      /*! \brief  Creates a new %job_group instance which depends on the 
       *          execution state of the provided %job_group object.
       */
      job_group create_job_group(std::vector<description> job_descs, 
                                 job_group job_group_obj, dependency dep);
      
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
       *   When the %faust %service starts up, it iterates over the list
       *   of given %resouces and validates them. If validation 
       *   fails for a %resource, the %resource is removed from the internal
       *   list and hence won't show up in the %list_resources vector.  
       *
       *         <b>Usage example:</b><br><br>
       *         <code>
       *         faust::service s(resources);<br>
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
       *  \return The %job object.
       *
       */
      faust::job get_job(std::string job_id);

      /*! \brief  Returns a %job_group object for a given %job ID.
       *         
       *  \return The %job_group object.
       *
       */
      faust::job_group get_job_group(std::string job_id);
      
      /*! \brief  Returns a %resource %description for a given contact string.
       *         
       *  \return The job object.
       *
       */
      faust::resource get_resource(std::string contact);
      
    };
}


#endif /* FAUST_DESCRIPTION_HPP */
