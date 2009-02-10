/*
 *  job_group.hpp
 *  FAUST - Framework for Adaptive Ubiquitous Scalable Tasks
 *
 *  Created by Ole Weidner <oweidner@cct.lsu.edu> on 11/30/08.
 *  Copyright 2008-2009 Center for Computation & Technology. 
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying 
 *  LICENSE file or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef FAUST_JOB_GROUP_HPP
#define FAUST_JOB_GROUP_HPP

#include <saga/saga.hpp>
#include <boost/shared_ptr.hpp>

#include <faust/faust/object.hpp>
#include <faust/faust/exports.hpp>
#include <faust/faust/state.hpp>
#include <faust/faust/description.hpp>

namespace faust
{
  // fwd. decl. implementation class 
  ///@cond - exclude from Doxygen
  namespace impl { class job_group; class service_impl; }
  ///@endcond - exclude from Doxygen

    
    /*! \brief The %job_group provides the manageability interface to a set
     *         of jobs submitted through a %faust %service instance. A
     *         job group behaves like a single job - any method that is called
     *         will be applied to all jobs in a group. 
     *         A %job_group can't be instanciated directly, only thourgh the 
     *         service::create_job_group factory methods.
     */
    class  job_group : public faust::object
    {
      // service impl. class needs to be friend to call private c'tor 
      friend class faust::impl::service_impl;
      
    private:
      
      boost::shared_ptr <faust::impl::job_group> get_impl (void) const;
           
      job_group();
      
    public:
      
      /*! \brief Runs all jobs in this group. 
       *
       */
      void run();
      
      /*! \brief Waits for all jobs in this group to finish.
       *
       */
      bool wait(double timeout = -1.0);
      
      /*! \brief Cancels all jobs in this group.
       *
       */
      bool cancel(double timeout = -1.0);
      
      /*! \brief Suspends all jobs in this group.
       *
       */
      bool suspend();
      
      /*! \brief Resumes all jobs in this group.
       *
       */
      bool resume();
      
      /*! \brief Returns the unique ID for this %group. 
       *
       */
      std::string get_job_id();     
      
      /*! \brief Returns a list of unique IDs for all jobs in this %group. 
       *
       */
      std::vector<std::string> list_jobs();     
      
    };
}

#endif /* FAUST_JOB_GROUP_HPP */