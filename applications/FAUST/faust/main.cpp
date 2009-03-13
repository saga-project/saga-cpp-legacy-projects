/*
 *  main.cpp
 *  FAUST - Framework for Adaptive Ubiquitous Scalable Tasks
 *  Website: https://macpro01.cct.lsu.edu/trac/faust
 *
 *  Created by Ole Weidner <oweidner@cct.lsu.edu> on 11/22/08.
 *  Copyright 2008-2009 Center for Computation & Technology. 
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying 
 *  LICENSE file or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include <iostream>
#include <faust/faust.hpp>

#include <iostream>

int main (int argc, char* argv[])
{
  
  std::vector<std::string> dir_ids, dir_path, dir_dev_space_total_cmd;
  
  faust::resource_description queenbee_rd;
  
  // A directory description
  dir_ids.push_back("my_work_dir");
  dir_path.push_back("/home/");
  dir_dev_space_total_cmd.push_back("df . | awk '/\// {print $2}'");
  
  // Another directory description
  dir_ids.push_back("my_home_dir");
  dir_path.push_back("/work/");
  dir_dev_space_total_cmd.push_back("df . | awk '/\// {print $2}'");
  
  queenbee_rd.set_attribute("identifier", "queenbee.loni.org");
  
  queenbee_rd.set_attribute("faust_agent_submit_url",  "fork://localhost/");
  queenbee_rd.set_attribute("faust_agent_binary_path", "/Users/oweidner/Work/FAUST/agent/faust_agent");	
  queenbee_rd.set_attribute("saga_root_path",          "/usr/local/saga-1.1/");
  
  queenbee_rd.set_vector_attribute("dir_id", dir_ids);
  queenbee_rd.set_vector_attribute("dir_path", dir_path);
  queenbee_rd.set_vector_attribute("dir_dev_space_total_cmd", dir_dev_space_total_cmd);
  
  while(1) {
    sleep(5);
    std::cout << std::endl;
    faust::resource queenbee(queenbee_rd);
    queenbee.set_persistent(false);
    
    sleep(5);
    
    // test re-connect
    /*faust::resource qb_reconnect("queenbee.loni.org");
     //qb_reconnect.set_persistent(false); 
     faust::resource_description qb_rec = qb_reconnect.get_description();
     std::vector<std::string> attr_ = qb_rec.list_attributes();
     std::vector<std::string>::const_iterator it;
     for(it = attr_.begin(); it != attr_.end(); ++it)
     {
     std::cout << "attribute: " << (*it) << std::endl;
     }*/
  }
  return 0;
}
