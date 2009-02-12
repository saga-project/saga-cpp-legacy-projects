/*
 *  version.hpp
 *  FAUST - Framework for Adaptive Ubiquitous Scalable Tasks
 *  Website: https://macpro01.cct.lsu.edu/trac/faust
 *
 *  Created by Ole Weidner <oweidner@cct.lsu.edu> on 11/22/08.
 *  Copyright 2008-2009 Center for Computation & Technology. 
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying 
 *  LICENSE file or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef FAUST_VERSION_HPP
#define FAUST_VERSION_HPP

#include <faust/faust/defines.hpp>
#include <boost/preprocessor/cat.hpp>

///////////////////////////////////////////////////////////////////////////////
//  The version of the FAUST engine
//
//  FAUST_VERSION_FULL & 0x0000FF is the sub-minor version
//  FAUST_VERSION_FULL & 0x00FF00 is the minor version
//  FAUST_VERSION_FULL & 0xFF0000 is the major version
//
//
#define FAUST_VERSION_FULL         0x000100

#define FAUST_VERSION_MAJOR        0
#define FAUST_VERSION_MINOR        1
#define FAUST_VERSION_SUBMINOR     0

#define FAUST_VERSION_DATE         081201

#define FAUST_VERSION_SUBMINORMASK 0xFF

// The version check enforces the major and minor version numbers to match for
// every compilation unit to be compiled.
///@cond - exclude from Doxygen
#define FAUST_CHECK_VERSION                                 \
BOOST_PP_CAT(FAUST_check_version_,                          \
BOOST_PP_CAT(FAUST_VERSION_MAJOR,                           \
BOOST_PP_CAT(_, FAUST_VERSION_MINOR)))                      \
///@endcond - exclude from Doxygen

///////////////////////////////////////////////////////////////////////////////
//
//  FAUST_VERSION_ISCOMPATIBLE(), FAUST_VERSION_ISCOMPATIBLE_EX()
// 
//  The FAUST_VERSION_ISCOMPATIBLE macros test, if the version of the loaded
//  FAUST engine is compatible with the version the calling application was 
//  compiled against.
//
#define FAUST_VERSION_ISCOMPATIBLE_EX(version)              \
(((version) & ~FAUST_VERSION_SUBMINORMASK) >=               \
(FAUST_VERSION_FULL & ~FAUST_VERSION_SUBMINORMASK))         \

#define FAUST_VERSION_ISCOMPATIBLE()                        \
FAUST_VERSION_ISCOMPATIBLE_EX(FAUST::get_engine_version())  \


namespace faust 
{
  /*! \brief Brief %description starts here     
   *
   *
   */
   unsigned long get_faust_version (void);
  
  ///@cond - exclude from Doxygen
  extern char const FAUST_CHECK_VERSION[];
  ///@endcond - exclude from Doxygen
  
} 

///////////////////////////////////////////////////////////////////////////////
// This is instantiated per translation unit, forcing to resolve the variable
// FAUST_CHECK_VERSION
namespace
{
  char const* check_faust_version()
  {
    return faust::FAUST_CHECK_VERSION;
  }
}

#endif /* FAUST_VERSION_HPP */
