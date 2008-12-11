/*
 *  dependency.hpp
 *  FAUST - Framework for Adaptive Ubiquitous Scalable Tasks
 *
 *  Created by Ole Weidner on 12/10/08.
 *  Copyright 2008 Center for Computation & Technology. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying 
 *  LICENSE file or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef FAUST_DEPENDENCY_HPP
#define FAUST_DEPENDENCY_HPP

namespace faust
{    
  /*! \brief Possible types of job dependencies. 
   */
  enum dependency
  { 
    /*! Describes a <b>data %dependency</b>. A data %dependency ocures if a 
     *  %job requires the output files of another %job or  %job_group. This 
     *  implies that the dependent job can only be scheduled when the %job 
     *  which provides its input data is in DONE state. 
     */
    Data            = 0x32,
    
     /*! Describes a <b>communication %dependency</b>. A communication 
      *  %dependency ocures if a %job wants to communicate with another
      *  %job or %job_group during runtime. This implies that all jobs have
      *  to be scheduled at the same time.
      */
    Communication   = 0x64,

  };
}

#endif /*FAUST_DEPENDENCY_HPP*/