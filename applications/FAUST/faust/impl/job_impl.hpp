/*
 *  job.hpp 
 *  FAUST - Framework for Adaptive Ubiquitous Scalable Tasks
 *
 *  Created by Ole Weidner <oweidner@cct.lsu.edu> on 11/30/08.
 *  Copyright 2008-2009 Center for Computation & Technology. 
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying 
 *  LICENSE file or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef FAUST_IMPL_JOB_IMPL_HPP
#define FAUST_IMPL_JOB_IMPL_HPP

#include <faust/faust/exports.hpp>
#include <faust/faust/job.hpp>
#include <faust/faust/state.hpp>
#include <faust/faust/description.hpp>

#include <faust/impl/object_impl.hpp>

namespace faust
{
  namespace impl 
  {
    //////////////////////////////////////////////////////////////////////////
    //
    class  job : public faust::impl::object
    {
      
      private:
        
        std::string        jobid_;
        faust::state       state_;
        faust::job_description description_;
        
      public:
        
        job();
        void run();
        bool wait(double timeout = -1.0);
        bool cancel(double timeout = -1.0);
        bool suspend();
        bool resume();
        std::string get_job_id();        
        faust::state get_state();
        faust::job_description get_description();        
    };
    //
    //////////////////////////////////////////////////////////////////////////
  }
}

#endif /* FAUST_IMPL_JOB_IMPL_HPP */