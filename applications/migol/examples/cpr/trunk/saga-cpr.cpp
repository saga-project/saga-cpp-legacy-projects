/*
 *  saga-cpr.cpp
 *  saga_applications
 *
 *  Created by luckow on 18.06.08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "saga-cpr.h"
#include <saga/saga.hpp>
#include <saga/saga/cpr.hpp>

int main (int argc, char* argv[])
{
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
    
    
    //saga::cpr::service js(saga::url("https://migol.kicks-ass.org:8443/wsrf/services/migol/JobBrokerService"));

}
