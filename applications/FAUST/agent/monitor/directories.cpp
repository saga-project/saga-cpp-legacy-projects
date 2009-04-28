/*
 *  system_monitor_directories.cpp 
 *  FAUST - Framework for Adaptive Ubiquitous Scalable Tasks
 *  Website: https://macpro01.cct.lsu.edu/trac/faust
 *
 *  Created by Ole Weidner <oweidner@cct.lsu.edu> on 04/29/09.
 *  Copyright 2008-2009 Center for Computation & Technology. 
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying 
 *  LICENSE file or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "directories.hpp"

using namespace faust::agent::monitor;

namespace FAR = faust::attributes::resource_description;
namespace FAM = faust::attributes::resource_monitor;

directories::directories( boost::shared_ptr <faust::agent::monitor::monitor> mon,
                          boost::shared_ptr <faust::resource_description>    rd, 
                          boost::shared_ptr <faust::resource_monitor>        rm)
: mon_(mon)
{
  mon_ = mon;
}

directories::~directories()
{

}

bool directories::check_attributes()
{
  std::string msg("Checking directory attributes for consitency");
  
  std::vector<std::string> dir_id;
  if(rd_->attribute_exists(FAR::dir_id)) {
    dir_id = rd_->get_vector_attribute(FAR::dir_id);
  }
  else {     
    msg += ". FAILED: dir_id not defined! ";
    log_->write(msg, LOGLEVEL_WARNING);
    return false;
  }
  
  std::vector<std::string> dir_path;
  if(rd_->attribute_exists(FAR::dir_path)) {
    dir_path = rd_->get_vector_attribute(FAR::dir_path);
  }
  else {     
    msg += ". FAILED: dir_path not defined! ";
    log_->write(msg, LOGLEVEL_WARNING);
    return false;
  }
  
  std::vector<std::string> dir_dev_space_total_cmd;
  if(rd_->attribute_exists(FAR::dir_dev_space_total_cmd)) {
    dir_dev_space_total_cmd = rd_->get_vector_attribute(FAR::dir_dev_space_total_cmd);
  }
  else {     
    msg += ". FAILED: dir_dev_space_total_cmd not defined! ";
    log_->write(msg, LOGLEVEL_WARNING);
    return false;
  }
  
  std::vector<std::string> dir_dev_space_used_cmd;
  if(rd_->attribute_exists(FAR::dir_dev_space_used_cmd)) {
    dir_dev_space_used_cmd = rd_->get_vector_attribute(FAR::dir_dev_space_used_cmd);
  } 
  else {     
    msg += ". FAILED: dir_dev_space_used_cmd not defined! ";
    log_->write(msg, LOGLEVEL_WARNING);
    return false;
  }
  
  std::vector<std::string> dir_quota_total_cmd;
  if(rd_->attribute_exists(FAR::dir_quota_total_cmd)) {
    dir_quota_total_cmd = rd_->get_vector_attribute(FAR::dir_quota_total_cmd);
  }
  else {     
    msg += ". FAILED: dir_quota_total_cmd not defined! ";
    log_->write(msg, LOGLEVEL_WARNING);
    return false;
  }
  
  std::vector<std::string> dir_quota_used_cmd;
  if(rd_->attribute_exists(FAR::dir_quota_used_cmd)) {
    dir_quota_used_cmd = rd_->get_vector_attribute(FAR::dir_quota_used_cmd);
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
