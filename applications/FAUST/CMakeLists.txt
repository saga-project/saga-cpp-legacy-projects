#  CMakeLists.txt
#  FAUST - Framework for Adaptive Ubiquitous Scalable Tasks
#
#  Created by Ole Weidner <oweidner@cct.lsu.edu> on 01/16/09.
#  Copyright 2009 Center for Computation & Technology. All rights reserved.
#
#  Distributed under the Boost Software License, Version 1.0. (See accompanying 
#  LICENSE file or copy at http://www.boost.org/LICENSE_1_0.txt)

# We require at least CMake V2.6.2
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)

# Allow more human readable "if then else" constructs
set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS TRUE)

# Project name
project(FAUST CXX C)

# Project version
set(FAUST_MAJOR_VERSION 0)
set(FAUST_MINOR_VERSION 1)
set(FAUST_PATCH_LEVEL   0)
set(FAUST_VERSION "${FAUST_MAJOR_VERSION}.${FAUST_MINOR_VERSION}.${FAUST_PATCH_LEVEL}")
set(FAUST_SOVERSION ${FAUST_MAJOR_VERSION})

# We have a patched BoostFIND modulelook for it in the misc directory
list(APPEND CMAKE_MODULE_PATH ${FAUST_SOURCE_DIR}/misc)

# set default cmake build type (None Debug Release RelWithDebInfo MinSizeRel)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "RelWithDebInfo")
endif()

# We need to use Boost >= 1.35, namely the following Boost libraries
set(Boost_FIND_VERSION_EXACT OFF)
set(Boost_FIND_VERSION_MAJOR 1)
set(Boost_FIND_VERSION_MINOR 35)
set(Boost_FIND_VERSION_PATCH 0)

set(Boost_COMPONENTS_NEEDED
    date_time
    program_options
    system
    thread)

#set(BOOST_USE_STATIC_LIBS OFF)

find_package(Boost 1.35 COMPONENTS ${Boost_COMPONENTS_NEEDED})

if(Boost_FOUND)
    message(STATUS "Boost version found: " 
        ${Boost_MAJOR_VERSION}.${Boost_MINOR_VERSION}.${Boost_SUBMINOR_VERSION}
        " (${Boost_INCLUDE_DIR})")
else()
    message("FATAL Boost installation not found. Please specify the Boost
            installation directory using -DBOOST_ROOT=...")
endif()

include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIR})

# initialize installation directory (can be changed via ccmake or 
# 'cmake -DCMAKE_PREFIX=...')
if(UNIX)
    SET(CMAKE_PREFIX "/usr/local/" 
        CACHE PATH "Prefix prepended to install directories")
endif()

# force some variables that could be defined in the command line to be written 
# to cache
set(CMAKE_INSTALL_PREFIX "${CMAKE_PREFIX}" 
    CACHE PATH "Where to install ${PROJECT_NAME}" 
    FORCE)
mark_as_advanced(CMAKE_INSTALL_PREFIX)
set(CMAKE_BUILD_TYPE "${CMAKE_BUILD_TYPE}" 
    CACHE STRING "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel." 
    FORCE)
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" 
    CACHE PATH "Path to custom CMake Modules" 
    FORCE)

message(STATUS "FAUST will be installed to: " ${CMAKE_INSTALL_PREFIX})

