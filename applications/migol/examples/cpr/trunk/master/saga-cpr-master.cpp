/*
 *  saga-cpr-master.cpp
 *  saga_applications
 *
 *  Created by luckow on 18.06.08.
 */

#include <saga/saga.hpp>
#include <saga/saga/cpr.hpp>

int main (int argc, char* argv[])
{
    // Init Monitoring
    // Init Migol/Monitoring
    // uses per default Application Information Service (AIS) configured in 
    // the $SAGA_LOCATON/share/saga/saga_adaptor_migol_cpr.ini
    // required for all subsequent CPR calls (otherwise exception is thrown)
    saga::cpr::service js;
    
    //Job Submission via Migol/GRAM2
    saga::cpr::description jd;
 
    jd.set_attribute (saga::job::attributes::description_executable, "~/saga-cpr-svn/examples/packages/cpr/worker/saga-cpr-worker-wrapper");
    jd.set_attribute (saga::job::attributes::description_workingdirectory, "/tmp/");
    jd.set_attribute (saga::job::attributes::description_output, "~/stdout");
    jd.set_attribute (saga::job::attributes::description_error, "~/stderr");
    
    std::vector<std::string> args;
    args.push_back("60");
    if (!args.empty())
        jd.set_vector_attribute (saga::job::attributes::description_arguments, args);

    std::vector<std::string> candidate_hosts;
    candidate_hosts.push_back("gram://ubuntu2");
    if (!candidate_hosts.empty())
        jd.set_vector_attribute (saga::job::attributes::description_candidatehosts, candidate_hosts);

    //env mapping not supported by Migol backend yet (recommendation: use wrapper script)
//    std::vector<std::string> env;
//    env.push_back("SAGA_LOCATION=/usr/local/saga-svn");
//    env.push_back("LD_LIBRARY_PATH=/usr/local/saga-svn/lib:/usr/lib/jvm/java-1.5.0-sun//jre/lib/i386/server/:/usr/lib/jvm/java-1.5.0-sun//jre/lib/i386/");
//    if (!env.empty())
//        jd.set_vector_attribute (saga::job::attributes::description_environment, env);
    
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
