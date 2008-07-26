/*
 *  saga-cpr.cpp
 *  saga_applications
 *
 *  Created by luckow on 18.06.08.
 */

#include "saga-cpr.h"
#include <saga/saga.hpp>
#include <saga/saga/cpr.hpp>

int main (int argc, char* argv[])
{
    //Init Monitoring
    saga::cpr::service js(saga::url("gram://ubuntu2"));
    
    //Checkpoint Registration
    saga::url url("remd_checkpoint");
    saga::cpr::checkpoint chkpt(url);
    chkpt.add_file(saga::url("gsiftp://~/remd/chkpt_it1.dat"));
    chkpt.add_file(saga::url("gsiftp://~/remd/chkpt_it2.dat"));
    
    std::vector<saga::url> files;
    files = chkpt.list_files();
    std::cout << "Received files: " <<std::endl;
    for (int i = 0; i < files.size(); i++)
    {
        std::cout << files[i] << std::endl;
    }
        
    //Job Submission via Migol/GRAM2
    saga::cpr::description jd;
 
    jd.set_attribute (saga::job::attributes::description_executable, "/bin/echo");
    jd.set_attribute (saga::job::attributes::description_workingdirectory, "/tmp/");
    jd.set_attribute (saga::job::attributes::description_output, "~/stdout");
    jd.set_attribute (saga::job::attributes::description_error, "~/stderr");
    
    std::vector<std::string> args;
    args.push_back("ho");
    args.push_back("ha");
    if (!args.empty())
        jd.set_vector_attribute (saga::job::attributes::description_arguments, args);
    
    saga::cpr::job job = js.create_job(jd, jd);
    std::string id = job.get_job_id();
    
    std::cout<<"Job ID: "<< id << std::endl;    
    saga::job::state state  = job.get_state();
    
    std::cout<<"Job State: "<< state << " ... run job now."<<std::endl;    
    job.run();    
    
    state  = job.get_state();
    std::cout<<"Job State: "<< state << std::endl;
    
    std::cout<<"finished saga-cpr"<<std::endl;   
}
