/*
 *  state.hpp
 *  FAUST - Framework for Adaptive Ubiquitous Scalable Tasks
 *
 *  Created by Ole Weidner <oweidner@cct.lsu.edu> on 11/29/08.
 *  Copyright 2008 Center for Computation & Technology. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying 
 *  LICENSE file or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef FAUST_STATE_HPP
#define FAUST_STATE_HPP

namespace faust
{    
    /*! \brief Possible states for a %faust %job. Currently it adopts the saga
     *         job model states but it might me extended/altered at some point.
     */
    enum state
    { /*! Add brief description! */
      Unknown   = -1,
      /*! Add brief description! */
      New       =  1,
      /*! Add brief description! */
      Pending   =  2,
      /*! Add brief description! */
      Running   =  4,
      /*! Add brief description! */
      Done      =  8,
      /*! Add brief description! */
      Canceled  =  16,
      /*! Add brief description! */
      Failed    =  32,
      /*! Add brief description! */
      Suspended =  64
    };
}

#endif /*FAUST_STATE_HPP*/