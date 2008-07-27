/*
 *  saga-cpr-worker.cpp
 *  saga_applications
 *
 *  Created by luckow on 18.06.08.
 */

#include <saga/saga.hpp>
#include <saga/saga/cpr.hpp>
#include <boost/thread/xtime.hpp>
#include <boost/thread.hpp>

#define SLEEP_TIME 1000;

int main (int argc, char* argv[])
{
    std::cout<<"start saga-cpr-worker"<<std::endl;  
    
    // Init Migol/Monitoring
    // uses per default Application Information Service (AIS) configured in 
    // the $SAGA_LOCATON/share/saga/saga_adaptor_migol_cpr.ini
    // required for all subsequent CPR calls (otherwise exception is thrown)
    saga::cpr::service js;
    
    //Checkpoint Registration
    saga::url url("remd_checkpoint");
    saga::cpr::checkpoint chkpt(url);
    chkpt.add_file(saga::url("gsiftp://~/remd/chkpt_it1.dat"));
    chkpt.add_file(saga::url("gsiftp://~/remd/chkpt_it2.dat"));
    
    std::vector<saga::url> files;
    files = chkpt.list_files();
    std::cout << "Received files: " <<std::endl;
    for (unsigned int i = 0; i < files.size(); i++)
    {
        std::cout << files[i] << std::endl;
    }
    
    int sleeptime= 1000;
    std::cout<<"go to sleep for " << sleeptime <<" s"<<std::endl;   
    boost::xtime xt;
    boost::xtime_get(&xt, boost::TIME_UTC);
    xt.sec += sleeptime;
    boost::thread::sleep(xt);     
    std::cout<<"finished saga-cpr-worker"<<std::endl;   
}
