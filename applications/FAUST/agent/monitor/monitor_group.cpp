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

  has_update_interval_mapping_ = false;
  default_update_interval_ = 60;
  default_last_update_ = 0;

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
  if(!has_update_interval_mapping_ & (default_last_update_+default_update_interval_ >= time(NULL)))
  {
    // DO NOTHING
  }
  else
  {
    process_all_();
  }
}


////////////////////////////////////////////////////////////////////////////////
//
void monitor_group::set_update_interval_mapping(char const* const rd_attrib, char const* const rm_attrib)
{
  update_interval_mapping_ = mapping_t(rd_attrib, rm_attrib);
  has_update_interval_mapping_ = true;
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
bool monitor_group::validate_(std::vector<mapping_t> &vec, SAGA_OSSTREAM & msg)
{    
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
  SAGA_OSSTREAM msg;
  msg << "Validating monitor_group '" << name_ << "'.";

  if(!validate_(cmd_value_mappings_, msg))
  {
    return;
  }
  
  if(!validate_(value_value_mappings_, msg))
  {
    return;
  }
  
  LOG_WRITE_SUCCESS_2(log_sptr_, msg);
  
  

  
  
  
  
  
  
  msg << "Updating monitor_group '" << name_ << "'.";
  
  //try 
  
    if(is_vector_group_)
    {
      std::vector<std::vector<std::string> > test;
      std::vector<std::string> last_updates;
      
      for(int i=0; i < vector_length_; ++i)
      {
        if(has_update_interval_mapping_)
        {
          update_interval = boost::lexical_cast<unsigned int>(
            description_.get_vector_attribute(update_interval_mapping_.first).at(i));
          try {
            // can be zero initially
            last_update = boost::lexical_cast<unsigned int>(
              monitor_.get_vector_attribute(update_interval_mapping_.second).at(i));
          }
          catch(std::out_of_range const & e)
          {
            last_update = 0;
          }
        }

        std::vector<std::string> results;
        std::vector<std::string> commands;
        
        if(last_update+update_interval > time(NULL))
        {
          results.push_back(monitor_.get_vector_attribute((*it).first).at(i));
          // PROCESS UPDATE_TIME VECTOR
        }
        else
        {
          std::vector<mapping_t>::const_iterator it;
          for(it = cmd_value_mappings_.begin(); it != cmd_value_mappings_.end(); ++it)
          {           
            std::string command = description_.get_vector_attribute((*it).first).at(i);
          
            boost::process::child c = run_bash_script("/bin/bash", command);
            boost::process::pistream & out = c.get_stdout();
            std::string val; getline(out, val);
            results.push_back(val);
            boost::process::status status = c.wait();
          }
          test.push_back(results);
      }
      
      int vec_pos = 0;
      std::vector<mapping_t>::const_iterator it_2;
      for(it_2 = cmd_value_mappings_.begin(); it_2 != cmd_value_mappings_.end(); ++it_2)
      {
        //std::cout << "out: " << (*it_2).second << std::endl;
        std::vector<std::string> reverse;
        
        std::vector<std::vector<std::string> >::const_iterator it_3;
        for(it_3 = test.begin(); it_3 != test.end(); ++it_3)
        {
          //std::cout << "  * " << (*it_3).at(vec_pos) << std::endl;
          reverse.push_back((*it_3).at(vec_pos));
        }
        
        monitor_.set_vector_attribute((*it_2).second, reverse);
        ++vec_pos;

      }
      
      /*for(int i=0; i < vector_length_; ++i)
      {
        std::vector<std::vector<std::string> >::const_iterator it_2;
        for(it_2 = test.begin(); it_2 != test.end(); ++it_2)
      {

          std::vector<std::string> reverse;
         // monitor_.set_vector_attribute();
        }*/
      }
    

        
        
    
    
    // process command -> value mappings
    /*std::vector<mapping_t>::const_iterator it;
    for(it = cmd_value_mappings_.begin(); it != cmd_value_mappings_.end(); ++it)
    {
      ////// VECTOR GROUP
      //
      if(is_vector_group_) 
      {
        std::vector<std::string> results;
        std::vector<std::string> commands = description_.get_vector_attribute((*it).first);
                
        std::vector<std::string> update_intervals;
        std::vector<std::string> last_updates;
        std::vector<std::string> update_times;
        
        if(has_update_interval_mapping_)
        {
          update_intervals = description_.get_vector_attribute(update_interval_mapping_.first);
          last_updates = monitor_.get_vector_attribute(update_interval_mapping_.second);
        }
        
        for(int i=0; i < vector_length_; ++i)
        {
          if(has_update_interval_mapping_) 
          {
            unsigned int update_interval_ui = 
              boost::lexical_cast<unsigned int>(update_intervals.at(i));
            unsigned int last_update_ui =
              boost::lexical_cast<unsigned int>(last_updates.at(i));
            
            if(last_update_ui+update_interval_ui > time(NULL))
            {
              // NO UPDATE NECCESSARY
              return;
            }
            else
            {
              std::cout << "UPDATE" << std::endl;
            }
          }
          
          boost::process::child c = run_bash_script("/bin/bash", commands.at(i));
          boost::process::pistream & out = c.get_stdout();
          std::string val; getline(out, val);
          results.push_back(val);
          boost::process::status status = c.wait();
          
          if(has_update_interval_mapping_)
          {
            std::string udt = boost::lexical_cast<std::string>(time(NULL));
            update_times.push_back(udt); 
          }
        }
        monitor_.set_vector_attribute((*it).second, results);
        
        if(has_update_interval_mapping_)
        {
          monitor_.set_vector_attribute(update_interval_mapping_.second, update_times);
        }
      }
      
      ////// SCALAR GROUP 
      //
      else
      {
        std::string result;
        std::string command = description_.get_attribute((*it).first);    
        
        std::string update_interval;
        std::string last_update;
        std::string update_time;
        
        if(has_update_interval_mapping_)
        {
          unsigned int update_interval_ui = 
            boost::lexical_cast<unsigned int>(description_.get_attribute(update_interval_mapping_.first));
          unsigned int last_update_ui =
            boost::lexical_cast<unsigned int>(monitor_.get_attribute(update_interval_mapping_.second));
          
          if(last_update_ui+update_interval_ui > time(NULL))
          {
            // NO UPDATE NECCESSARY
            return;
          }
        }
        
        boost::process::child c = run_bash_script("/bin/bash", command);
        boost::process::pistream & out = c.get_stdout();
        getline(out, result);
        boost::process::status status = c.wait();
        
        monitor_.set_attribute((*it).second, result);
        
        if(has_update_interval_mapping_)
        {
          monitor_.set_attribute(update_interval_mapping_.second, 
                                 boost::lexical_cast<std::string>(time(NULL)));
        }
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
  }*/
}
