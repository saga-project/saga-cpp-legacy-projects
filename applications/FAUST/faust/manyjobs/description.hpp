/*
 *  description.hpp
 *  FAUST - Framework for Adaptive Ubiquitous Scalable Tasks
 *
 *  Created by Ole Weidner on 11/22/08.
 *  Copyright 2008 Center for Computation & Technology. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying 
 *  LICENSE file or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef FAUST_MANYJOBS_DESCRIPTION_HPP
#define FAUST_MANYJOBS_DESCRIPTION_HPP

#include <saga/saga.hpp>
#include <faust/faust/defines.hpp>

namespace faust
{
  namespace manyjobs {
    
    
    /*! \brief The %description encapsulates all the attributes which define a 
     *         %job. It has no methods of its own, but implements the 
     *         saga::attributes interface.
     *
     */
    class FAUST_EXPORT description : public saga::object
    {
    public:
      
      description();
      
      ~description();
    };
  }
}

#endif /* FAUST_MANYJOBS_DESCRIPTION_HPP */
