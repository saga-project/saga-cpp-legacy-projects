/*
 *  main.cpp
 *  FAUST - Framework for Adaptive Ubiquitous Scalable Tasks
 *
 *  Created by Ole Weidner on 11/22/08.
 *  Copyright 2008 Center for Computation & Technology. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying 
 *  LICENSE file or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include <iostream>

#include <faust/faust.hpp>
#include <faust/manyjobs.hpp>

int main (int argc, char* argv[])
{
  std::vector<faust::manyjobs::resource> hostlist;
  faust::manyjobs::resource h1, h2, h3;
  
  //h1.contact = "gram://gatekeeper.lonestar.tacc.teragrid.org:2119/jobmanager-lsf";
  //h1.project = "";
  //h1.queue   = "";
  //h1.workdir = "";

  //h2.contact = "gram://gatekeeper.ranger.tacc.teragrid.org:2119/jobmanager-sge";

  h2.contact = "gram://qb.loni.org:2119/jobmanager-pbs";
  h2.project = "loni_jha_big";
  h2.queue   = "workq";
  h2.workdir = "/tmp/";
  
  h3.contact = "gram://qb1.loni.org:2119/jobmanager-pbs";
  h3.project = "loni_jha_big";
  h3.queue   = "workq";
  h3.workdir = "/tmp/";
  
  //hostlist.push_back(h1);
  hostlist.push_back(h2);
  hostlist.push_back(h3);
  
  faust::manyjobs::service s(hostlist, 64);
  
  //////////////////////////////////
  // test service::create_job_group()
  faust::manyjobs::description d1, d2;
  std::vector<faust::manyjobs::description> desc;
  desc.push_back(d1);
  desc.push_back(d2);
  faust::manyjobs::job_group jg1 = s.create_job_group(desc); 
  std::cout << "group id: " << jg1.get_job_id() << std::endl;
  faust::manyjobs::job_group jg2 = jg1;
  std::cout << "copied group id: " << jg2.get_job_id() << std::endl;

  
  //////////////////////////////////
  // test service::create_job()
  faust::manyjobs::description d;
  faust::manyjobs::job j = s.create_job(d); 
  faust::manyjobs::job j1 = s.create_job(d); 
  faust::manyjobs::job j2 = s.create_job(d); 
  
  //////////////////////////////////
  // test job.get_job_id()
  std::cout << j.get_job_id() << std::endl;

  //////////////////////////////////
  // test service::list_resources()
  std::vector<std::string> rl = s.list_resources();
  std::vector<std::string>::const_iterator ci1;
  for(ci1 = rl.begin(); ci1 != rl.end(); ++ci1)
    std::cout << (*ci1) << std::endl;
  
  //////////////////////////////////
  // test service::list_jobs()
  std::vector<std::string> jl = s.list_jobs();
  std::vector<std::string>::const_iterator ci;
  for(ci = jl.begin(); ci != jl.end(); ++ci)
    std::cout << (*ci) << std::endl;
  
  
  s.debug_check();
  
  return 0;
}