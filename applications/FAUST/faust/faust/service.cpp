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

#include <faust/faust/service.hpp>
#include <faust/impl/service_impl.hpp>

using namespace faust;

////////////////////////////////////////////////////////////////////////////////
//
service::service (std::vector<resource> resources, int num_jobs)
: impl(new faust::impl::service_impl(resources, num_jobs))
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
job service::create_job(description job_desc, std::string job_id, dependency dep)
{
  return impl->create_job(job_desc, job_id, dep);
}

////////////////////////////////////////////////////////////////////////////////
//
job service::create_job(description job_desc, job job_obj, dependency dep)
{
  return impl->create_job(job_desc, job_obj, dep);
}

////////////////////////////////////////////////////////////////////////////////
//
job service::create_job(description job_desc, job_group job_group_obj, dependency dep)
{
  return impl->create_job(job_desc, job_group_obj, dep);
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
                           std::string dep_job, dependency dep)
{
  return impl->create_job_group(job_descs, dep_job, dep);
}

////////////////////////////////////////////////////////////////////////////////
//
job_group service::create_job_group(std::vector<description> job_descs, 
                                    job job_obj, dependency dep)
{
  return impl->create_job_group(job_descs, job_obj, dep);
}

////////////////////////////////////////////////////////////////////////////////
//
job_group service::create_job_group(std::vector<description> job_descs, 
                                    job_group job_group_obj, dependency dep)
{
  return impl->create_job_group(job_descs, job_group_obj, dep);
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
faust::job service::get_job(std::string job_id)
{
  return impl->get_job(job_id);
}

////////////////////////////////////////////////////////////////////////////////
//
faust::job_group service::get_job_group(std::string job_id)
{
  return impl->get_job_group(job_id);
}
