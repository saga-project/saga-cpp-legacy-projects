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

#include <saga/saga.hpp>
#include <boost/shared_ptr.hpp>

#include <faust/faust/exports.hpp>
#include <faust/faust/manyjobs/state.hpp>
#include <faust/faust/manyjobs/description.hpp>

namespace faust
{
  // fwd. decl. implementation class 
  namespace impl { namespace manyjobs { class job_group_impl; class service_impl; } }

  namespace manyjobs {
    
    class FAUST_EXPORT job_group : public saga::object
    {
      // service impl. class needs to be friend to call private c'tor 
      friend class faust::impl::manyjobs::service_impl;
      
    private:
      
      typedef boost::shared_ptr<faust::impl::manyjobs::job_group_impl> impl_ptr;
      impl_ptr impl;
           
      job_group();
      
    public:
      
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
      
      /*! \brief Returns the unique ID for this %job_group. 
       *
       */
      std::string get_job_id();     
      
    };
  }
}

#endif /* FAUST_MANYJOBS_JOB_GROUP_HPP */