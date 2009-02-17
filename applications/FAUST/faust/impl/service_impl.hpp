/*
 *  job_service_impl.hpp 
 *  FAUST - Framework for Adaptive Ubiquitous Scalable Tasks
 *  Website: https://macpro01.cct.lsu.edu/trac/faust
 *
 *  Created by Ole Weidner <oweidner@cct.lsu.edu> on 11/30/08.
 *  Copyright 2008-2009 Center for Computation & Technology. 
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying 
 *  LICENSE file or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef FAUST_IMPL_SERVICE_IMPL_HPP
#define FAUST_IMPL_SERVICE_IMPL_HPP

#include <map>
#include <saga/saga.hpp>
#include <boost/shared_ptr.hpp>

#include <faust/faust/object.hpp> 
#include <faust/faust/exports.hpp> 
#include <faust/faust/state.hpp>
#include <faust/faust/dependency.hpp>
#include <faust/faust/job.hpp>
#include <faust/faust/resource.hpp>


#include <faust/impl/logwriter.hpp>
#include <faust/impl/object_impl.hpp>

namespace faust
{
  namespace impl
  {
    class job; // fwd. decl.
    
    typedef std::map<std::string,  faust::object> joblist_map_t;
    typedef std::pair<std::string, faust::object> joblist_pair_t;
    
    typedef std::map<std::string,  faust::resource_description> resource_descriptions_map;
    typedef std::pair<std::string, faust::resource_description> resource_descriptions_pair;
    
    //////////////////////////////////////////////////////////////////////////
    //
    class  service_impl : public faust::impl::object
    {
      
    private:
      faust::detail::logwriter * log_;
      
      resource_descriptions_map              resource_descriptions_;
      joblist_map_t                joblist_;
      
      void insert_job_into_job_list(std::string jobid, faust::object obj);
      
    public:
      
      explicit service_impl (std::vector<faust::resource> resource_descriptions, 
                             int num_jobs);
      ~service_impl();
      
      faust::job create_job(faust::job_description job_desc);
      faust::job create_job(faust::job_description job_desc, 
                            std::string dep_job_id, 
                            faust::dependency dep);
      faust::job create_job(faust::job_description job_desc, 
                            faust::job job_obj, 
                            faust::dependency dep);
      faust::job create_job(faust::job_description job_desc, 
                            faust::job_group job_group_obj, 
                            faust::dependency dep);
      
      
      faust::job_group create_job_group(std::vector<faust::job_description> job_descs);
      faust::job_group create_job_group(std::vector<faust::job_description> job_descs, 
                                        std::string dep_job_id, 
                                        faust::dependency dep);
      faust::job_group create_job_group(std::vector<faust::job_description> job_descs, 
                                        faust::job job_obj, 
                                        faust::dependency dep);
      faust::job_group create_job_group(std::vector<faust::job_description> job_descs, 
                                        faust::job_group job_group_obj, 
                                        faust::dependency dep);        
      
      std::vector<std::string> list_jobs(void); 
      std::vector<std::string> list_resource_descriptions(void); 
      
      faust::job get_job(std::string job_id);
      faust::job_group get_job_group(std::string job_id);
      faust::resource_description get_resource_description(std::string contact);
      
    };
  }
}

#endif /* FAUST_IMPL_SERVICE_IMPL_HPP */