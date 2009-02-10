/*
 *  main.cpp
 *  FAUST - Framework for Adaptive Ubiquitous Scalable Tasks
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
  std::vector<faust::resource> hostlist;
  faust::resource h1, h2, h3;
  
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
  std::cout << "\nTesting: list_resources() & get_resource()" << std::endl;
  std::cout << "==========================================" << std::endl;
  std::vector<std::string> rl = s.list_resources();
  std::vector<std::string>::const_iterator ci1;
  for(ci1 = rl.begin(); ci1 != rl.end(); ++ci1) {
    std::cout << s.get_resource(*ci1).contact << " " ;
    std::cout << s.get_resource(*ci1).workdir << " " ;
    std::cout << s.get_resource(*ci1).queue   << " " ;
    std::cout << s.get_resource(*ci1).project << std::endl ;
  } 
  try {
    s.get_resource("non_existing_contact");
  }
  catch(faust::exception const & e) {
    std::cerr << "Exception successfully caught: " << e.what() << std::endl;
  }
  //
  //////////////////////////////////////////////////////////////////////////////
  
  return 0;
}