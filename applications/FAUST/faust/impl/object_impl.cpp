/*
 *  object_iml.cpp
 *  FAUST - Framework for Adaptive Ubiquitous Scalable Tasks
 *  Website: https://macpro01.cct.lsu.edu/trac/faust
 *
 *  Created by Ole Weidner <oweidner@cct.lsu.edu> on 12/16/08.
 *  Copyright 2008-2009 Center for Computation & Technology. 
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying 
 *  LICENSE file or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include <faust/impl/object_impl.hpp>

using namespace faust::impl;

// STATIC MEMBERS ///////////////////////////////////////////////////////////
//
std::string object::faust_root_namesapce_ = "";
bool object::faust_initialized_ = false;

boost::shared_ptr <faust::detail::logwriter> object::log_sptr_ = 
  log_sptr_ = boost::shared_ptr <faust::detail::logwriter> 
    (new faust::detail::logwriter("No ID set", std::cout));;

void object::initialize_faust()
{
  if(faust_initialized_)
  {
    return;
  }
  else 
  {
    // INITIALIZE FAUST
    faust_root_namesapce_ = "advert://macpro01.cct.lsu.edu:5432//FAUST/";
    //faust_root_namesapce_ = "advert://fortytwo.cct.lsu.edu:5432//FAUST/";
    
    faust_initialized_ = true;
    
    log_sptr_ = boost::shared_ptr <faust::detail::logwriter> 
    (new faust::detail::logwriter("No ID set", std::cout));;
    
    return;
  }
}

////////////////////////////////////////////////////////////////////////////
//
object::object (faust::object::type type)
: type_(type)
{
  uuid_ = saga::uuid().string();
  initialize_faust();
}

////////////////////////////////////////////////////////////////////////////
//
faust::object::type object::get_type() const
{
  return type_;
}

///////////////////////////////////////////////////////////////////////////
//
saga::impl::attribute* object::get_attributes()
{
  // TODO throw exception  
  return NULL;
}

///////////////////////////////////////////////////////////////////////////
//
saga::impl::attribute const* object::get_attributes() const
{
  // TODO throw exception
  return NULL;
}
