//  Copyright (c) 2005-2007 Hartmut Kaiser 
//  Copyright (c) 2005-2007 Andre Merzky   (andre@merzky.net)
// 
//  Distributed under the Boost Software License, Version 1.0. 
//  (See accompanying file LICENSE or copy at 
//   http://www.boost.org/LICENSE_1_0.txt)

// saga includes
#include <saga/saga.hpp>

// saga adaptor icnludes
#include <saga/saga/adaptors/task.hpp>
#include <saga/saga/adaptors/attribute.hpp>
#include <saga/saga/adaptors/file_transfer_spec.hpp>

// saga engine includes
#include <saga/impl/config.hpp>
#include <saga/impl/exception_list.hpp>

// saga package includes
#include <saga/saga/packages/cpr/adaptors/cpr_job_self.hpp>
#include <saga/saga/packages/cpr/cpr_job_description.hpp>
#include <common_helpers.hpp>
// adaptor includes
#include "cpr_job.hpp"
namespace cpr
{
// constructor
cpr_job_cpi_impl::cpr_job_cpi_impl (proxy                           * p, 
                                    cpi_info const                  & info,
                                    saga::ini::ini const            & glob_ini, 
                                    saga::ini::ini const            & adap_ini,
                                    TR1::shared_ptr <saga::adaptor>   adaptor)
: base_cpi  (p, info, adaptor, cpi::Noflags),
    state_(saga::job::New)
    {

    SAGA_VERBOSE (SAGA_VERBOSE_LEVEL_INFO)
    {
        std::cout<<"constructor: cpr_job_cpi_impl:cpr_job_cpi_impl" <<std::endl;
    }
    mutex_type::scoped_lock l(mtx_);
    {//scoped lock    
        instance_data idata (this);
        saga::url url_ = idata->rm_;
       
        // check if we have a rm url.  If yes, check if we are asked for.
        if ( ! idata->rm_.get_string ().empty () )
        {
            // initialize our service URL
            url_ = idata->rm_;        
            if ( ! url_.get_scheme ().empty ()      && 
                url_.get_scheme () != "gram"     && 
                url_.get_scheme () != "any"      )
            {
                SAGA_OSSTREAM strm;
                strm << "Could not initialize job for [" << idata->rm_ << "]. " 
                << "Only any:// and fork:// schemes are supported.";
                
                SAGA_ADAPTOR_THROW(SAGA_OSSTREAM_GETSTRING(strm), 
                                   saga::BadParameter);
            }
        }
        else
        {
            // if we don't have an URL at all, we accept
        }
        boost::shared_ptr<cpr::migol> mig= cpr::migol::instance();
        SAGA_VERBOSE (SAGA_VERBOSE_LEVEL_INFO)
        {
            std::cout<<"Register job at AIS" <<std::endl;
        }
        //jobid_==migol_guid
        jobid_ = idata->jobid_;
        jobid_.assign(mig->register_service("http://qb", "SAGA App", "unsubmitted"));    
        std::cout<<"Migol GUID: " << jobid_ << std::endl; 
        saga::adaptors::attribute attr (this);    
        attr.set_attribute (saga::job::attributes::jobid, jobid_);    
        update_state( saga::job::New);
    }    
}

// destructor
cpr_job_cpi_impl::~cpr_job_cpi_impl (void)
{
}

//  SAGA API functions
void cpr_job_cpi_impl::sync_get_state (saga::job::state & ret)
{
    ret = get_job_state();
    //ret=state_;
}

void cpr_job_cpi_impl::sync_get_description (saga::cpr::description & ret)
{
    SAGA_ADAPTOR_THROW ("Not implemented.", 
                        saga::NotImplemented);
}

void cpr_job_cpi_impl::sync_get_job_id (std::string & ret)
{
   jobid_ = get_job_id();
   SAGA_VERBOSE (SAGA_VERBOSE_LEVEL_INFO)
   {
        std::cout<<"cpr_job_cpi_impl::sync_get_job_id: " << jobid_ << std::endl;
   }
   ret=jobid_;
}

// access streams for communication with the child
void cpr_job_cpi_impl::sync_get_stdin (saga::job::ostream & ret)
{
    SAGA_ADAPTOR_THROW ("Not implemented.", 
                        saga::NotImplemented);
}

void cpr_job_cpi_impl::sync_get_stdout (saga::job::istream & ret)
{
    SAGA_ADAPTOR_THROW ("Not implemented.", 
                        saga::NotImplemented);
}

void cpr_job_cpi_impl::sync_get_stderr (saga::job::istream & ret)
{
    SAGA_ADAPTOR_THROW ("Not implemented.", 
                        saga::NotImplemented);
}

void cpr_job_cpi_impl::sync_checkpoint (saga::impl::void_t & ret)
{
    SAGA_ADAPTOR_THROW ("Not implemented.", 
                        saga::NotImplemented);
}

void cpr_job_cpi_impl::sync_migrate (saga::impl::void_t     & ret, 
                                     saga::cpr::description   jd)
{
    SAGA_ADAPTOR_THROW ("Not implemented.", 
                        saga::NotImplemented);
}

void cpr_job_cpi_impl::sync_signal (saga::impl::void_t & ret, 
                                    int                  signal)
{
    SAGA_ADAPTOR_THROW ("Not implemented.", 
                        saga::NotImplemented);
}


//  suspend the child process 
void cpr_job_cpi_impl::sync_suspend (saga::impl::void_t & ret)
{
    SAGA_ADAPTOR_THROW ("Not implemented.", 
                        saga::NotImplemented);
}

//  resume the child process 
void cpr_job_cpi_impl::sync_resume (saga::impl::void_t & ret)
{
    SAGA_ADAPTOR_THROW ("Not implemented.", 
                        saga::NotImplemented);
}


//////////////////////////////////////////////////////////////////////
// inherited from the task interface
void cpr_job_cpi_impl::sync_run (saga::impl::void_t & ret)
{
    SAGA_VERBOSE (SAGA_VERBOSE_LEVEL_INFO)
    {
        std::cout<<"cpr_job_cpi_impl::sync_run " << std::endl;
    }   
    mutex_type::scoped_lock l(mtx_);
    {//scoped lock 
    // retrieve jd, check it, and run.
    instance_data     idata (this);
    adaptor_data_t adata(this); 
    
    if ( ! idata->jd_is_valid_ )
    {
        SAGA_ADAPTOR_THROW ("Job description cannot be retrieved.", 
                            saga::NotImplemented);
    }
    
    // retrieve and check saga job description for this host
    saga::cpr::description  jd_ = idata->jd_start_;
    saga::url rm_ = idata->rm_;
    // check if we can run on the candidate hosts
    // FIXME: attrib should always exist
    std::vector <std::string> chosts;
   
    //where to run: job specified with rm url?
    if ( jd_.attribute_exists (saga::job::attributes::description_candidatehosts) )
    { 
        chosts = jd_.get_vector_attribute 
        (saga::job::attributes::description_candidatehosts);        
        for ( unsigned int i = 0; i < chosts.size (); i++ )
        {
            SAGA_VERBOSE (SAGA_VERBOSE_LEVEL_INFO)
            {
                std::cout<<"cpr_job_cpi_impl::sync_run: " << chosts[i] << std::endl;
            }   
        }
        SAGA_ADAPTOR_THROW ("Cannot submit to CandidateHosts.", 
                            saga::BadParameter);
    }
    
    std::string exe, exe_dir, args_string, stdin, stderr, stdout;
    if ( jd_.attribute_exists (saga::job::attributes::description_executable) )
    { 
        exe = jd_.get_attribute (saga::job::attributes::description_executable);
        exe_dir = jd_.get_attribute (saga::job::attributes::description_workingdirectory);
        std::vector<std::string> args = jd_.get_vector_attribute (saga::job::attributes::description_arguments);
        try{
            stdin = jd_.get_attribute (saga::job::attributes::description_input);
        }
        catch ( saga::exception const & e ) 
        {
            SAGA_VERBOSE (SAGA_VERBOSE_LEVEL_INFO)
            {
                std::cout<<"exception: " << e.what() << std::endl;
            } 
        }
        try{
        stdout = jd_.get_attribute (saga::job::attributes::description_output);
        }
        catch ( saga::exception const & e ) 
        {
            SAGA_VERBOSE (SAGA_VERBOSE_LEVEL_INFO)
            {
                std::cout<<"exception: " << e.what() << std::endl;
            } 
        }
        try{    
            stderr = jd_.get_attribute (saga::job::attributes::description_error);        
        }
        catch ( saga::exception const & e ) 
        {
            SAGA_VERBOSE (SAGA_VERBOSE_LEVEL_INFO)
            {
                std::cout<<"exception: " << e.what() << std::endl;
            } 
        }
        
        for (unsigned int i = 0; i < args.size(); i++)
        {
            //std::cout << args[i] << std::endl;
            args_string.append(args[i]);
            args_string.append(" ");
        }
    }
    
    if ( state_ != saga::job::New )
    {
        SAGA_ADAPTOR_THROW ("run can only be called on New jobs.", 
                            saga::IncorrectState);
    }
        
    boost::shared_ptr<cpr::migol> mig= cpr::migol::instance();
    SAGA_VERBOSE (SAGA_VERBOSE_LEVEL_INFO)
    {
        std::cout<<"Start job at: " << rm_.get_host() << std::endl;
        std::cout<<"Exe: " << exe << std::endl;
        std::cout<<"Args: " << exe_dir << std::endl;
        std::cout<<"Working Dir: " << args_string << std::endl;
        std::cout<<"Stdin: " << stdin << std::endl;
        std::cout<<"Stdout: " << stdout << std::endl;
        std::cout<<"Stderr: " << stderr << std::endl;        
    }
    
    /**
     submit_job(std::string guid, 
     std::string contact,
     std::string executable_start,
     std::string execution_directory_start,
     std::string arguments_start,
     std::string stdin,
     std::string stdout,
     std::string stderr,
     std::string arguments_restart)
    **/
    bool result = mig->submit_job(jobid_, rm_.get_host(), exe, exe_dir, args_string, stdin, stdout, stderr, args_string);   
    SAGA_VERBOSE (SAGA_VERBOSE_LEVEL_INFO)
    {
        std::cout<<"Result of job submission: " << result << std::endl;
    }
    }
        
}

void cpr_job_cpi_impl::sync_cancel (saga::impl::void_t & ret)
{
    SAGA_ADAPTOR_THROW ("Not Implemented.", 
                        saga::NotImplemented);
}

void cpr_job_cpi_impl::sync_wait (bool   & ret, 
                                  double   timeout)
{

}


///////////////////////////////////////////////////////////////////////
//  helper methods
    std::string cpr_job_cpi_impl::get_job_id ()    {
        //mutex_type::scoped_lock l(mtx_);
//        {//scoped lock 
//            instance_data idata (this);
//            std::string jobid = idata->jobid_;
//            return jobid;
//        }
        saga::attribute attr (this->proxy_);
        std::string jobid = attr.get_attribute(saga::job::attributes::jobid);
        return jobid;
    }

