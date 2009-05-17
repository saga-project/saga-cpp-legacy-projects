/*
 *  monitor_group.hpp 
 *  FAUST - Framework for Adaptive Ubiquitous Scalable Tasks
 *  Website: https://macpro01.cct.lsu.edu/trac/faust
 *
 *  Created by Ole Weidner <oweidner@cct.lsu.edu> on 05/11/09.
 *  Copyright 2008-2009 Center for Computation & Technology. 
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying 
 *  LICENSE file or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef FAUST_AGENT_MONITOR_MONITOR_GROUP_HPP
#define FAUST_AGENT_MONITOR_MONITOR_GROUP_HPP

#include <saga/saga.hpp>

#include <boost/process.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

#include <faust/impl/logwriter.hpp>
#include <faust/faust/resource_description.hpp>
#include <faust/faust/resource_monitor.hpp>

namespace faust { namespace agent { namespace monitor {
  
  //////////////////////////////////////////////////////////////////////////////
  //
  inline boost::process::child 
  run_bash_script(std::string bash_path, std::string const & shell_command)
  {
    boost::process::launcher cmd_adv_launcher_;
    cmd_adv_launcher_.set_stdin_behavior  (boost::process::redirect_stream);
    cmd_adv_launcher_.set_stdout_behavior (boost::process::redirect_stream);
    cmd_adv_launcher_.set_stderr_behavior (boost::process::close_stream);
    cmd_adv_launcher_.set_merge_out_err(true);
    
    boost::process::command_line cl(bash_path, "", "/usr/bin/");
    cl.argument("-c");
    cl.argument(shell_command);
    
    return cmd_adv_launcher_.start(cl);  
  }
  //
  //////////////////////////////////////////////////////////////////////////////
  
  //////////////////////////////////////////////////////////////////////////////
  //
  class monitor_group 
  {
    
  private:
    
    std::string name_;
    bool is_vector_group_;
    int vector_length_;
    
    boost::shared_ptr <faust::detail::logwriter> log_sptr_;
    
    typedef std::pair<std::string, std::string> mapping_t;
    std::vector<mapping_t> cmd_value_mappings_;
    std::vector<mapping_t> value_value_mappings_;
    
    bool has_update_interval_mapping_;
    mapping_t update_interval_mapping_;
    
    unsigned int default_update_interval_;
    time_t default_last_update_;
    
    faust::resource_description description_;
    faust::resource_monitor monitor_;
    
    bool validate_(std::vector<mapping_t> &vec, SAGA_OSSTREAM & msg);
    bool process_all_();
    
  public:
    
    monitor_group(std::string name, 
                  faust::resource_description desc_,
                  faust::resource_monitor mon_,
                  boost::shared_ptr <faust::detail::logwriter> log_sptr);
    
    ~monitor_group();
    
    bool execute();

    void set_update_interval_mapping(char const* const rd_attrib, char const* const rm_attrib);
    void add_cmd_value_mapping      (char const* const rd_attrib, char const* const rm_attrib);
    void add_value_value_mapping    (char const* const rd_attrib, char const* const rm_attrib);
    
  };
  //
  //////////////////////////////////////////////////////////////////////////////

  
}}} // namespace faust::agent::monitor

#endif