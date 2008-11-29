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

#include <saga/saga.hpp>

#include <boost/shared_ptr.hpp>

#include <faust/faust/defines.hpp>
#include <faust/faust/logwriter.hpp>

#include <faust/manyjobs/state.hpp>
#include <faust/manyjobs/job.hpp>
#include <faust/manyjobs/service.hpp>
#include <faust/manyjobs/description.hpp>

namespace faust
{
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
    
    // forward decl. 
    class job; class job_group; 
    
    ///@cond - exclude from Doxygen
    typedef boost::shared_ptr<job> job_ptr;
    typedef std::map<std::string,  job_ptr> joblist_map;
    typedef std::pair<std::string, job_ptr> joblist_pair;
    
    typedef std::map<std::string,  resource> resources_map;
    typedef std::pair<std::string, resource> resources_pair;
    
    ///@endcond - exclude from Doxygen
    
    /*! \brief A %job %service represents a %manyjobs resource manager that
     *         uses a set of hosts an scheduling strategies to efficiently 
     *         create and schedule %job instances.
     * 
     *         <br><b>Usage example:</b><br><br>
     *         <code> 
     *         std::vector<faust::manyjobs::resource> resources;<br>
     *
     *         faust::manyjobs::resource rd;<br>
     *         rd.contact = "gram://qb.loni.org:2119/jobmanager-pbs";<br>
     *         rd.project = "sample_project";<br>
     *         rd.queue   = "workq";<br>
     *         rd.workdir = "/tmp/";<br>
     * 
     *         resources.push_back(rd);<br>
     * 
     *         faust::manyjobs::service s(resources);
     *
     *         </code> 
     */
    class FAUST_EXPORT service : public saga::object
    {
      
    private:
      faust::detail::logwriter *      log_;
      
      resources_map  resources_;
      joblist_map    joblist_;
      
    public:
      
      /*! \brief Tries to create a new %manyjobs %service instance
       *         that uses the hosts decribed by hostlist.
       *  
       */
      explicit service (std::vector<resource> resources, int num_jobs);
      
      /*! \brief Tries to properly shut down this %manyjobs instance.
       *  
       */
      ~service();
      
      /*! \brief  Creates a new %job instance without dependencies ('top-level %job') 
       *          based on the provided %job %description.
       *
       *  \return A new job object pointer. 
       * 
       */
      job * create_job(description job_desc);

      /*! \brief  Creates a new 'sub-job' instance which execution state depends
       *          on the state of the %job identified by job_id. 
       *
       *  \return A new job object pointer. 
       * 
       */
      job * create_job(description job_desc, std::string job_id, state job_state);

      
      /*! \brief  Creates a new %job instance without dependencies ('top-level %job') 
       *          based on the provided %job %description.
       *
       *  \return A new job object pointer. 
       * 
       */
      job_group create_job_group(std::vector<description> job_descs);
      
      /*! \brief  Creates a new group of %jobs ('sub-job') instance which all 
       *          depend on the state of the %job identified by job_id. 
       *
       *  \return A new job object pointer. 
       * 
       */
      job_group create_job_group(std::vector<description> job_descs, 
                                           std::string job_id, state job_state);
      
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
      
      
      /*! \brief  Temporary debug method...
       *
       */
      void debug_check();
      
    private:
      
      /* Create job services
       *
       */ 
      void create_job_services();
      
      
    };
  }
}


#endif /* FAUST_MANYJOBS_DESCRIPTION_HPP */
