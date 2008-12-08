/*
 *  job_service_impl.hpp 
 *  FAUST - Framework for Adaptive Ubiquitous Scalable Tasks
 *
 *  Created by Ole Weidner on 11/30/08.
 *  Copyright 2008 Center for Computation & Technology. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying 
 *  LICENSE file or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef FAUST_IMPL_MANYJOBS_SERVICE_IMPL_HPP
#define FAUST_IMPL_MANYJOBS_SERVICE_IMPL_HPP

#include <map>
#include <saga/saga.hpp>
#include <boost/shared_ptr.hpp>

#include <faust/faust/exports.hpp> 
#include <faust/faust/state.hpp>
#include <faust/faust/job.hpp>

#include <faust/impl/logwriter.hpp>

namespace faust
{
  namespace impl
  {
    namespace manyjobs 
    {
      class job; // fwd. decl.

      typedef std::map<std::string,  faust::manyjobs::job> joblist_map;
      typedef std::pair<std::string, faust::manyjobs::job> joblist_pair;
      
      typedef std::map<std::string,  faust::manyjobs::resource> resources_map;
      typedef std::pair<std::string, faust::manyjobs::resource> resources_pair;
            
      class FAUST_EXPORT service_impl : public saga::object
      {

      private:
        faust::detail::logwriter * log_;
        
        resources_map              resources_;
        joblist_map                joblist_;
        
      public:
        
        explicit service_impl (std::vector<faust::manyjobs::resource> resources, 
                               int num_jobs);
        ~service_impl();
        
        faust::manyjobs::job create_job(faust::manyjobs::description job_desc);
        faust::manyjobs::job create_job(faust::manyjobs::description job_desc, 
                                        std::string dep_job_id, 
                                        faust::manyjobs::state job_state);
        
        faust::manyjobs::job_group create_job_group(std::vector<faust::manyjobs::description> job_descs);
        faust::manyjobs::job_group create_job_group(std::vector<faust::manyjobs::description> job_descs, 
                                                    std::string dep_job_id, 
                                                    faust::manyjobs::state job_state);
        
        std::vector<std::string> list_jobs(void); 
        std::vector<std::string> list_resources(void); 
        
        faust::manyjobs::job get_job(std::string job_id);
        faust::manyjobs::resource get_resource(std::string contact);

      };
    }
  }
}

#endif /* FAUST_IMPL_MANYJOBS_SERVICE_IMPL_HPP */