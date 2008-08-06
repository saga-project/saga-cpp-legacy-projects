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
        boost::shared_ptr<cpr::migol> mig= cpr::migol::instance();
        SAGA_VERBOSE (SAGA_VERBOSE_LEVEL_INFO)
        {
            std::cout<<"Register job at AIS" <<std::endl;
        }
        jobid_ = idata->jobid_;
        std::string rm_ = idata->rm_.get_string();
        jobid_.assign(mig->register_service(rm_, "SAGA App", "unsubmitted"));    
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

void cpr_job_cpi_impl::sync_get_description (saga::job::description & ret)
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
                                     saga::job::description   jd)
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
        try{
            rm_ = saga::url(chosts[0]);
        } catch ( saga::exception const & e ) 
        {
                SAGA_VERBOSE (SAGA_VERBOSE_LEVEL_INFO)
                {
                    std::cout<<"exception: " << e.what() << std::endl;
                } 
        }
    }
    if ( jd_.attribute_exists (saga::job::attributes::description_environment) ){
        
        SAGA_OSSTREAM strm;
        strm << "Attribute " << saga::job::attributes::description_environment<< " not implemented.";
        SAGA_ADAPTOR_THROW(SAGA_OSSTREAM_GETSTRING(strm), saga::NotImplemented);
    }   
    
    std::string exe, exe_dir, args_string, stdin, stderr, stdout, job_type, number_nodes, number_procs_per_node, number_procs, queue;
    if ( jd_.attribute_exists (saga::job::attributes::description_executable) )
    { 
        exe = jd_.get_attribute (saga::job::attributes::description_executable);
        exe_dir = jd_.get_attribute (saga::job::attributes::description_workingdirectory);
        std::vector<std::string> args = jd_.get_vector_attribute (saga::job::attributes::description_arguments);
        if(jd_.attribute_exists(saga::job::attributes::description_input)){
            stdin = jd_.get_attribute (saga::job::attributes::description_input);
        }
        if(jd_.attribute_exists(saga::job::attributes::description_output)){  
            stdout = jd_.get_attribute (saga::job::attributes::description_output);
        }
        if(jd_.attribute_exists(saga::job::attributes::description_error)){      
            stderr = jd_.get_attribute (saga::job::attributes::description_error);        
        }
        if(jd_.attribute_exists(saga::job::attributes::description_spmdvariation)){   
            job_type = jd_.get_attribute (saga::job::attributes::description_spmdvariation);        
        }
        if(jd_.attribute_exists(saga::job::attributes::description_totalcpucount)){   
            number_nodes = jd_.get_attribute (saga::job::attributes::description_totalcpucount);        
        }
        if(jd_.attribute_exists(saga::job::attributes::description_processesperhost)){   
            number_procs_per_node = jd_.get_attribute (saga::job::attributes::description_processesperhost);        
        }  
        if(jd_.attribute_exists(saga::job::attributes::description_queue)){   
            queue = jd_.get_attribute (saga::job::attributes::description_queue);        
        } 
        if(jd_.attribute_exists(saga::job::attributes::description_numberofprocesses)){
            number_procs = jd_.get_attribute (saga::job::attributes::description_numberofprocesses);
        }
        if(jd_.attribute_exists(saga::job::attributes::description_threadsperprocess)){
           SAGA_OSSTREAM strm;
           strm << "Attribute " << saga::job::attributes::description_threadsperprocess << " not implemented.";
           SAGA_ADAPTOR_THROW(SAGA_OSSTREAM_GETSTRING(strm), saga::NotImplemented);
        }
        
        for (unsigned int i = 0; i < args.size(); i++)
        {
            //std::cout << args[i] << std::endl;
            args_string.append("\"");
            args_string.append(args[i]);
            args_string.append("\"");
            if(i!=(args.size()-1)) {
                args_string.append(" ");
            }
        }
    }
    
    if ( state_ != saga::job::New )
    {
        SAGA_ADAPTOR_THROW ("run can only be called on New jobs.", 
                            saga::IncorrectState);
    }
    SAGA_OSSTREAM strm;
    strm << rm_.get_host();
    std::string path = rm_.get_path();
    if (path!=""){
	if(path.at(0)=='/') {
		path.erase(0,1);
        }
        strm<<"/"<<path;
    }
    std::string contact = SAGA_OSSTREAM_GETSTRING(strm);
    boost::shared_ptr<cpr::migol> mig= cpr::migol::instance();
    SAGA_VERBOSE (SAGA_VERBOSE_LEVEL_INFO)
    {
        std::cout<<"Start job at: " << contact << std::endl;
        std::cout<<"Exe: " << exe << std::endl;
        std::cout<<"Args: " << args_string << std::endl;
        std::cout<<"Working Dir: " << exe_dir << std::endl;
        std::cout<<"Stdin: " << stdin << std::endl;
        std::cout<<"Stdout: " << stdout << std::endl;
        std::cout<<"Stderr: " << stderr << std::endl; 
        std::cout<<"Job Type: " << job_type << std::endl;
        std::cout<<"Total CPUs: " << number_nodes << std::endl;
        std::cout<<"Number Processes (total requested CPUs): " << number_procs << std::endl;
        std::cout<<"Number CPU per Nodes: " << number_procs_per_node << std::endl; 
        std::cout<<"Queue: " << queue << std::endl; 

    }
    
    /**  submit_job    **/
    bool result = mig->submit_job(jobid_, contact, exe, exe_dir, args_string, 
                                  stdin, stdout, stderr, args_string, job_type, 
                                  number_nodes, number_procs_per_node, number_procs, queue);   
    SAGA_VERBOSE (SAGA_VERBOSE_LEVEL_INFO)
    {
        std::cout<<"Result of job submission: " << result << std::endl;
    }
    }
        
}

   void cpr_job_cpi_impl::sync_cancel (saga::impl::void_t & ret, double timeout)
   {
       std::string guid = get_job_id();
       boost::shared_ptr<cpr::migol> mig= cpr::migol::instance();
       mig->cancel_job(guid);
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
