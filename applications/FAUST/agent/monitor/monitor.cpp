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


#include <agent/monitor/monitor.hpp>
#include <boost/date_time/posix_time/posix_time_duration.hpp>

#include <iostream>
#include <fstream>

using namespace faust::agent::monitor;

////////////////////////////////////////////////////////////////////////////////
//
void monitor::init()
{
  monitor_group mg1 ("dirs", description_, monitor_, log_sptr_);
  mg1.set_update_interval     ("dir_update_interval");
  mg1.add_value_value_mapping ("dir_id",                  "dir_id");
  mg1.add_value_value_mapping ("dir_path",                "dir_path");
  mg1.add_cmd_value_mapping   ("dir_dev_space_total_cmd", "dir_dev_space_total");
  mg1.add_cmd_value_mapping   ("dir_dev_space_used_cmd",  "dir_dev_space_used");
  mg1.add_cmd_value_mapping   ("dir_quota_total_cmd",     "dir_quota_total");
  mg1.add_cmd_value_mapping   ("dir_quota_used_cmd",      "dir_quota_used");
  mgv_.push_back(mg1);
  
  monitor_group mg2 ("queues", description_, monitor_, log_sptr_);
  mg2.set_update_interval     ("queue_update_interval");
  mg2.add_value_value_mapping ("queue_id",               "queue_id");
  mg2.add_value_value_mapping ("queue_name",             "queue_name");
  mg2.add_cmd_value_mapping   ("queue_nodes_busy_cmd",   "queue_nodes_busy");
  mg2.add_cmd_value_mapping   ("queue_nodes_down_cmd",   "queue_nodes_down");
  mg2.add_cmd_value_mapping   ("queue_nodes_total_cmd",  "queue_nodes_total");
  mg2.add_cmd_value_mapping   ("queue_nodes_queued_cmd", "queue_nodes_queued");   
  mgv_.push_back(mg2);
}


////////////////////////////////////////////////////////////////////////////////
//
void monitor::thread_entry_point_(monitor * THIS)
{
  while(1) // wait for quit() command! 
  {
    std::vector<monitor_group>::iterator it;
    for(it = THIS->mgv_.begin(); it != THIS->mgv_.end(); ++it)
    {
      std::cout << (*it).get_update_interval() << std::endl;
      if( (*it).get_last_update() + (*it).get_update_interval() <= time(NULL) )
      {
        (*it).execute();        
      }
    }

    boost::this_thread::sleep(boost::posix_time::milliseconds(5000));
  }
}


////////////////////////////////////////////////////////////////////////////////
//

unsigned int monitor::get_update_interval()
{
  return update_interval_;
}


////////////////////////////////////////////////////////////////////////////////
//
void monitor::run()
{
  SAGA_OSSTREAM msg;
  
  /* starting the service thread */
  msg << "Starting resource monitor thread.";
  
  try 
  {
    service_thread_ = boost::thread(boost::bind(&thread_entry_point_, this));
    LOG_WRITE_SUCCESS_2(log_sptr_, msg);
  }
  catch(...)
  {
    LOG_WRITE_FAILED_AND_THROW_2(log_sptr_, msg, "Unknown Reson", faust::NoSuccess);
  }
}


////////////////////////////////////////////////////////////////////////////////
//
monitor::monitor (unsigned int update_interval,
                  faust::resource_description desc,
                  faust::resource_monitor mon,
                  boost::shared_ptr <faust::detail::logwriter> log_sptr)
: update_interval_(update_interval), description_(desc), monitor_(mon),
  log_sptr_(log_sptr)

{
  init();
  
  last_update_ = 0;
}


////////////////////////////////////////////////////////////////////////////////
//
monitor::~monitor()
{
  SAGA_OSSTREAM msg;
  msg <<  "Shutting down resource monitor thread.";
  try 
  {
    service_thread_.join();
    LOG_WRITE_SUCCESS_2(log_sptr_, msg);
  }
  catch(...)
  {
    LOG_WRITE_FAILED_AND_THROW_2(log_sptr_, msg, "Unknown Reason", faust::NoSuccess);
  }
}

