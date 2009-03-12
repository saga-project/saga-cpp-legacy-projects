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

int main (int argc, char* argv[])
{
  //faust::resource qb_reconnect1("queenbee.loni.org");
  //qb_reconnect1.set_persistent(false);
  
  /*std::vector<std::string> dir_ids, dir_path, dir_dev_space_total_cmd;
  
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
  queenbee_rd.set_attribute("agent_submit_url", "gram://qb1.loni.org:2119/jobmanager-fork");
  queenbee_rd.set_vector_attribute("dir_id", dir_ids);
  queenbee_rd.set_vector_attribute("dir_path", dir_path);
  queenbee_rd.set_vector_attribute("dir_dev_space_total_cmd", dir_dev_space_total_cmd);
  
  faust::resource queenbee(queenbee_rd);*/
  
  // test re-connect
  faust::resource qb_reconnect("queenbee.loni.org");
  //qb_reconnect.set_persistent(false); 
  faust::resource_description qb_rec = qb_reconnect.get_description();
  std::vector<std::string> attr_ = qb_rec.list_attributes();
  std::vector<std::string>::const_iterator it;
  for(it = attr_.begin(); it != attr_.end(); ++it)
  {
    std::cout << "attribute: " << (*it) << std::endl;
  }
  
/*  std::vector<faust::resource_description> hostlist;
  faust::resource_description h1, h2, h3;
  
  h1.contact = "gram://gatekeeper.lonestar.tacc.teragrid.org:2119/jobmanager-lsf";
  h1.project = "";
  h1.queue   = "";
  h1.workdir = "/tmp/";

  h2.contact = "gram://qb.loni.org:2119/jobmanager-pbs";
  h2.project = "loni_jha_big";
  h2.queue   = "workq";
  h2.workdir = "/tmp/";
  
  h3.contact = "gram://qb1.loni.org:2119/jobmanager-pbs";
  h3.project = "loni_jha_big";
  h3.queue   = "workq";
  h3.workdir = "/tmp/";
  
  hostlist.push_back(h1);
  hostlist.push_back(h2);
  hostlist.push_back(h3);
  
  faust::service s(hostlist, 64);
  
  
  //////////////////////////////////////////////////////////////////////////////
  //
  std::cout << "\nTesting: list_resource_descriptions() & get_resource_description()" << std::endl;
  std::cout << "==========================================" << std::endl;
  std::vector<std::string> rl = s.list_resource_descriptions();
  std::vector<std::string>::const_iterator ci1;
  for(ci1 = rl.begin(); ci1 != rl.end(); ++ci1) {
    std::cout << s.get_resource_description(*ci1).contact << " " ;
    std::cout << s.get_resource_description(*ci1).workdir << " " ;
    std::cout << s.get_resource_description(*ci1).queue   << " " ;
    std::cout << s.get_resource_description(*ci1).project << std::endl ;
  } 
  try {
    s.get_resource_description("non_existing_contact");
  }
  catch(faust::exception const & e) {
    std::cerr << "Exception successfully caught: " << e.what() << std::endl;
  }
  //
  //////////////////////////////////////////////////////////////////////////////
  */
  return 0;
}