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

#include <saga/saga.hpp>

#include <faust/faust/exports.hpp>
#include <faust/impl/logwriter.hpp>
#include <faust/impl/manyjobs/job_impl.hpp>

namespace faust
{
  namespace impl
  {
    namespace manyjobs 
    {
      struct resource {
        saga::url    contact;
        saga::url    workdir;
        std::string  queue;
        std::string  project;
      };
      
      class job; // fwd. decl.

      typedef boost::shared_ptr<job> job_ptr;
      typedef std::map<std::string,  job_ptr> joblist_map;
      typedef std::pair<std::string, job_ptr> joblist_pair;
      
      typedef std::map<std::string,  resource> resources_map;
      typedef std::pair<std::string, resource> resources_pair;
            
      class FAUST_EXPORT service_impl : public saga::object
      {

      private:
        faust::detail::logwriter * log_;
        
        resources_map              resources_;
        joblist_map                joblist_;
        
        
      public:
        
        explicit service_impl (std::vector<resource> resources, int num_jobs);
        
      };
    }
  }
}

#endif /* FAUST_IMPL_MANYJOBS_SERVICE_IMPL_HPP */