/*
 *  logwriter.hpp
 *  FAUST - Framework for Adaptive Ubiquitous Scalable Tasks
 *  Website: https://macpro01.cct.lsu.edu/trac/faust
 *
 *  Created by Ole Weidner <oweidner@cct.lsu.edu> on 11/23/08.
 *  Copyright 2008-2009 Center for Computation & Technology. 
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying 
 *  LICENSE file or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef FAUST_LOGWRITER_HPP
#define FAUST_LOGWRITER_HPP

#define LOG_WRITE_SUCCESS_2(L, S)      { S << "SUCCESS"; L->write(SAGA_OSSTREAM_GETSTRING(S), LOGLEVEL_INFO); S.str(""); }
#define LOG_WRITE_FAILED_AND_THROW_2(L, S, M, E) { S << "FAILED: " << M; L->write(SAGA_OSSTREAM_GETSTRING(S), LOGLEVEL_ERROR); throw faust::exception (SAGA_OSSTREAM_GETSTRING(S), E); S.str("");}
#define LOG_WRITE_FAILED_AND_WARN_2(L, S, M) { S << "FAILED: " << M; L->write(SAGA_OSSTREAM_GETSTRING(S), LOGLEVEL_WARNING); S.str("");}


#define LOG_WRITE_SUCCESS(S)      { S << "SUCCESS"; log_sptr_->write(SAGA_OSSTREAM_GETSTRING(S), LOGLEVEL_INFO); S.str(""); }
#define LOG_WRITE_FAILED_AND_THROW(S, M, E) { S << "FAILED: " << M; log_sptr_->write(SAGA_OSSTREAM_GETSTRING(S), LOGLEVEL_ERROR); throw faust::exception (SAGA_OSSTREAM_GETSTRING(S), E); S.str("");}

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
      
      void setIdentifier(std::string identifier) {appName_ = identifier;};
      
      ~logwriter();
      
      void write(std::string, unsigned int logLevel);
      
    };
    //
    //////////////////////////////////////////////////////////////////////////    
  } 
} 

#endif /* FAUST_LOGWRITER_HPP */

