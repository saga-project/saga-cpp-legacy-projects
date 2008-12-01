/*
 *  job.hpp
 *  FAUST - Framework for Adaptive Ubiquitous Scalable Tasks
 *
 *  Created by Ole Weidner on 11/22/08.
 *  Copyright 2008 Center for Computation & Technology. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying 
 *  LICENSE file or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef FAUST_MANYJOBS_JOB_HPP
#define FAUST_MANYJOBS_JOB_HPP

#include <boost/shared_ptr.hpp>
#include <faust/impl/manyjobs/job_impl.hpp>

#include <saga/saga.hpp>
#include <faust/faust/defines.hpp>

#include <faust/faust/manyjobs/state.hpp>
#include <faust/faust/manyjobs/service.hpp>
#include <faust/faust/manyjobs/description.hpp>



namespace faust
{
  namespace manyjobs {

    /*! \brief The %job provides the manageability interface to a %job 
     *         instance submitted through a manyjob %service instance. 
     *         It can't be instanciated directly, only thourgh the create_job
     *         factory method. 
     */
    class FAUST_EXPORT job : public saga::object
    {
    
    friend class service;
      
    private:

      typedef boost::shared_ptr<faust::impl::manyjobs::job_impl> impl_ptr;
      impl_ptr impl;
      
      job() : impl(new faust::impl::manyjobs::job_impl())
      {
      }
            
    public:
            
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
      bool resume() { return impl->resume(); }

      /*! \brief Returns this %job instance's %job ID.
       *  
       */      
      std::string get_job_id() { return impl->get_job_id(); }
      
      /*! \brief Tries to return this %job instance's state.
       *
       */
      faust::manyjobs::state get_state() { return impl->get_state(); }
      
      /*! \brief Returns this %job instance's description.
       *
       */
      faust::manyjobs::description get_description() { return impl->get_description(); }

    };
  }
}

#endif /* FAUST_MANYJOBS_JOB_HPP */
