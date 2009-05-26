/*
 *  resource_monitor_impl.cpp 
 *  FAUST - Framework for Adaptive Ubiquitous Scalable Tasks
 *  Website: https://macpro01.cct.lsu.edu/trac/faust
 *
 *  Created by Ole Weidner <oweidner@cct.lsu.edu> on 02/13/09.
 *  Copyright 2008-2009 Center for Computation & Technology. 
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying 
 *  LICENSE file or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include <faust/faust/exception.hpp>
#include <faust/impl/resource_monitor_impl.hpp>
//#include <faust/impl/detail/serialize.hpp>

using namespace faust::impl;

////////////////////////////////////////////////////////////////////////////////
// 
void resource_monitor::write_to_db_(std::string key)
{
  SAGA_OSSTREAM strm;
  strm << "Writing resource_monitor attributes to database. " ;
  
  try 
  {
    if( key.empty() )
    {
      std::vector<std::string> attribs = attributes_.list_attributes();
      std::vector<std::string>::const_iterator it;
      for(it = attribs.begin(); it != attribs.end(); ++it)
      {
        if((*it) == "utime" || (*it) == "ctime" || (*it) == "persistent")
          continue;
        
        if(attributes_.attribute_is_vector(*it)) {
          monitor_adv_.set_vector_attribute((*it), attributes_.get_vector_attribute((*it)));
        }
        else {
          monitor_adv_.set_attribute((*it), attributes_.get_attribute((*it)));
        }
      }
    }
    else
    {
      if(attributes_.attribute_is_vector(key))
      {
        monitor_adv_.set_vector_attribute(key, attributes_.get_vector_attribute(key));
      }
      else
      {
        monitor_adv_.set_attribute(key, attributes_.get_attribute(key));
      }
    }
    LOG_WRITE_SUCCESS_2(get_log(),strm);
  }
  catch(saga::exception const & e) 
  {
    LOG_WRITE_FAILED_AND_THROW_2(get_log(), strm, e.what(), faust::NoSuccess);
  }    
}

////////////////////////////////////////////////////////////////////////////////
// 
void resource_monitor::read_from_db_(std::string key)
{
  SAGA_OSSTREAM strm;
  strm << "Reading resource_monitor attributes from database. ";
  
  try 
  {
    if( key.empty() )
    {
      std::vector<std::string> attribs = monitor_adv_.list_attributes();
      std::vector<std::string>::const_iterator it;
      for(it = attribs.begin(); it != attribs.end(); ++it)
      {
        if((*it) == "utime" || (*it) == "ctime" || (*it) == "persistent")
          continue;
        
        if(monitor_adv_.attribute_is_vector(*it)) {
          attributes_.set_vector_attribute((*it), monitor_adv_.get_vector_attribute((*it)));
        }
        else {
          attributes_.set_attribute((*it), monitor_adv_.get_attribute((*it)));
        }
      }
    }
    else
    {
      if(monitor_adv_.attribute_is_vector(key))
      {
        attributes_.set_vector_attribute(key, monitor_adv_.get_vector_attribute(key));
      }
      else
      {
        attributes_.set_attribute(key, monitor_adv_.get_attribute(key));
      }
    }
    LOG_WRITE_SUCCESS_2(get_log(),strm);
  }
  catch(saga::exception const & e) 
  {
    LOG_WRITE_FAILED_AND_THROW_2(get_log(), strm, e.what(), faust::NoSuccess);
  }  
}


////////////////////////////////////////////////////////////////////////////////
// 
resource_monitor::resource_monitor() : object(faust::object::ResourceMonitor)
{
  
}


////////////////////////////////////////////////////////////////////////////////
// 
resource_monitor::resource_monitor(saga::advert::entry & monitor_adv)

: object(faust::object::ResourceMonitor),  monitor_adv_(monitor_adv)
{
  
}


////////////////////////////////////////////////////////////////////////////////
// 
void resource_monitor::read_attributes(std::string key) 
{
  // update all values
  this->read_from_db_(key);
}

////////////////////////////////////////////////////////////////////////////////
// 
void resource_monitor::write_attributes(std::string key) 
{
  // update all values
  this->write_to_db_(key);
}
