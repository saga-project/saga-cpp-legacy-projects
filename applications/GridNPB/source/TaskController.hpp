/*
 *  Task.hpp
 *  GridNPB-SAGA
 *
 *  Created by Ole Weidner on 10/8/08.
 *  Copyright 2008 Center for Computation & Technology. All rights reserved.
 *
 */

#include <list>
#include <string> 

#include <saga/saga.hpp>

#ifndef GRIDNPB_TASK_H

namespace
{
    ////////////////////////////////////////////////////////////////////////////
    // 'itoa' - print integers in a given base 2-16 (default 10)
    // http://www.cs.princeton.edu/courses/archive/fall96/cs126/examples/itoa.c
    //
    int convert(int n, int b, char str[], int i) {
        if (n/b > 0)
            i = convert(n/b, b, str, i);
        str[i++] = "0123456789ABCDEF"[n%b];
        return i;
    }
    
    int itoa(int n, int b, char str[]) {
        int i = convert(n, b, str, 0);
        str[i] = '\0';
        return i;
    }
}

namespace GridNPB 
{    
    class TaskController {
        
    private:
        std::list<std::string> host_list;
        saga::job::description job_desc;
        saga::job::job job;
        
    public:
        TaskController(saga::url rm_url, std::string workdir, int instances);
        ~TaskController() { };
        
        void launch();
        void kill(int timeout);
        
    };
}

#endif