# global library configuration
add_definitions(-DFAUST_PREFIX=\"${CMAKE_INSTALL_PREFIX}\")
add_definitions(-DFAUST_USE_LOCKFREE=1)
add_definitions(-DFAUST_USE_TBB=0)

# the Boost serialization library needs to be linked as a shared library
add_definitions(-DBOOST_SERIALIZATION_DYN_LINK)
add_definitions(-DBOOST_ARCHIVE_DYN_LINK)

# all other Boost libraries don't need to be loaded as shared libraries (but 
# it's easier configuration wise to do so)
add_definitions(-DBOOST_FILESYSTEM_DYN_LINK)
add_definitions(-DBOOST_DATE_TIME_DYN_LINK)
add_definitions(-DBOOST_PROGRAM_OPTIONS_DYN_LINK)
add_definitions(-DBOOST_REGEX_DYN_LINK)
add_definitions(-DBOOST_SYSTEM_DYN_LINK)
add_definitions(-DBOOST_SIGNALS_DYN_LINK)
add_definitions(-DBOOST_THREAD_DYN_DLL)

# compiler/platform specific configuration
if(MSVC)
    add_definitions(-D_WINDOWS)
    add_definitions(-DBOOST_USE_WINDOWS_H)
    add_definitions(-D_WIN32_WINNT=0x0501)
    add_definitions(-D_SCL_SECURE_NO_WARNINGS)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    add_definitions(-D_SCL_SECURE_NO_DEPRECATE)
    add_definitions(-D_CRT_SECURE_NO_DEPRECATE)
    add_definitions(-D_CRT_NONSTDC_NO_WARNINGS)
    # suppress certain warnings
    add_definitions(-wd4251 -wd4231 -wd4275 -wd4660 -wd4094 -wd4267)
    if(CMAKE_CL_64)
        add_definitions(-DBOOST_COROUTINE_USE_FIBERS)
    endif()
endif()

if(CMAKE_COMPILER_IS_GNUCXX)
    add_definitions(-DFAUST_GCC_HAVE_VISIBILITY)
    add_definitions(-DBOOST_COROUTINE_GCC_HAVE_VISIBILITY)

    # check for availability of pthread_setaffinity_np()
    include(CheckSymbolExists)
    check_symbol_exists(pthread_setaffinity_np "pthread.h"
        HAVE_PTHREAD_SETAFFINITY_NP)
    if(HAVE_PTHREAD_SETAFFINITY_NP)
        add_definition(-DHAVE_PTHREAD_SETAFFINITY_NP)
    endif()
endif()

# additional preprocessor definitions
add_definitions(-DBOOST_COROUTINE_USE_ATOMIC_COUNT)
add_definitions(-DBOOST_COROUTINE_ARG_MAX=2)

# Mac systems headers have some dependency on _XOPEN_SOURCE being defined
# let's better be on the safe side...
if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    add_definitions(-D_XOPEN_SOURCE=1)
endif()

# FAUST_LIBRARIES lists all libraries a FAUST module needs to be linked with by 
# default, there may be more for individual modules, but this is the mandatory
# set
set(FAUST_LIBRARIES FAUST FAUST_serialization)
if(UNIX)
    set(FAUST_LIBRARIES ${FAUST_LIBRARIES} dl)
endif()

# set variable helping in naming the target library names
if(MSVC)
    set(CMAKE_DEBUG_POSTFIX "d")
endif()
if(UNIX)
    set(component_LIBRARY_PREFIX "FAUST_component_")
endif()

# Recurse into some subdirectories. This does not actually cause another cmake 
# executable to run. The same process will walk through the project's entire 
# directory structure.
add_subdirectory(${FAUST_SOURCE_DIR}/faust)
add_subdirectory(${FAUST_SOURCE_DIR}/agent)



# Components are to build separately
#add_subdirectory(FAUST/components)

# executables depend on libraries above
#add_subdirectory(tests)
#add_subdirectory(examples)
#add_subdirectory(runtime)

# installation instructions
install(
    DIRECTORY FAUST/                    # install all FAUST header files
    DESTINATION include/FAUST 
    FILES_MATCHING PATTERN "*.hpp"
    REGEX ".svn" EXCLUDE
    REGEX "CMakeFiles" EXCLUDE)

# install external dependencies
#install(
#    DIRECTORY external/cache/boost
#    DESTINATION include
#    FILES_MATCHING PATTERN "*.hpp"
#    REGEX ".svn" EXCLUDE)
#install(
#    DIRECTORY external/coroutine/boost
#    DESTINATION include
#    FILES_MATCHING PATTERN "*.hpp"
#    REGEX ".svn" EXCLUDE)
#install(
#    DIRECTORY external/endian/boost
#    DESTINATION include
#    FILES_MATCHING PATTERN "*.hpp"
#    REGEX ".svn" EXCLUDE)
#if(${Boost_MINOR_VERSION} LESS 36)
#    install(
#        DIRECTORY external/exception/boost
#        DESTINATION include
#        FILES_MATCHING PATTERN "*.hpp"
#        REGEX ".svn" EXCLUDE)
#endif()
#install(
#    DIRECTORY external/lockfree/boost
#    DESTINATION include
#    FILES_MATCHING PATTERN "*.hpp"
#    REGEX ".svn" EXCLUDE)
#install(
#    DIRECTORY external/logging/boost
#    DESTINATION include
#    FILES_MATCHING PATTERN "*.hpp"
#   REGEX ".svn" EXCLUDE)
#install(
#    DIRECTORY external/plugin/boost
#    DESTINATION include
#    FILES_MATCHING PATTERN "*.hpp"
#    REGEX ".svn" EXCLUDE)

# export build settings
include(CMakeExportBuildSettings)
cmake_export_build_settings("${PROJECT_NAME}BuildSettings.cmake")

# export library dependencies (keep this as the last line in the file)
export_library_dependencies("${PROJECT_NAME}LibDeps.cmake")
