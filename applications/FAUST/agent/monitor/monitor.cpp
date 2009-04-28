/*
 *  monitor.cpp 
 *  FAUST - Framework for Adaptive Ubiquitous Scalable Tasks
 *  Website: https://macpro01.cct.lsu.edu/trac/faust
 *
 *  Created by Ole Weidner <oweidner@cct.lsu.edu> on 03/23/09.
 *  Copyright 2008-2009 Center for Computation & Technology. 
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying 
 *  LICENSE file or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#include <boost/process.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

#include <faust/faust/exception.hpp>
#include <agent/monitor/monitor.hpp>

using namespace saga;
using namespace faust::agent::monitor;

namespace FAR = faust::attributes::resource_description;
namespace FAM = faust::attributes::resource_monitor;

//////////////////////////////////////////////////////////////////////////
//
namespace {
  
  
  inline boost::process::child 
  run_bash_script(std::string bash_path, std::string const & shell_command)
  {
    boost::process::launcher cmd_launcher_;
    cmd_launcher_.set_stdin_behavior(boost::process::redirect_stream);
    cmd_launcher_.set_stdout_behavior(boost::process::redirect_stream);
    cmd_launcher_.set_stderr_behavior(boost::process::close_stream);
    cmd_launcher_.set_merge_out_err(true);
    
    boost::process::command_line cl(bash_path, "", "/usr/bin/");
    cl.argument("-c");
    cl.argument(shell_command);
    
    return cmd_launcher_.start(cl);  
  }
  
}
//////////////////////////////////////////////////////////////////////////
//
monitor::monitor(std::string shell_path, faust::resource_description & rd, 
                               faust::resource_monitor & rm, std::string uuid, faust::detail::logwriter * lw)
: sp_(shell_path), rd_(rd), rm_(rm)
{
  // Initialize the logwriter
  std::string identifier("faust_agent ("+uuid+")"); std::string msg("");
  log_ = new detail::logwriter(identifier, std::cout);
  
  directory_attributes_checked_ = false;
}

//////////////////////////////////////////////////////////////////////////
//  
void monitor::query(query_type qt)
{
  if(qt == QueryAll || qt == QueryDir) {
    query_directories();
  }
}

//////////////////////////////////////////////////////////////////////////
//
bool monitor::check_directory_attributes()
{
  std::string msg("Checking directory attributes for consitency");
  
  // we need to check the integrity of the directory-related attributes
  //if(!rd_.attribute_exists(FAR::dir_id)) {
  //  return false; // no attributes defined. That's perfectly valid.
  //}
  
  std::vector<std::string> dir_id;
  if(rd_.attribute_exists(FAR::dir_id)) {
    dir_id = rd_.get_vector_attribute(FAR::dir_id);
  }
  else {     
    msg += ". FAILED: dir_id not defined! ";
    log_->write(msg, LOGLEVEL_WARNING);
    return false;
  }
  
  std::vector<std::string> dir_path;
  if(rd_.attribute_exists(FAR::dir_path)) {
    dir_path = rd_.get_vector_attribute(FAR::dir_path);
  }
  else {     
    msg += ". FAILED: dir_path not defined! ";
    log_->write(msg, LOGLEVEL_WARNING);
  return false;
  }
  
  std::vector<std::string> dir_dev_space_total_cmd;
  if(rd_.attribute_exists(FAR::dir_dev_space_total_cmd)) {
    dir_dev_space_total_cmd = rd_.get_vector_attribute(FAR::dir_dev_space_total_cmd);
  }
  else {     
    msg += ". FAILED: dir_dev_space_total_cmd not defined! ";
    log_->write(msg, LOGLEVEL_WARNING);
    return false;
  }
  
  std::vector<std::string> dir_dev_space_used_cmd;
  if(rd_.attribute_exists(FAR::dir_dev_space_used_cmd)) {
    dir_dev_space_used_cmd = rd_.get_vector_attribute(FAR::dir_dev_space_used_cmd);
  } 
  else {     
    msg += ". FAILED: dir_dev_space_used_cmd not defined! ";
    log_->write(msg, LOGLEVEL_WARNING);
    return false;
  }
  
  std::vector<std::string> dir_quota_total_cmd;
  if(rd_.attribute_exists(FAR::dir_quota_total_cmd)) {
    dir_quota_total_cmd = rd_.get_vector_attribute(FAR::dir_quota_total_cmd);
  }
  else {     
    msg += ". FAILED: dir_quota_total_cmd not defined! ";
    log_->write(msg, LOGLEVEL_WARNING);
    return false;
  }
  
  std::vector<std::string> dir_quota_used_cmd;
  if(rd_.attribute_exists(FAR::dir_quota_used_cmd)) {
    dir_quota_used_cmd = rd_.get_vector_attribute(FAR::dir_quota_used_cmd);
  }   
  else {     
    msg += ". FAILED ";
    log_->write(msg, LOGLEVEL_WARNING);
    return false;
  }
  
  if(dir_id.size() != dir_path.size() || 
     dir_id.size() != dir_dev_space_total_cmd.size() ||
     dir_id.size() != dir_dev_space_used_cmd.size() ||
     dir_id.size() != dir_quota_total_cmd.size() ||
     dir_id.size() != dir_quota_used_cmd.size())
  {
    msg += ". FAILED ";
    log_->write(msg, LOGLEVEL_WARNING);
    return false;
  }
  else 
  {
    msg += ". SUCCESS ";
    log_->write(msg, LOGLEVEL_INFO);
    return true;
  }
}

void monitor::query_directories()
{
  using boost::lexical_cast;
  using boost::bad_lexical_cast;
  
  std::string msg("Updating directory monitor");
  
  // check attribute persistency 
  if(directory_attributes_checked_ == false) {
    directory_attributes_ok_ = check_directory_attributes();
    directory_attributes_checked_ = true;
  }
  
  if(directory_attributes_ok_ != true)
  {
    msg += ". FAILED: Attribute check returned FALSE. ";
    log_->write(msg, LOGLEVEL_INFO);
  }
  else
  {
    
    if(rd_.attribute_exists(FAR::dir_id)) {
      std::vector<std::string> dir_id = rd_.get_vector_attribute(FAR::dir_id);
      rm_.set_vector_attribute(FAM::dir_id, dir_id);
    }

    if(rd_.attribute_exists(FAR::dir_path)) {
      std::vector<std::string> dir_path = rd_.get_vector_attribute(FAR::dir_path);
      rm_.set_vector_attribute(FAM::dir_path, dir_path);
    }
    std::vector<std::string> dir_dev_space_total_cmd;
    if(rd_.attribute_exists(FAR::dir_dev_space_total_cmd)) {
      dir_dev_space_total_cmd = rd_.get_vector_attribute(FAR::dir_dev_space_total_cmd);
    }
    std::vector<std::string> dir_dev_space_used_cmd;
    if(rd_.attribute_exists(FAR::dir_dev_space_used_cmd)) {
      dir_dev_space_used_cmd = rd_.get_vector_attribute(FAR::dir_dev_space_used_cmd);
    }  
    std::vector<std::string> dir_quota_total_cmd;
    if(rd_.attribute_exists(FAR::dir_quota_total_cmd)) {
      dir_quota_total_cmd = rd_.get_vector_attribute(FAR::dir_quota_total_cmd);
    }  
    std::vector<std::string> dir_quota_used_cmd;
    if(rd_.attribute_exists(FAR::dir_quota_used_cmd)) {
      dir_quota_used_cmd = rd_.get_vector_attribute(FAR::dir_quota_used_cmd);
    }  
    
    boost::process::child child;

    std::vector<std::string> dev_space_total;
    std::vector<std::string> dev_space_used;
    std::vector<std::string> dev_space_free;
    std::vector<std::string> quota_total;
    std::vector<std::string> quota_used;
    std::vector<std::string> quota_free;
        
    for(int i=0; i< dir_dev_space_total_cmd.size(); ++i) 
    {
      child = run_bash_script("/bin/bash", dir_dev_space_total_cmd.at(i));
      boost::process::pistream & out1 = child.get_stdout();
      std::string val1; getline(out1, val1);
      try {
        boost::lexical_cast<int>(val1);
        dev_space_total.push_back(val1);
        boost::process::status status = child.wait();
        std::string error = msg + ". SUCCESS: dir_dev_space_total_cmd returned ' "+val1+"'.";
        log_->write(error, LOGLEVEL_INFO);
      }
      catch(boost::bad_lexical_cast &) {
        dev_space_used.push_back("");
        std::string error = msg + ". FAILED: dir_dev_space_total_cmd returned ' "+val1+"'.";
        log_->write(error, LOGLEVEL_ERROR);
      }
      
      child = run_bash_script("/bin/bash", dir_dev_space_used_cmd.at(i));
      boost::process::pistream & out2 = child.get_stdout();
      std::string val2; getline(out2, val2);
      try {
        boost::lexical_cast<int>(val2);
        dev_space_used.push_back(val2);
        boost::process::status status = child.wait();
        std::string error = msg + ". SUCCESS: dir_dev_space_used_cmd returned ' "+val2+"'.";
        log_->write(error, LOGLEVEL_INFO);
        
        // 'total' & 'used' worked. now we can process 'free'
        int total = boost::lexical_cast<int>(dev_space_total.at(i));
        int used = boost::lexical_cast<int>(dev_space_used.at(i));
        std::string free_s = boost::lexical_cast<std::string>(total-used);
        dev_space_free.push_back(free_s);
        
      }
      catch(boost::bad_lexical_cast &) {
        dev_space_used.push_back("");
        std::string error = msg + ". FAILED: dir_dev_space_used_cmd returned ' "+val2+"'.";
        log_->write(error, LOGLEVEL_ERROR);
      }      
      
      child = run_bash_script("/bin/bash", dir_quota_total_cmd.at(i));
      boost::process::pistream & out3 = child.get_stdout();
      std::string val3; getline(out3, val3);
      try {
        boost::lexical_cast<int>(val3);
        quota_total.push_back(val3);
        boost::process::status status = child.wait();
        std::string error = msg + ". SUCCESS: dir_quota_total_cmd returned ' "+val3+"'.";
        log_->write(error, LOGLEVEL_INFO);
      }
      catch(boost::bad_lexical_cast &) {
        quota_total.push_back("");
        std::string error = msg + ". FAILED: dir_quota_total_cmd returned ' "+val3+"'.";
        log_->write(error, LOGLEVEL_ERROR);
      }     
      
      child = run_bash_script("/bin/bash", dir_quota_used_cmd.at(i));
      boost::process::pistream & out4 = child.get_stdout();
      std::string val4; getline(out4, val4);
      try {
        boost::lexical_cast<int>(val4);
        quota_used.push_back(val4);
        boost::process::status status = child.wait();
        std::string error = msg + ". SUCCESS: dir_quota_used_cmd returned ' "+val4+"'.";
        log_->write(error, LOGLEVEL_INFO);
        
        // 'total' & 'used' worked. now we can process 'free'
        int total = boost::lexical_cast<int>(quota_total.at(i));
        int used = boost::lexical_cast<int>(quota_used.at(i));
        std::string free_s = boost::lexical_cast<std::string>(total-used);
        quota_free.push_back(free_s);
      }
      catch(boost::bad_lexical_cast &) {
        quota_total.push_back("");
        std::string error = msg + ". FAILED: dir_quota_used_cmd returned ' "+val4+"'.";
        log_->write(error, LOGLEVEL_ERROR);
      }   
    }
    
    rm_.set_vector_attribute(FAM::dir_dev_space_total, dev_space_total);
    rm_.set_vector_attribute(FAM::dir_dev_space_used,  dev_space_used);
    rm_.set_vector_attribute(FAM::dir_dev_space_free,  dev_space_free);
    rm_.set_vector_attribute(FAM::dir_quota_total,     quota_total);
    rm_.set_vector_attribute(FAM::dir_quota_used,      quota_used);
    rm_.set_vector_attribute(FAM::dir_quota_free,      quota_free);
    
    rm_.writeToDB();
    
  }
  
}