    saga::job::state cpr_job_cpi_impl::get_job_state (void)
    {
      //  saga::monitorable monitor (this->proxy_);
//        saga::metric m (monitor.get_metric(saga::metrics::task_state));
//        saga::job::state state = (saga::job::state) 
//        boost::lexical_cast<int> (m.get_attribute(saga::attributes::metric_value));
//        return (state);
        
        std::string guid = get_job_id();
        boost::shared_ptr<cpr::migol> mig= cpr::migol::instance();
        std::string state = mig->get_job_state(guid);
        if(state != ""){
            state_ = migol_to_saga_state(state);
        }
        return state_;
    }
    
    void cpr_job_cpi_impl::update_state(saga::job::state new_state)
    {
        saga::monitorable monitor (this->proxy_);
        saga::adaptors::metric m (monitor.get_metric(saga::metrics::task_state));
        m.set_attribute(saga::attributes::metric_value, 
                        boost::lexical_cast<std::string>(new_state));
    }
    
    
    saga::job::state cpr_job_cpi_impl::migol_to_saga_state(std::string migol_state){
        if(migol_state=="unsubmitted"){
            return saga::job::New;   
        } else if (migol_state=="pending"){
            return saga::job::New;        
        } else if (migol_state=="active"){
            return saga::job::Running;
        } else if (migol_state=="done"){
            return saga::job::Done;
        } else if (migol_state=="inactive"){
            return saga::job::Failed;
        } else if (migol_state=="migrating"){
            return saga::job::Failed;
        }
        return saga::job::Unknown;
    }
    
}
