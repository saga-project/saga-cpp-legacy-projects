//  Copyright (c) 2005-2007 Hartmut Kaiser 
//  Copyright (c) 2005-2007 Andre Merzky   (andre@merzky.net)
// 
//  Distributed under the Boost Software License, Version 1.0. 
//  (See accompanying file LICENSE or copy at 
//   http://www.boost.org/LICENSE_1_0.txt)

// saga includes
#include <saga/saga.hpp>

// saga engine includes
#include <saga/impl/config.hpp>

// saga adaptor includes
#include <saga/saga/adaptors/task.hpp>
#include <saga/saga/adaptors/attribute.hpp>

// saga package includes
#include <saga/saga/packages/job/adaptors/job.hpp>
#include <saga/saga/packages/job/adaptors/job_self.hpp>

// adaptor includes
#include "cpr_job_service.hpp"
#include "migol.hpp"


// constructor
cpr_job_service_cpi_impl::cpr_job_service_cpi_impl (proxy                * p, 
                                                    cpi_info const       & info,
                                                    saga::ini::ini const & glob_ini, 
                                                    saga::ini::ini const & adap_ini,
                                                    TR1::shared_ptr <saga::adaptor> adaptor)
: base_cpi (p, info, adaptor, cpi::Noflags)
{
   
    //init_migol_context();

    instance_data data(this);
    if (!data->rm_.get_url().empty())
    {
        saga::url rm(data->rm_);
        std::string host(rm.get_host());
        std::string scheme(rm.get_scheme());
        if (!scheme.empty() && scheme != "https")
        {
            SAGA_OSSTREAM strm;
            strm << "Could not initialize job service for [" << data->rm_ << "]. " 
            << "Only any:// and gram:// schemes are supported.";
            SAGA_ADAPTOR_THROW(SAGA_OSSTREAM_GETSTRING(strm), 
                               saga::BadParameter); 
        }
    }
    else
    {
        SAGA_OSSTREAM strm;
        strm << "Could not initialize job service for [" << data->rm_ << "]. " 
        << "Resource discovery is not available yet.";
        SAGA_ADAPTOR_THROW(SAGA_OSSTREAM_GETSTRING(strm), 
                           saga::BadParameter); 
    }
    
    
}

// destructor
cpr_job_service_cpi_impl::~cpr_job_service_cpi_impl (void)
{
}

//////////////////////////////////////////////////////////////////////
// SAGA API functions
void cpr_job_service_cpi_impl::sync_create_job (saga::job::job         & ret, 
                                                saga::job::description   jd)
{
    // make sure the executable path is given
    saga::attribute attr (jd);
    if (!attr.attribute_exists(saga::job::attributes::description_executable) ||
        attr.get_attribute(saga::job::attributes::description_executable).empty())
    {
        SAGA_ADAPTOR_THROW("Missing 'Executable' attribute on job description.",
                           saga::BadParameter);
    }
    
    std::string guid;
    {//scoped lock
        adaptor_data_t d(this);
        guid = d->migol_guid;
        std::cout << "job_service_cpi_impl::sync_create_job: " << guid << std::endl;
    }
    cpr::migol::instance()->migol::update_jobdescription(guid, jd);
    
    // get resource manager string
    std::string rm;
    //    { 
    //        instance_data data(this);
    //        rm = ensure_resourcemanager(data->rm_);
    //    }
    
    saga::job::job job = saga::adaptors::job(rm, jd, proxy_->get_session());
    
    //    // set the created attribute
    //    saga::adaptors::attribute jobattr (job);
    //    std::time_t current = 0;
    //    std::time(&current);
    //    jobattr.set_attribute(saga::job::attributes::created, ctime(&current));
    ret = job;

}

void cpr_job_service_cpi_impl::sync_run_job (saga::job::job     & ret, 
                                             std::string          host, 
                                             std::string          exe, 
                                             saga::job::ostream & in, 
                                             saga::job::istream & out, 
                                             saga::job::istream & err)
{
  // rely on fallback adaptor to kick in
  SAGA_ADAPTOR_THROW ("run_job is not implemented", saga::NotImplemented);
}

// FIXME: merge known_job_list_ with created_job_list
void cpr_job_service_cpi_impl::sync_list (std::vector <std::string> & ret)
{
    SAGA_ADAPTOR_THROW (std::string ("Not implemented"), 
                        saga::NotImplemented);
}

void cpr_job_service_cpi_impl::sync_get_job (saga::job::job & ret, 
                                             std::string      jobid)
{
    SAGA_ADAPTOR_THROW (std::string ("Not implemented"), 
                        saga::NotImplemented);
}

void cpr_job_service_cpi_impl::sync_get_self (saga::job::self & ret)
{
    SAGA_ADAPTOR_THROW (std::string ("Not implemented"), 
                        saga::NotImplemented);
}







