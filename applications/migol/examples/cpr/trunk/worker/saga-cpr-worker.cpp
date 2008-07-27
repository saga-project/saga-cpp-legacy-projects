/*
 *  saga-cpr-worker.cpp
 *  saga_applications
 *
 *  Created by luckow on 18.06.08.
 */

#include <saga/saga.hpp>
#include <saga/saga/cpr.hpp>

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
    for (int i = 0; i < files.size(); i++)
    {
        std::cout << files[i] << std::endl;
    }
    
    std::cout<<"finished saga-cpr-worker"<<std::endl;   
}
