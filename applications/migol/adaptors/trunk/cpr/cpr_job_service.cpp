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
#include <saga/saga/packages/cpr/cpr_job_description.hpp>
#include <saga/saga/packages/cpr/adaptors/cpr_job.hpp>
#include <saga/saga/packages/cpr/adaptors/cpr_job_self.hpp>

#include <saga/saga/packages/job/adaptors/job.hpp>
#include <saga/saga/packages/job/adaptors/job_self.hpp>

// adaptor includes
#include "cpr_job_service.hpp"
#include <common_helpers.hpp>



namespace cpr
{
    
        // wrapper for gethostname()
        inline std::string get_hostname()
        {
            char buffer[MAX_PATH] = { '\0' };
            gethostname(buffer, sizeof(buffer));
            return std::string(buffer);
        }
        
        // ensure to get a non-empty resource manager name
        inline std::string ensure_resourcemanager(saga::url rm_url)
        {
            if ( rm_url.get_host ().empty () ) 
                rm_url.set_host (get_hostname ());
            if ( rm_url.get_scheme ().empty () ) 
                rm_url.set_scheme("fork");
            return rm_url.get_url();
        }
        
  
int  cpr_job_service_cpi_impl::refcount_ = 0;    
    
// constructor
cpr_job_service_cpi_impl::cpr_job_service_cpi_impl (proxy                * p, 
                                                    cpi_info const       & info,
                                                    saga::ini::ini const & glob_ini, 
                                                    saga::ini::ini const & adap_ini,
                                                    TR1::shared_ptr <saga::adaptor> adaptor)
: base_cpi (p, info, adaptor, cpi::Noflags)
{
   
    
    instance_data data (this);
    saga::url rm = data->rm_; 
    if ( ! data->rm_.get_url ().empty () )
    {
        std::string scheme (data->rm_.get_scheme ());
        if ( ! ( scheme.empty () ) && 
            ! ( scheme == "cpr" ) && 
            ! ( scheme == "migol" ) &&
            ! ( scheme == "any" ) ) 
        {
            SAGA_LOG_BLURB ("saga default cpr adaptor: cpr_job_service c'tor refuse");
            SAGA_ADAPTOR_THROW ("Can't use schemes others from 'any', 'migol', or 'cpr' " 
                                "for cpr job submission.", saga::NotImplemented);
        }
    }            
    
    //check whether Migol has been initialized 
    std::string guid("");    
    mutex_type::scoped_lock l(mtx_);
    {//scoped lock
        refcount_++;
        SAGA_VERBOSE (SAGA_VERBOSE_LEVEL_INFO)
        {
            std::cout<<"Refcounter ~cpr_job_service_cpi_impl: " << refcount_ << std::endl;
        }
        adaptor_data_t d(this);
        guid = d->migol_guid;
        if(guid==""){
            guid=migol::init_migol_context(adap_ini);
            if ( guid == "")
            {
                SAGA_ADAPTOR_THROW (std::string ("Migol infrastructure could not be initialized."), 
                                    saga::IncorrectURL);
            }
            d->migol_guid=guid;
            
        }
        SAGA_LOG_BLURB("cpr_job_service_cpi_impl ctor: end");
    }
   
   // check if we can handle this request
     
   std::string scheme (rm.get_scheme ());
   std::cout<<"Job Manager: " << rm.get_string () << std::endl;
}

// destructor
cpr_job_service_cpi_impl::~cpr_job_service_cpi_impl (void)
{
    SAGA_LOG_BLURB ("~cpr_job_service_cpi_impl d'tor begin");
    std::string guid("");
    std::string state("");
    mutex_type::scoped_lock l(mtx_);
    {//scoped lock
        refcount_--;
        adaptor_data_t d(this);
        guid = d->migol_guid;
        state = d->migol_state;
        
        SAGA_VERBOSE (SAGA_VERBOSE_LEVEL_INFO)
        {
            std::cout<<"Refcounter ~cpr_job_service_cpi_impl: " << refcount_ << std::endl;
        }
        if(refcount_==0 && guid!="" && state!="done"){
            migol::instance()->change_service_state(guid, "done");  
            migol::instance()->finalize_external_monitoring(); 
            d->migol_state="done";
        }
    }
    SAGA_LOG_BLURB ("~cpr_job_service_cpi_impl d'tor end");
}

//////////////////////////////////////////////////////////////////////
// SAGA API functions
void cpr_job_service_cpi_impl::sync_create_job_cpr (saga::cpr::job         & ret, 
                                                    saga::cpr::description   jd_run,
                                                    saga::cpr::description   jd_restart)
{
    saga::url rm;
    std::string guid;

    instance_data data(this);
    mutex_type::scoped_lock l(mtx_);
    {//scoped lock
        adaptor_data_t d(this);
        guid = d->migol_guid;
        if ( guid == "")
        {
            SAGA_ADAPTOR_THROW (std::string ("Migol infrastructure could not be initialized."), 
                                saga::IncorrectState);
        }
        std::cout << "cpr_job_service_cpi_impl::sync_create_job_cpr: " << guid << std::endl;
               
    }
         
    //////////////////////////////////////////////////////////////////////////////
    // create job cpi    
    rm = data->rm_;
    std::cout<<"Create Job at RM: "<< rm.get_string() <<std::endl;
    saga::cpr::job job = saga::adaptors::cpr_job(rm.get_url (), jd_run, jd_restart);
   
    // set the created attribute
    saga::adaptors::attribute jobattr (job);
    std::time_t current = 0;
    std::time(&current);
    jobattr.set_attribute(saga::job::attributes::created, ctime(&current));
    
    ret = job;
}


void cpr_job_service_cpi_impl::sync_create_job     (saga::cpr::job            & ret, 
                          saga::cpr::description      jd_run){

    SAGA_ADAPTOR_THROW ("Not implemented.", 
                        saga::NotImplemented);
    
}
    
    
void cpr_job_service_cpi_impl::sync_run_job (saga::cpr::job     & ret, 
                                             std::string          host, 
                                             std::string          exe, 
                                             saga::job::ostream & in, 
                                             saga::job::istream & out, 
                                             saga::job::istream & err)
{
  // rely on fallback adaptor to kick in
  SAGA_ADAPTOR_THROW ("run_job is not implemented", saga::NotImplemented);    
    
    
}

void cpr_job_service_cpi_impl::sync_list (std::vector <std::string> & ret)
{
    SAGA_ADAPTOR_THROW ("Not implemented.", 
                        saga::NotImplemented);
}

    void cpr_job_service_cpi_impl::sync_get_job        (saga::job::job            & ret,
                              std::string                 jobid)
{
    SAGA_ADAPTOR_THROW ("Not implemented.", 
                        saga::NotImplemented);
}

void cpr_job_service_cpi_impl::sync_get_self (saga::job::self & ret)
{
    SAGA_ADAPTOR_THROW ("Not implemented.", 
                        saga::NotImplemented);
}

} //end namespace cpr
