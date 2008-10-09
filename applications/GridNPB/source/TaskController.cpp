/*
 *  Task.cpp
 *  GridNPB-SAGA
 *
 *  Created by Ole Weidner on 10/8/08.
 *  Copyright 2008 Center for Computation & Technology. All rights reserved.
 *
 */
#include "TaskController.hpp"

#define HOSTLIST_OUT "hostlist.out"
#define HOSTLIST_ERR "hostlist.err"


GridNPB::TaskController::TaskController(saga::url rm_url, std::string workdir, int instances)
{
    namespace attr = saga::job::attributes;
    
    char i_buf[50];
    ::itoa(instances, 10, i_buf);
        
    saga::url workdir_url;
    workdir_url.set_host(rm_url.get_host());
    workdir_url.set_scheme("any");
    workdir_url.set_path(workdir);
    
    std::vector<std::string> project;
    project.push_back("loni_jha_big");
    
    try {
        // try to remove any existing previous hostlists
        saga::filesystem::directory basedir(workdir_url);
        
        if(basedir.exists(HOSTLIST_OUT))
            basedir.remove(HOSTLIST_OUT);
        if(basedir.exists(HOSTLIST_ERR))
            basedir.remove(HOSTLIST_ERR);
    }
    catch (saga::exception const & e) {
        std::cerr << "Exception: " << e.what () << std::endl;
    }
    
    try {   
        
        job_desc.set_attribute(attr::description_executable, "/bin/hostname");
        
        job_desc.set_attribute(attr::description_wall_time_limit, "10");
        job_desc.set_attribute(attr::description_number_of_processes, i_buf);
        job_desc.set_attribute(attr::description_processes_per_host, "1");
        
        job_desc.set_vector_attribute(attr::description_job_project, project);
        job_desc.set_attribute(attr::description_queue, "checkpt");
        
        job_desc.set_attribute(attr::description_working_directory, workdir);
        job_desc.set_attribute(attr::description_output, HOSTLIST_OUT);
        job_desc.set_attribute(attr::description_error,  HOSTLIST_ERR);
        
        
        // create job instance through job service
        saga::job::service js (rm_url.get_url());
        job = js.create_job (job_desc);
    }
    catch (saga::exception const & e) {
        std::cerr << "Exception: " << e.what () << std::endl;
    }
    
    
    /// OUTPUT
    job.run ();
    
    while(job.get_state() == saga::job::Running)
    {
        std::cout << "job state: " << job.get_state() << std::endl;
        sleep(2);
    }
    
    try {
        
        std::string path(workdir);
        path.append("/");
        path.append(HOSTLIST_OUT);
        saga::url path_url(workdir_url);
        path_url.set_path(path);
        
        // try to remove any existing previous hostlists
        saga::filesystem::file f (saga::url(path_url), saga::filesystem::Read);
        
        while ( true )
        {
            saga::size_t const n = 1024*64;
            saga::uint8_t data[n+1];
            
            for ( unsigned int i = 0; i <= n; ++i ) { data[i] = '\0'; }
            
            // read a chunk into the buffer
            if ( f.read (saga::buffer (data, n), n) )
            {
                // show what we found
                std::cout << data;
            }
            else
            {
                break;
            }
        }
        
    }
    catch (saga::exception const & e) {
        std::cerr << "Exception: " << e.what () << std::endl;
    }
    
}

void GridNPB::TaskController::launch()
{

    
    
    
}