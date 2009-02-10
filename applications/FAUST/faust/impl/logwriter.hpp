/*
 *  logwriter.hpp
 *  FAUST - Framework for Adaptive Ubiquitous Scalable Tasks
 *
 *  Created by Ole Weidner <oweidner@cct.lsu.edu> on 11/23/08.
 *  Copyright 2008-2009 Center for Computation & Technology. 
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying 
 *  LICENSE file or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef FAUST_LOGWRITER_HPP
#define FAUST_LOGWRITER_HPP

#define LOGLEVEL_FATAL     0x2   
#define LOGLEVEL_ERROR     0x4  
#define LOGLEVEL_WARNING   0x8   
#define LOGLEVEL_INFO      0x16   
#define LOGLEVEL_DEBUG     0x32  

#include <string>

namespace faust 
{ 
  namespace detail 
  {
    //////////////////////////////////////////////////////////////////////////
    //
    class logwriter {
      
    private:
      
      std::string appName_;
      std::ostream & out_;
      
    public:
      
      logwriter();
      logwriter(std::string appName);
      logwriter(std::string appName, std::ostream& logStream);
      
      ~logwriter();
      
      void write(std::string, unsigned int logLevel);
      
    };
    //
    //////////////////////////////////////////////////////////////////////////    
  } 
} 

#endif /* FAUST_LOGWRITER_HPP */

