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
bool monitor_group::execute()
{
  if(!has_update_interval_mapping_ & (default_last_update_+default_update_interval_ >= time(NULL)))
  {
    // DO NOTHING
    return false;
  }
  else
  {
    return process_all_();
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
bool monitor_group::process_all_()
{
  bool global_update = false;
  
  SAGA_OSSTREAM msg;
  msg << "Validating monitor_group '" << name_ << "'.";
  
  if(!validate_(cmd_value_mappings_, msg))
  {
    return false;
  }
  
  if(!validate_(value_value_mappings_, msg))
  {
    return false;
  }
  
  msg.str("");
  //LOG_WRITE_SUCCESS_2(log_sptr_, msg);
    
  try 
  {
    ////// VECTOR GROUP PROCESSING
    //
    if(is_vector_group_)
    {
      std::vector<std::vector<std::string> > cmd_val_map_results;
      std::vector<std::vector<std::string> > val_val_map_results;
      std::vector<std::string> update_intervals;
      std::vector<std::string> last_updates;
      
      if(has_update_interval_mapping_)
      {
        update_intervals = description_.get_vector_attribute(update_interval_mapping_.first);
        last_updates = monitor_.get_vector_attribute(update_interval_mapping_.second);
        
        // update fields might not be properly initialized in the first pass
        for(int i=0; i < vector_length_; ++i)
        {
          try { last_updates.at(i); }
          catch (std::out_of_range const & e) { last_updates.push_back("0"); }
        }
      }
      
      for(int i=0; i < vector_length_; ++i)
      {
        bool needs_update = true;
        
        // check if this set needs to be updates
        if(has_update_interval_mapping_)
        {
          unsigned int lu = boost::lexical_cast<unsigned int>(last_updates.at(i));
          unsigned int ui = boost::lexical_cast<unsigned int>(update_intervals.at(i));
          
          if( lu + ui >= time(NULL) )
          {
            needs_update = false;
          }
          else
          {
            needs_update = true;
          }
        }
        
        if(needs_update) 
        {
          msg << "Updating (vector) monitor_group '" << name_ << ":"<< i <<"'.";

          ////// CMD -> VALUE MAPPINGS
          //
          std::vector<std::string> cmd_val_results;
          std::vector<mapping_t>::const_iterator cmd_val_it;
          for(cmd_val_it = cmd_value_mappings_.begin(); cmd_val_it != cmd_value_mappings_.end(); ++cmd_val_it)
          {           
            std::string command = description_.get_vector_attribute((*cmd_val_it).first).at(i);
            
            boost::process::child c = run_bash_script("/bin/bash", command);
            boost::process::pistream & out = c.get_stdout();
            std::string val; getline(out, val);
            cmd_val_results.push_back(val);
            boost::process::status status = c.wait();
          }
          cmd_val_map_results.push_back(cmd_val_results);
          
          ////// VALUE -> VALUE MAPPINGS
          //
          std::vector<std::string> val_val_results;
          std::vector<mapping_t>::const_iterator v_v_it;
          for(v_v_it = value_value_mappings_.begin(); v_v_it != value_value_mappings_.end(); ++v_v_it)
          {           
            std::string val = description_.get_vector_attribute((*v_v_it).first).at(i);
            
            val_val_results.push_back(val);
          }
          val_val_map_results.push_back(val_val_results);
          
          if(has_update_interval_mapping_)
          {
            last_updates.at(i) = boost::lexical_cast<std::string>(time(NULL));
          }
          
          LOG_WRITE_SUCCESS_2(log_sptr_, msg);
          global_update = true;
        }
        else // no update needed. copy old values
        {
          std::vector<std::string> cmd_val_results;
          std::vector<mapping_t>::const_iterator cmd_val_it;
          for(cmd_val_it = cmd_value_mappings_.begin(); cmd_val_it != cmd_value_mappings_.end(); ++cmd_val_it)
          {  
            cmd_val_results.push_back(monitor_.get_vector_attribute((*cmd_val_it).second).at(i));
          }
          cmd_val_map_results.push_back(cmd_val_results);
          
          std::vector<std::string> val_val_results;
          std::vector<mapping_t>::const_iterator v_v_it;
          for(v_v_it = value_value_mappings_.begin(); v_v_it != value_value_mappings_.end(); ++v_v_it)
          {  
            val_val_results.push_back(monitor_.get_vector_attribute((*v_v_it).second).at(i));
          }
          val_val_map_results.push_back(val_val_results);
        }
        
      } 
      
      ////// SET UPDATE TIMESTAMP IF SUPPROTED
      //
      if(has_update_interval_mapping_)
      {
        monitor_.set_vector_attribute(update_interval_mapping_.second, last_updates);
      }
      
      ////// REVERSE CMD -> VALUE RESULTS
      //
      int vec_pos = 0;
      std::vector<mapping_t>::const_iterator it_2;
      for(it_2 = cmd_value_mappings_.begin(); it_2 != cmd_value_mappings_.end(); ++it_2)
      {
        std::vector<std::string> reverse;
        std::vector<std::vector<std::string> >::const_iterator it_3;
        for(it_3 = cmd_val_map_results.begin(); it_3 != cmd_val_map_results.end(); ++it_3)
        {
          reverse.push_back((*it_3).at(vec_pos));
        }
        
        // write values to monitor if update required
        monitor_.set_vector_attribute((*it_2).second, reverse);
        ++vec_pos;
      }
      
      ////// REVERSE VALUE -> VALUE RESULTS
      //
      vec_pos = 0;
      std::vector<mapping_t>::const_iterator it_3;
      for(it_3 = value_value_mappings_.begin(); it_3 != value_value_mappings_.end(); ++it_3)
      {
        std::vector<std::string> reverse;
        std::vector<std::vector<std::string> >::const_iterator it_4;
        for(it_4 = val_val_map_results.begin(); it_4 != val_val_map_results.end(); ++it_4)
        {
          reverse.push_back((*it_4).at(vec_pos));
        }
        
        // write values to monitor
        monitor_.set_vector_attribute((*it_3).second, reverse);
        ++vec_pos;
      }
    }
    
    
    ////// SCALAR GROUP PROCESSING
    //
    else
    {
      std::string update_intervals;
      std::string last_updates;
      bool needs_update = true;
      
      if(has_update_interval_mapping_)
      {
        unsigned int lu = boost::lexical_cast<unsigned int>(last_updates);
        unsigned int ui = boost::lexical_cast<unsigned int>(update_intervals);
        
        if( lu + ui >= time(NULL) )
        {
          needs_update = false;
        }
        else
        {
          needs_update = true;
        }
      }
      
      
      if(needs_update) 
      {
        msg << "Updating (scalar) monitor_group '" << name_ << "'.";
        
        ////// CMD -> VALUE MAPPINGS
        //
        std::vector<mapping_t>::const_iterator cmd_val_it_s;
        for(cmd_val_it_s = cmd_value_mappings_.begin(); cmd_val_it_s != cmd_value_mappings_.end(); ++cmd_val_it_s)
        {           
          std::string command = description_.get_attribute((*cmd_val_it_s).first);
          boost::process::child c = run_bash_script("/bin/bash", command);
          boost::process::pistream & out = c.get_stdout();
          std::string val; getline(out, val);
          monitor_.set_attribute((*cmd_val_it_s).second, val);
          boost::process::status status = c.wait();
        }
        
        ////// VALUE -> VALUE MAPPINGS
        //
        std::vector<mapping_t>::const_iterator val_val_it_s;
        for(val_val_it_s = value_value_mappings_.begin(); val_val_it_s != value_value_mappings_.end(); ++val_val_it_s)
        {           
          std::string val = description_.get_attribute((*val_val_it_s).first);
          monitor_.set_attribute((*val_val_it_s).second, val);
        }
        
        if(has_update_interval_mapping_)
        {
          std::string update_time = boost::lexical_cast<std::string>(time(NULL));
          monitor_.set_attribute(update_interval_mapping_.second, update_time);
        }
        
        LOG_WRITE_SUCCESS_2(log_sptr_, msg);
        global_update = true;
      }
    }
  }
  catch(saga::exception const & e)
  {
    LOG_WRITE_FAILED_AND_THROW_2(log_sptr_,msg, e.what(), faust::NoSuccess);
  }
  
  return global_update;
}

