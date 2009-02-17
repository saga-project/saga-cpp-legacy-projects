/*
 *  service.hpp
 *  FAUST - Framework for Adaptive Ubiquitous Scalable Tasks
 *  Website: https://macpro01.cct.lsu.edu/trac/faust
 *
 *  Created by Ole Weidner <oweidner@cct.lsu.edu> on 11/22/08.
 *  Copyright 2008-2009 Center for Computation & Technology. 
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
#include <faust/faust/resource.hpp>
#include <faust/faust/dependency.hpp>


namespace faust
{
  // fwd. decl. implementation class // 
  ///@cond - exclude from Doxygen
  namespace impl { class service_impl; }
  ///@endcond - exclude from Doxygen
    
    class job; class job_group;
    
    /*! \brief A %job %service represents a %faust %resource_description manager that
     *         uses a set of hosts an scheduling strategies to efficiently 
     *         create and schedule %job instances.
     * 
     */
    class  service : public faust::object
    {
      
    private:
      
      boost::shared_ptr <faust::impl::service_impl> get_impl (void) const;
      
    public:
      
      /*! \brief  Creates a new %faust %service instance that schedules 
       *          jobs on the hosts decribed in the provided list of resource_descriptions.
       *
       *          Detailed %description goes here ...
       *
       *          <b>Example:</b><br>
       *          <code> 
       *          std::vector<faust::resource_description> resource_descriptions;<br>
       *
       *          faust::resource_description rd;<br>
       *          rd.contact = "gram://qb.loni.org:2119/jobmanager-pbs";<br>
       *          rd.project = "sample_project";<br>
       *          rd.queue   = "workq";<br>
       *          rd.workdir = "/tmp/";<br>
       * 
       *          resource_descriptions.push_back(rd);<br>
       * 
       *          faust::service s(resource_descriptions);
       *
       *          </code> 
       *
       *  \param  resource_descriptions List of resource_description objects this %service instance
       *                    should work with.
       *  \param  num_jobs Total number of jobs this %service instance can
       *                   expect to run (scheduling hint).
       *  
       */
      explicit service (std::vector<faust::resource> resource_descriptions, int num_jobs);
      
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
      job create_job(job_description job_desc);

      /*! \brief  Creates a new %job instance which depends on the execution
       *          state of the %job identified by job_id. The %job_id can
       *          point to a %job or a %job_group.
       *
       *  \return A new %job object. 
       * 
       */
      job create_job(job_description job_desc, std::string job_id, dependency dep);

      /*! \brief  Creates a new %job instance which depends on the execution
       *          state of the provided %job object.
       *
       *  \return A new %job object. 
       * 
       */
      job create_job(job_description job_desc, job job_obj, dependency dep);
      
//      /* \brief  Creates a new %job instance which depends on the execution
//       *          state of the provided %job_group object.
//       *
//       *  \return A new job object. 
//       * 
//       */
//    job create_job(description job_desc, job_group job_group_obj, dependency dep);
      
      
//      /* \brief  Creates a new %job_group instance without %dependecies to
//       *          other jobs.
//       *
//       *          Detailed %description goes here...
//       *          
//       *          <b>Example:</b><br>
//       *          <code> 
//       *          std::vector<description> desc;<br>
//       *          desc.push_back(jd1);<br>
//       *          desc.push_back(jd2);<br>
//       *          desc.push_back(jd3);<br>
//       *          <br>
//       *          faust::service s(resource_descriptions);<br> 
//       *          faust::job_group jg = s.create_job_group(desc);<br>
//       *          </code>
//       *
//       *  \param  job_descs A list of %job %description objects describing the 
//       *          %job properties.
//       *
//       *  \return A new job_group object containing one ore more jobs. 
//       * 
//       */
//    job_group create_job_group(std::vector<description> job_descs);
      
//      /* \brief  Creates a new %job_group instance that depends on the state
//       *          of another %job instance. 
//       *
//       *          Detailed %description goes here...
//       *
//       *          <b>Example:</b><br>
//       *          <code> 
//       *          std::vector<description> desc;<br>
//       *          desc.push_back(jd1);<br>
//       *          desc.push_back(jd2);<br>
//       *          desc.push_back(jd3);<br>
//       *          <br>
//       *          faust::service s(resource_descriptions);<br> <br>
//       *          //Creates a job_group that can't be scheduled before <br>
//       *          //j1 has reached DONE state.<br>
//       *          faust::job_group jg = s.create_job_group(desc, j1, Data);<br>
//       *          </code>
//       *
//       *  \param  job_descs A list of %job %description objects describing the 
//       *          %job properties.
//       *  \param  dep_job The job object that provides the state on which the 
//       *          scheduling of this %job_group depends on.
//       *  \param  dep The type of dependency 
//       *  \return A new job_group object containing one ore more jobs. 
//       * 
//       */
//    job_group create_job_group(std::vector<description> job_descs, 
//                                 std::string dep_job, dependency dep);

//      /* \brief  Creates a new %job_group instance which depends on the 
//       *          execution state of the provided %job object.
//       */
//    job_group create_job_group(std::vector<description> job_descs, 
//                                 job job_obj, dependency dep);
      
//      /* \brief  Creates a new %job_group instance which depends on the 
//       *          execution state of the provided %job_group object.
//       */
//    job_group create_job_group(std::vector<description> job_descs, 
//                                 job_group job_group_obj, dependency dep);
      
      /*! \brief  Lists the IDs of all jobs that are currently 
       *          associated with this %service instance.
       *
       *  \return List of job identifiers.
       * 
       */
      std::vector<std::string> list_jobs(void); 

      /*! \brief  Lists the contact strings of all resource_descriptions that 
       *          are associated with this %service instance.
       *
       *   When the %faust %service starts up, it iterates over the list
       *   of given %resouces and validates them. If validation 
       *   fails for a %resource_description, the %resource_description is removed from the internal
       *   list and hence won't show up in the %list_resource_descriptions vector.  
       *
       *         <b>Usage example:</b><br><br>
       *         <code>
       *         faust::service s(resource_descriptions);<br>
       *         std::vector<std::string> rl = s.list_resource_descriptions();<br>
       *         std::vector<std::string>::const_iterator ci;<br>
       *         for(ci = rl.begin(); ci != rl.end(); ++ci) <br>
       *         {<br>
       *         &nbsp;&nbsp;std::cout << (*ci) << std::endl; <br>
       *         }
       *
       *         </code> 
       *  \return List of %resource_description contact strings.
       * 
       */
      std::vector<std::string> list_resource_descriptions(void); 
      
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
//    faust::job_group get_job_group(std::string job_id);
      
      /*! \brief  Returns a %resource_description %description for a given contact string.
       *         
       *  \return The job object.
       *
       */
      faust::resource_description get_resource_description(std::string contact);
      
    };
}


#endif /* FAUST_DESCRIPTION_HPP */
