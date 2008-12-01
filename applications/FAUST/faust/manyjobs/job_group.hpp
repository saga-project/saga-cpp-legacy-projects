/*
 *  job_group.hpp
 *  FAUST - Framework for Adaptive Ubiquitous Scalable Tasks
 *
 *  Created by Ole Weidner on 11/30/08.
 *  Copyright 2008 Center for Computation & Technology. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying 
 *  LICENSE file or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef FAUST_MANYJOBS_JOB_GROUP_HPP
#define FAUST_MANYJOBS_JOB_GROUP_HPP

#include <boost/shared_ptr.hpp>
#include <faust/impl/manyjobs/job_group_impl.hpp>

namespace faust
{
  namespace manyjobs {
    
    class FAUST_EXPORT job_group : public saga::object
    {
      
      friend class service;
      
    private:
      
      typedef boost::shared_ptr<faust::manyjobs::impl::job_group_impl> impl_ptr;
      impl_ptr impl;
            
      job_group() : impl(new faust::manyjobs::impl::job_group_impl())
      {
      }
      
    public:
       
      std::string get_job_id() { return impl->get_job_id(); }
      
      /*! \brief Tries to run this %job instance. 
       *
       */
      void run() { impl->run(); }
      
      /*! \brief Tries to wait for this %job instance to complete.
       *
       */
      bool wait(double timeout = -1.0) { return impl->wait(timeout); }
      
      /*! \brief Tries to cancel this %job instance.
       *
       */
      bool cancel(double timeout = -1.0) { return impl->cancel(timeout); }
      
      /*! \brief Tries to suspend this %job instance.
       *
       */
      bool suspend() { return impl->suspend(); }
      
      /*! \brief Tries to resume this %job instance.
       *
       */
      bool resume() { return impl->suspend(); }
      
    };
  }
}

#endif /* FAUST_MANYJOBS_JOB_GROUP_HPP */