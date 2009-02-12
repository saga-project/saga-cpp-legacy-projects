/*
 *  job.hpp
 *  FAUST - Framework for Adaptive Ubiquitous Scalable Tasks
 *  Website: https://macpro01.cct.lsu.edu/trac/faust
 *
 *  Created by Ole Weidner <oweidner@cct.lsu.edu> on 11/22/08.
 *  Copyright 2008-2009 Center for Computation & Technology. 
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying 
 *  LICENSE file or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef FAUST_JOB_HPP
#define FAUST_JOB_HPP

#include <boost/shared_ptr.hpp>

#include <faust/faust/object.hpp>
#include <faust/faust/exports.hpp>
#include <faust/faust/state.hpp>
#include <faust/faust/description.hpp>

namespace faust
{
  // fwd. decl. implementation class // 
  ///@cond - exclude from Doxygen
  namespace impl { class job; class service_impl; } 
  ///@endcond - exclude from Doxygen


    /*! \brief The %job provides the manageability interface to a %job 
     *         instance submitted through a %faust %service instance. 
     *         It can't be instanciated directly, only thourgh the 
     *         service::create_job factory methods.
     */
    class  job : public faust::object
    {
    
      // service impl. class needs to be friend to call private c'tor 
      friend class faust::impl::service_impl;
      
    private:
      
      job();
      
      boost::shared_ptr <faust::impl::job> get_impl (void) const;
            
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

      /*! \brief Returns this %job instance's %job ID.
       *  
       */      
      std::string get_job_id();
      
      /*! \brief Tries to return this %job instance's state.
       *
       */
      faust::state get_state();
      
      /*! \brief Returns this %job instance's description.
       *
       */
      faust::job_description get_description();

    };
}

#endif /* FAUST_JOB_HPP */
