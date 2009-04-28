/*
 *  directories.hpp 
 *  FAUST - Framework for Adaptive Ubiquitous Scalable Tasks
 *  Website: https://macpro01.cct.lsu.edu/trac/faust
 *
 *  Created by Ole Weidner <oweidner@cct.lsu.edu> on 04/29/09.
 *  Copyright 2008-2009 Center for Computation & Technology. 
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying 
 *  LICENSE file or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef FAUST_AGENT_MONITOR_DIRECTORIES_HPP
#define FAUST_AGENT_MONITOR_DIRECTORIES_HPP

//#include <saga/saga.hpp>
//#include <faust/faust/resource_description.hpp>

#include <faust/impl/logwriter.hpp>
#include <faust/faust/resource_description.hpp>
#include <faust/faust/resource_monitor.hpp>

////////////////////////////////////////////////////////////////////////////
//
namespace faust
{
  //////////////////////////////////////////////////////////////////////////
  //
  namespace agent { namespace monitor {
    
    class directories {
      
    private:
      boost::shared_ptr <faust::resource_description> rd_;
      boost::shared_ptr <faust::resource_monitor>     rm_;
      
      faust::detail::logwriter * log_;
      
      bool attributes_checked_;
      bool attributes_ok_;
      bool check_attributes();
      
    public:
      
      directories (faust::resource_description & rd, 
                    faust::resource_monitor     & rm);
      ~directories( );
    };
    
  }}
  //
  //////////////////////////////////////////////////////////////////////////
}

#endif /* FAUST_AGENT_MONITOR_DIRECTORIES_HPP */
