/*
 *  main.cpp
 *  GridNPB-SAGA
 *
 *  Created by Ole Weidner on 10/7/08.
 *  Copyright 2008 Center for Computation & Technology. All rights reserved.
 *
 */

#include <string>
#include <fstream>
#include <cstdlib>
#include <iostream>

#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>

#include "TaskController.hpp"

int main (int argc, char * const argv[]) 
{
    saga::url rm_url_1("any://oliver1.loni.org/jobmanager-pbs");

    
    GridNPB::TaskController t1(rm_url_1, 
                               "/work/oweidner/", 
                               "checkpt", 
                               "loni_jha_big",
                               16);

    
    
    return EXIT_SUCCESS;
}
