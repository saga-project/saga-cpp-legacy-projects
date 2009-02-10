/*
 *  exports.hpp
 *  FAUST - Framework for Adaptive Ubiquitous Scalable Tasks
 *
 *  Created by Ole Weidner <oweidner@cct.lsu.edu> on 11/22/08.
 *  Copyright 2008-2009 Center for Computation & Technology. 
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying 
 *  LICENSE file or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef FAUST_EXPORTS_HPP
#define FAUST_EXPORTS_HPP

#if !defined (FAUST_CREATE_PREPROCESSED_FILES)

# if defined(FAUST_WINDOWS)
#   define FAUST_SYMBOL_EXPORT   __declspec (dllexport)
#   define FAUST_SYMBOL_IMPORT   __declspec (dllimport)
#   define FAUST_SYMBOL_INTERNAL /* empty */
# elif defined(FAUST_GCC_HAVE_VISIBILITY)
#   define FAUST_SYMBOL_EXPORT   __attribute__((visibility("default")))
#   define FAUST_SYMBOL_IMPORT   __attribute__((visibility("default")))
#   define FAUST_SYMBOL_INTERNAL __attribute__((visibility("hidden")))
# else
#   define FAUST_SYMBOL_EXPORT   /* empty */
#   define FAUST_SYMBOL_IMPORT   /* empty */
#   define FAUST_SYMBOL_INTERNAL /* empty */
# endif

///////////////////////////////////////////////////////////////////////////////
// define the export/import helper macros

# ifdef     FAUST_ENGINE_EXPORTS
#   define  FAUST_EXPORT             FAUST_SYMBOL_EXPORT
#   define  FAUST_EXCEPTION_EXPORT   FAUST_SYMBOL_EXPORT
# elif !defined(BUILD_FAUST_LITE)
#   define  FAUST_EXPORT             FAUST_SYMBOL_IMPORT
#   define  FAUST_EXCEPTION_EXPORT   FAUST_SYMBOL_IMPORT
# else
#   define  FAUST_EXPORT             /* empty */
#   define  FAUST_EXCEPTION_EXPORT   /* empty */
# endif

# ifdef     FAUST_PACKAGE_EXPORTS
#   define  FAUST_PACKAGE_EXPORT     FAUST_SYMBOL_EXPORT
#   define  FAUST_PACKAGE_LOCAL      FAUST_SYMBOL_INTERNAL
# elif !defined(BUILD_FAUST_LITE)
#   define  FAUST_PACKAGE_EXPORT     FAUST_SYMBOL_IMPORT
#   define  FAUST_PACKAGE_LOCAL      FAUST_SYMBOL_INTERNAL
# else
#   define  FAUST_PACKAGE_EXPORT     /* empty */
#   define  FAUST_PACKAGE_LOCAL      /* empty */
# endif

# ifdef     FAUST_ADAPTOR_EXPORTS
#   define  FAUST_ADAPTOR_EXPORT     FAUST_SYMBOL_EXPORT
#   define  FAUST_ADAPTOR_LOCAL      FAUST_SYMBOL_INTERNAL
# elif !defined(BUILD_FAUST_LITE)
#   define  FAUST_ADAPTOR_EXPORT     FAUST_SYMBOL_IMPORT
#   define  FAUST_ADAPTOR_LOCAL      FAUST_SYMBOL_INTERNAL
# else
#   define  FAUST_ADAPTOR_EXPORT     /* empty */
#   define  FAUST_ADAPTOR_LOCAL      /* empty */
# endif

# define    FAUST_LOCAL              /* empty */

#endif

#endif /* FAUST_EXPORTS_HPP */
