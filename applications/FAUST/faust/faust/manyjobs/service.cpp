/*
 *  service.cpp
 *  FAUST - Framework for Adaptive Ubiquitous Scalable Tasks
 *
 *  Created by Ole Weidner on 11/22/08.
 *  Copyright 2008 Center for Computation & Technology. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying 
 *  LICENSE file or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include <faust/faust/manyjobs/service.hpp>
#include <faust/impl/manyjobs/service_impl.hpp>

using namespace faust::manyjobs;

////////////////////////////////////////////////////////////////////////////////
//
service::service (std::vector<resource> resources, int num_jobs)
: impl(new faust::impl::manyjobs::service_impl(resources, num_jobs))
{
}

////////////////////////////////////////////////////////////////////////////////
//
service::~service()
{
}

////////////////////////////////////////////////////////////////////////////////
//
job service::create_job(description job_desc)
{
  return impl->create_job(job_desc);
}

////////////////////////////////////////////////////////////////////////////////
//
job service::create_job(description job_desc, std::string job_id, state job_state)
{
  return impl->create_job(job_desc, job_id, job_state);
}

////////////////////////////////////////////////////////////////////////////////
//
job_group service::create_job_group(std::vector<description> job_descs)
{
  return impl->create_job_group(job_descs);
}

////////////////////////////////////////////////////////////////////////////////
//
job_group service::create_job_group(std::vector<description> job_descs, 
                           std::string dep_job, state job_state)
{
  return impl->create_job_group(job_descs, dep_job, job_state);
}

////////////////////////////////////////////////////////////////////////////////
//
std::vector<std::string> service::list_jobs()
{
  return impl->list_jobs();
}

////////////////////////////////////////////////////////////////////////////////
//
std::vector<std::string> service::list_resources()
{
  return impl->list_resources();
}

////////////////////////////////////////////////////////////////////////////////
//
faust::manyjobs::job service::get_job(std::string job_id)
{
  return impl->get_job(job_id);
}
