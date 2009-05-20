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

#include <faust/impl/detail/serialize.hpp>

#include <iostream>
#include <fstream>

using namespace faust::agent::monitor;

////////////////////////////////////////////////////////////////////////////////
//
void monitor::init()
{
  namespace FAR = faust::attributes::resource_description;
  namespace FAM = faust::attributes::resource_monitor;

  monitor_group mg1 ("dirs", description_, monitor_, log_sptr_);
  mg1.set_update_interval_mapping (FAR::dir_update_interval,     FAM::dir_last_update);
  mg1.add_value_value_mapping     (FAR::dir_id,                  FAM::dir_id);
  mg1.add_value_value_mapping     (FAR::dir_path,                FAM::dir_path);
  mg1.add_cmd_value_mapping       (FAR::dir_dev_space_total_cmd, FAM::dir_dev_space_total);
  mg1.add_cmd_value_mapping       (FAR::dir_dev_space_used_cmd,  FAM::dir_dev_space_used);
  mg1.add_cmd_value_mapping       (FAR::dir_quota_total_cmd,     FAM::dir_quota_total);
  mg1.add_cmd_value_mapping       (FAR::dir_quota_used_cmd,      FAM::dir_quota_used);
  mgv_.push_back(mg1);
  
  monitor_group mg2 ("queues", description_, monitor_, log_sptr_);
  mg2.set_update_interval_mapping (FAR::queue_update_interval,   FAM::queue_last_update);
  mg2.add_value_value_mapping     (FAR::queue_id,                FAM::queue_id);
  mg2.add_value_value_mapping     (FAR::queue_name,              FAM::queue_name);
  mg2.add_cmd_value_mapping       (FAR::queue_nodes_busy_cmd,    FAM::queue_nodes_busy);
  mg2.add_cmd_value_mapping       (FAR::queue_nodes_down_cmd,    FAM::queue_nodes_down);
  mg2.add_cmd_value_mapping       (FAR::queue_nodes_total_cmd,   FAM::queue_nodes_total);
  mg2.add_cmd_value_mapping       (FAR::queue_nodes_queued_cmd,  FAM::queue_nodes_queued);   
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
      bool should_write = (*it).execute();    
      
      if(should_write)
      {
        THIS->monitor_.get_impl()->write_attributes();
      }
    }

    boost::this_thread::sleep(boost::posix_time::milliseconds(2000));
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
                  saga::advert::entry mon_adv,
                  boost::shared_ptr <faust::detail::logwriter> log_sptr)
: update_interval_(update_interval), description_(desc), monitor_(mon),
  mon_adv_(mon_adv), log_sptr_(log_sptr)

{
  init();
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

