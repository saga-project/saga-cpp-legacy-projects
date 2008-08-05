/*
 *  saga-cpr-master.cpp
 *  saga_applications
 *
 *  Created by luckow on 18.06.08.
 */

#include <saga/saga.hpp>
#include <saga/saga/cpr.hpp>
#include <sys/time.h>  

std::string print_state(saga::job::state state);

int main (int argc, char* argv[])
{
    struct timeval startTime, endTime; 
    double atime;     
    gettimeofday(&startTime, NULL);
    
    saga::job::service js(saga::url("gram://qb1.loni.org/jobmanager-pbs"));
    
    saga::job::description jd;
 
    jd.set_attribute (saga::job::attributes::description_executable, "/home/luckow/sw/saga-svn/bin/saga-cpr-worker");
    jd.set_attribute (saga::job::attributes::description_workingdirectory, "/home/luckow/NAMD");
    jd.set_attribute (saga::job::attributes::description_output, "namd.log");
    jd.set_attribute (saga::job::attributes::description_error, "namd.err");

    //MPI configuration
    jd.set_attribute (saga::job::attributes::description_spmdvariation, "single");
    jd.set_attribute(saga::job::attributes::description_numberofprocesses, "16");

    //Allocation
    jd.set_attribute (saga::job::attributes::description_queue, "loni_jha_big@");

    std::vector<std::string> args;
    args.push_back("/usr/local/packages/namd-2.6-mvapich-1.0-intel10.1/namd2 NPT.conf");
    args.push_back("mpi");
    if (!args.empty())
        jd.set_vector_attribute (saga::job::attributes::description_arguments, args);
   
    saga::job::job job = js.create_job(jd);
    std::string id = job.get_job_id();
    
    std::cout<<"Job ID: "<< id << std::endl;    
    //saga::job::state state  = job.get_state();
    //
    //std::cout<<"Job State: "<< print_state(state) << " ... run job now."<<std::endl;    
    job.run();    
    
    //state  = job.get_state();
    //std::cout<<"Job State: "<< print_state(state) << std::endl;
 
    
    //cancel job
    //job.cancel();
    
    gettimeofday(&endTime, NULL);
    atime =  (double) (endTime.tv_sec + endTime.tv_usec / 1e6) 
    - (startTime.tv_sec + startTime.tv_usec / 1e6 );           
    
    std::cout<<"finished saga-job: " << atime << " s"<<std::endl;   
}

std::string print_state(saga::job::state state){

if(state==saga::job::New){
   return "NEW";
}else if (state==saga::job::Running){
   return "RUNNING";
} else if (state==saga::job::Done){
   return "DONE";
} else if(state==saga::job::Failed){
   return "FAILED";
}else if (state==saga::job::Suspended){
    return "SUSPENDED";
} else if (state==saga::job::Canceled){
    return "CANCELED";
}
return "FAILED";
}
