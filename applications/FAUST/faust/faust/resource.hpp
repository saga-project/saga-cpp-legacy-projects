/*
 *  resource.hpp
 *  FAUST - Framework for Adaptive Ubiquitous Scalable Tasks
 *
 *  Created by Ole Weidner <oweidner@cct.lsu.edu> on 11/22/08.
 *  Copyright 2008 Center for Computation & Technology. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying 
 *  LICENSE file or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef FAUST_RESOURCE_HPP
#define FAUST_RESOURCE_HPP

namespace faust
{      
    /*! \brief This structure defines a computing %resource. It contains attributes
     *         like a contact string, queue and project names. A list of 
     *         resources is used to create a %faust %service instance.
     *  
     */
    struct resource {
      saga::url    contact;
      saga::url    workdir;
      std::string  queue;
      std::string  project;
    };
}

#endif /* FAUST_RESOURCE_HPP */