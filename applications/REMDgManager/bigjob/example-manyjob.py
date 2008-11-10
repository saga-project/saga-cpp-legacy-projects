#!/usr/bin/env python

"""Module many_job.

This Module is used to launch a set of bigjobs.

"""

import sys
import getopt
import saga
import time
import pdb
import os
import traceback
import advert_job
import logging
import many_job

NUMBER_JOBS=300

def has_finished(state):
        state = state.lower()
        if state=="done" or state=="failed" or state=="canceled":
            return True
        else:
            return False

""" Test Job Submission via ManyJob abstraction """
if __name__ == "__main__":
    try:
        print "ManyJob load test with " + str(NUMBER_JOBS) + " jobs."
        starttime=time.time()
        # create job description
        jd = saga.job.description()
        jd.executable = "/bin/date"
        jd.number_of_processes = "1"
        jd.spmd_variation = "single"
        jd.arguments = [""]
        jd.working_directory = "/home/luckow"
        jd.output = "output.txt"
        jd.error = "error.txt"
        # submit via mj abstraction
        resource_list =  ( {"gram_url" : "gram://qb1.loni.org/jobmanager-pbs", "number_cores" : "128", "allocation" : "loni_jha_big", "queue" : "workq", "re_agent": "$(HOME)/src/REMDgManager/bigjob/advert_launcher.sh"},
                           {"gram_url" : "gram://qb1.loni.org/jobmanager-pbs", "number_cores" : "128", "allocation" : "loni_jha_big", "queue" : "workq", "re_agent": "$(HOME)/src/REMDgManager/bigjob/advert_launcher.sh"})
        #resource_list = []
        #resource_list.append({"gram_url" : "gram://qb1.loni.org/jobmanager-pbs", "number_cores" : "16", "allocation" : "loni_jha_big", "queue" : "workq", "re_agent": "$(HOME)/src/REMDgManager/bigjob/advert_launcher.sh"})
        print "Create manyjob service "
        mjs = many_job.many_job_service(resource_list, None)
        
        jobs = []
        for i in range(0, NUMBER_JOBS):
            subjob = mjs.create_job(jd)
            subjob.run()
            print "Submited sub-job " + "%d"%i + "."
            jobs.append(subjob)

        while 1: 
            finish_counter=0
            result_map = {}
            for i in range(0, NUMBER_JOBS):
                state = jobs[i].get_state()
                if result_map.has_key(state) == False:
                    result_map[state]=0
                result_map[state] = result_map[state]+1
                #print "counter: " + str(i) + " job: " + str(jobs[i]) + " state: " + state
                if has_finished(state)==True:
                     finish_counter = finish_counter + 1

            print "Current states: " + str(result_map) 
            time.sleep(5)


            if finish_counter == NUMBER_JOBS:
                break

        mjs.cancel()
        runtime = time.time()-starttime
        print "Runtime: " + str(runtime) + " s."
    except:
        traceback.print_exc(file=sys.stdout)
        try:
            mjs.cancel()
        except:
            pass

