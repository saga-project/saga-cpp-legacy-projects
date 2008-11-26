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

#include <faust/manyjobs/job.hpp>
#include <faust/manyjobs/service.hpp>
#include <faust/manyjobs/description.hpp>

namespace faust
{
  namespace manyjobs {
 
    /*! \brief Structure representing an execution host.
     *  
     */
    struct host_description {
      saga::url    contact;
      saga::url    workdir;
      std::string  queue;
      std::string  project;
    };
    
    // forward decl. 
    class job; 
    
    typedef boost::shared_ptr<job> job_ptr;
    typedef std::map<std::string,  job_ptr> joblist_map;
    typedef std::pair<std::string, job_ptr> joblist_pair;
    
    class FAUST_EXPORT service : public saga::object
    {
      
    private:
      
      std::vector<host_description>   hostlist_;
      std::vector<saga::job::service> servicelist_;
      std::map<std::string, job_ptr>  joblist_;
      faust::detail::logwriter *      log_;
      
    public:
      
      /*! \brief Tries to create a new %manyjobs %service instance
       *         that uses the hosts decribed by hostlist.
       *  
       */
      explicit service (std::vector<host_description> hostlist);
      
      /*! \brief Tries to properly shut down this %manyjobs instance.
       *  
       */
      ~service();
      
      /*! \brief  Tries to create a new %job instance described by
       *          %job %description.
       *  \return A new job object. 
       * 
       */
      faust::manyjobs::job * create_job(faust::manyjobs::description job_desc);
      
      /*! \brief  Tries to get a list of jobs currently known by 
       *          this %service instance.
       *  \return List of job identifiers.
       * 
       */
      std::vector<std::string> list(void); 
      
      /*! \brief  Tries to return a %job object for a given %job ID.
       *         
       *  \return The job object.
       *
       */
      faust::manyjobs::job get_job(std::string job_id);
      
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
