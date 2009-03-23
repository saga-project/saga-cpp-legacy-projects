/*
 *  query_system.cpp 
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

#include <faust/faust/exception.hpp>
#include <agent/query_system.hpp>

using namespace saga;
using namespace faust;

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

faust::agent::query_system::query_system(std::string shell_path, faust::resource_description & rd, faust::resource_monitor & rm)
: sp_(shell_path), rd_(rd), rm_(rm)
{
  
}


bool faust::agent::query_system::check_directory_attributes()
{
  
  // we need to check the integrity of the directory-related attributes
  if(!rd_.attribute_exists(FAR::dir_id)) {
    return true; // no attributes defined. That's perfectly valid.
  }
  
  std::vector<std::string> dir_id;
  if(!rd_.attribute_exists(FAR::dir_id)) {
    dir_id = rd_.get_vector_attribute(FAR::dir_id);
  }
  std::vector<std::string> dir_path;
  if(!rd_.attribute_exists(FAR::dir_path)) {
    dir_path = rd_.get_vector_attribute(FAR::dir_path);
  }
  std::vector<std::string> dir_dev_space_total_cmd;
  if(!rd_.attribute_exists(FAR::dir_dev_space_total_cmd)) {
    dir_dev_space_total_cmd = rd_.get_vector_attribute(FAR::dir_dev_space_total_cmd);
  }
  std::vector<std::string> dir_dev_space_used_cmd;
  if(!rd_.attribute_exists(FAR::dir_dev_space_used_cmd)) {
    dir_dev_space_used_cmd = rd_.get_vector_attribute(FAR::dir_dev_space_used_cmd);
  }  
  std::vector<std::string> dir_quota_total_cmd;
  if(!rd_.attribute_exists(FAR::dir_quota_total_cmd)) {
    dir_quota_total_cmd = rd_.get_vector_attribute(FAR::dir_quota_total_cmd);
  }  
  std::vector<std::string> dir_quota_used_cmd;
  if(!rd_.attribute_exists(FAR::dir_quota_used_cmd)) {
    dir_quota_used_cmd = rd_.get_vector_attribute(FAR::dir_quota_used_cmd);
  }    
  
  if(dir_id.size() != dir_path.size() || 
     dir_id.size() != dir_dev_space_total_cmd.size() ||
     dir_id.size() != dir_dev_space_used_cmd.size() ||
     dir_id.size() != dir_quota_total_cmd.size() ||
     dir_id.size() != dir_quota_used_cmd.size())
  {
    return false;
  }
  else 
  {
    return true;
  }
}

void faust::agent::query_system::query_directories()
{
  // check attribute persistency 
  if(directory_attributes_checked_ == false) {
    directory_attributes_ok_ = check_directory_attributes();
    directory_attributes_checked_ = true;
  }
  
  if(directory_attributes_ok_ != true)
  {
    // do nothing. report warning //
  }
  else
  {
    std::vector<std::string> dir_id;
    if(!rd_.attribute_exists(FAR::dir_id)) {
      dir_id = rd_.get_vector_attribute(FAR::dir_id);
    }
    std::vector<std::string> dir_path;
    if(!rd_.attribute_exists(FAR::dir_path)) {
      dir_path = rd_.get_vector_attribute(FAR::dir_path);
    }
    std::vector<std::string> dir_dev_space_total_cmd;
    if(!rd_.attribute_exists(FAR::dir_dev_space_total_cmd)) {
      dir_dev_space_total_cmd = rd_.get_vector_attribute(FAR::dir_dev_space_total_cmd);
    }
    std::vector<std::string> dir_dev_space_used_cmd;
    if(!rd_.attribute_exists(FAR::dir_dev_space_used_cmd)) {
      dir_dev_space_used_cmd = rd_.get_vector_attribute(FAR::dir_dev_space_used_cmd);
    }  
    std::vector<std::string> dir_quota_total_cmd;
    if(!rd_.attribute_exists(FAR::dir_quota_total_cmd)) {
      dir_quota_total_cmd = rd_.get_vector_attribute(FAR::dir_quota_total_cmd);
    }  
    std::vector<std::string> dir_quota_used_cmd;
    if(!rd_.attribute_exists(FAR::dir_quota_used_cmd)) {
      dir_quota_used_cmd = rd_.get_vector_attribute(FAR::dir_quota_used_cmd);
    }  
    
    rm_.set_vector_attribute(FAM::dir_id, dir_id);
    rm_.set_vector_attribute(FAM::dir_path, dir_path);
    
    /*for(int i=0; i< rd_.get_vector_attribute(FAR::dir_id).size(); ++i) {
   rm_.set_vector_attribute(FAM::dir_id, rd_.get_vector_attribute(FAR::dir_id).at(i));
      
      
      boost::process::child child = run_bash_script("/bin/bash", "/bin/df /Users/oweidner | awk '/\\// {print $4}'");
      boost::process::pistream & out = child.get_stdout();
      
      std::string line;
      
      while (getline(out, line))
      {
        
        std::cerr << "Output: " << line << std::endl;
      }
      
      boost::process::status status = child.wait();
    }*/
  }
  
}
