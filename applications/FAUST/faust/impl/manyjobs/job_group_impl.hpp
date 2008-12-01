/*
 *  job_group_impl.hpp IMPLEMENTATION
 *  FAUST - Framework for Adaptive Ubiquitous Scalable Tasks
 *
 *  Created by Ole Weidner on 11/30/08.
 *  Copyright 2008 Center for Computation & Technology. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying 
 *  LICENSE file or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef FAUST_MANYJOBS_IMPL_JOB_GROUP_IMPL_HPP
#define FAUST_MANYJOBS_IMPL_JOB_GROUP_IMPL_HPP

#include <faust/manyjobs/service.hpp>
#include <faust/manyjobs/job_group.hpp>

namespace faust
{
  namespace manyjobs 
  {
    namespace impl {
      
      class FAUST_EXPORT job_group_impl : public saga::object
      {
        
      //  friend class faust::manyjobs::job_group;
      private:
        
        std::string jobid_;
        
      public:

        job_group_impl() {
          std::string jobid("manyjob://");
          jobid.append(saga::uuid().string());
          jobid_ = jobid;          
        }
        
        
        std::string get_job_id() {
          return jobid_; 
        }
        
        /*! \brief Tries to run this %job instance. 
         *
         */
        void run();
        
        /*! \brief Tries to wait for this %job instance to complete.
         *
         */
        bool wait(double timeout = -1.0);
        
        /*! \brief Tries to cancel this %job instance.
         *
         */
        bool cancel(double timeout = -1.0);
        
        /*! \brief Tries to suspend this %job instance.
         *
         */
        bool suspend();
        
        /*! \brief Tries to resume this %job instance.
         *
         */
        bool resume();
        
      };
    }
  }
}

#endif /* FAUST_MANYJOBS_IMPL_JOB_GROUP_IMPL_HPP */