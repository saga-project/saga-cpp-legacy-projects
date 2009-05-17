/*
 *  monitor_group.cpp 
 *  FAUST - Framework for Adaptive Ubiquitous Scalable Tasks
 *  Website: https://macpro01.cct.lsu.edu/trac/faust
 *
 *  Created by Ole Weidner <oweidner@cct.lsu.edu> on 05/11/09.
 *  Copyright 2008-2009 Center for Computation & Technology. 
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying 
 *  LICENSE file or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include <agent/monitor/monitor_group.hpp>

using namespace faust::agent::monitor;


////////////////////////////////////////////////////////////////////////////////
//
monitor_group::monitor_group(std::string name,
                             faust::resource_description desc,
                             faust::resource_monitor mon,
                             boost::shared_ptr <faust::detail::logwriter> log_sptr)
: name_(name), description_(desc), monitor_(mon), log_sptr_(log_sptr)
{
  is_vector_group_ = false;
  vector_length_ = 0;
  last_update_ = 0;
  update_interval_ = 60; // default 1 minute
  //update_interval_c = "";
}


////////////////////////////////////////////////////////////////////////////////
//
monitor_group::~monitor_group()
{
}


////////////////////////////////////////////////////////////////////////////////
//
void monitor_group::execute()
{
  process_all_();
}


////////////////////////////////////////////////////////////////////////////////
//
void monitor_group::set_update_interval(unsigned int update_interval)
{
  update_interval_ = update_interval;
}

////////////////////////////////////////////////////////////////////////////////
//
void monitor_group::set_update_interval(char const* const rd_attrib)
{
  update_interval_c_ = std::string(rd_attrib);
}



////////////////////////////////////////////////////////////////////////////////
//
unsigned int monitor_group::get_update_interval()
{
  return update_interval_;
}


////////////////////////////////////////////////////////////////////////////////
//
void monitor_group::add_cmd_value_mapping(char const* const rd_attrib, char const* const rm_attrib)
{
  cmd_value_mappings_.push_back(mapping_t(rd_attrib, rm_attrib));
}


////////////////////////////////////////////////////////////////////////////////
//
void monitor_group::add_value_value_mapping(char const* const rd_attrib, char const* const rm_attrib)
{
  value_value_mappings_.push_back(mapping_t(rd_attrib, rm_attrib));
}


////////////////////////////////////////////////////////////////////////////////
//
bool monitor_group::validate_(std::vector<mapping_t> &vec)
{  
  SAGA_OSSTREAM msg;
  msg << "Validating monitor_group '" << name_ << "'.";
  
  if(vec.size() < 1) 
  {
    LOG_WRITE_SUCCESS_2(log_sptr_, msg);
    return true;
  }
  
  is_vector_group_ = 
  description_.attribute_is_vector((*vec.begin()).first);
  
  if(is_vector_group_) {
    vector_length_ = 
    description_.get_vector_attribute((*vec.begin()).first).size();
  }
  
  try
  {
    
    std::vector<mapping_t>::const_iterator it;
    for(it = vec.begin(); it != vec.end(); ++it)
    {
      if(!description_.attribute_exists((*it).first))
      {
        LOG_WRITE_FAILED_AND_WARN_2(log_sptr_, msg, "attribute "+(*it).first+"not defined");
        return false;
      }
      
      // either all members have to be scalar or vector attributes.
      // if they are vector attributes, they have to have the same length.
      
      if(description_.attribute_is_vector((*it).first) )
      {
        if(description_.get_vector_attribute((*it).first).size() != vector_length_)
        {
          LOG_WRITE_FAILED_AND_WARN_2(log_sptr_, msg, "group vector length doesn't match");
          return false;
        }
        
        if(!is_vector_group_)
        {
          LOG_WRITE_FAILED_AND_WARN_2(log_sptr_, msg, "attribute "+(*it).first+"is a vector attrbibute in a non-vector group");
          return false;
        }
      }
      else
      {
        if(is_vector_group_)
        {
          LOG_WRITE_FAILED_AND_WARN_2(log_sptr_, msg, "attribute "+(*it).first+"is a scalar in a vector group");
          return false;
        } 
      }
      
    }
    last_update_ = time(NULL);
    LOG_WRITE_SUCCESS_2(log_sptr_, msg);
    
  }
  catch(saga::exception const & e)
  {
    LOG_WRITE_FAILED_AND_THROW_2(log_sptr_,msg, e.what(), faust::NoSuccess);
  }
  
  return true;
}


////////////////////////////////////////////////////////////////////////////////
//
void monitor_group::process_all_()
{
  if(!validate_(cmd_value_mappings_))
  {
    return;
  }
  
  if(!validate_(value_value_mappings_))
  {
    return;
  }
  
  SAGA_OSSTREAM msg;
  msg << "Updating monitor_group '" << name_ << "'.";
  
  try 
  {
    // process command -> value mappings
    std::vector<mapping_t>::const_iterator it;
    for(it = cmd_value_mappings_.begin(); it != cmd_value_mappings_.end(); ++it)
    {
      if(is_vector_group_) 
      {
        std::vector<std::string> results;
        std::vector<std::string> commands = description_.get_vector_attribute((*it).first);
        
        for(int i=0; i < vector_length_; ++i)
        {
          boost::process::child c = run_bash_script("/bin/bash", commands.at(i));
          boost::process::pistream & out = c.get_stdout();
          std::string val; getline(out, val);
          results.push_back(val);
          boost::process::status status = c.wait();
        }
        monitor_.set_vector_attribute((*it).second, results);
      }
      else
      {
        std::string result;
        std::string command = description_.get_attribute((*it).first);    
        
        boost::process::child c = run_bash_script("/bin/bash", command);
        boost::process::pistream & out = c.get_stdout();
        getline(out, result);
        boost::process::status status = c.wait();
        
        monitor_.set_attribute((*it).second, result);
      }
    }
    
    // process value -> value mappings
    std::vector<mapping_t>::const_iterator it2;
    for(it2 = value_value_mappings_.begin(); it2 != value_value_mappings_.end(); ++it2)
    {
      if(is_vector_group_)
      {
        std::vector<std::string> results;
        std::vector<std::string> values = description_.get_vector_attribute((*it2).first);
        
        for(int i=0; i < vector_length_; ++i)
        {
          results.push_back(values.at(i));
        }
        monitor_.set_vector_attribute((*it2).second, results);
      }
      else
      {
        monitor_.set_attribute((*it2).second, 
                               description_.get_attribute((*it2).first));
      }
    }
    LOG_WRITE_SUCCESS_2(log_sptr_, msg);
  }
  catch(saga::exception const & e)
  {
    LOG_WRITE_FAILED_AND_THROW_2(log_sptr_,msg, e.what(), faust::NoSuccess);
  }
}
