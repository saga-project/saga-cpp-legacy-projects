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
import Calibrate
import FileTest

"""
These are parameters that you want to change for the jobs
"""
NUMBER_STAGES                     =  2
NUMBER_JOBS_PER_STAGE      = 2
WORK_DIR                               = "/work/yye00/ICAC/Simulations/"
SIMULATION_DIR_PREFIX          = "Simulation_id_"
SIMULATION_EXE                      = "/work/yye00/ICAC/cactus_SAGASandTank"
SIMULATION_PAR_FILE_PREFIX = "/work/yye00/ICAC/ParFiles/ParFile_id_"
ENKF_EXE                                 =  "/bin/date"
OUTPUT_FILE_EXTENSION         = "h5"

"""
Now create the work directories by iterating over the range and creating sub-directories
This works fine for LOCAL SYSTEMS ONLY and needs to be revisited for other systems
"""
for i in range(0, NUMBER_JOBS_PER_STAGE):
    simdirname = WORK_DIR + SIMULATION_DIR_PREFIX + "%05d" %i
    if not os.path.isdir(simdirname+"/"):
        os.mkdir(simdirname+"/")
"""
These are the arrays of parameters for the  various jobs, you only need the sizes if they are different
"""
JOB_SIZES=[]
for i in range(0, NUMBER_JOBS_PER_STAGE):
  JOB_SIZES.append(4)

def has_finished(state):
        state = state.lower()
        if state=="done" or state=="failed" or state=="canceled":
            return True
        else:
            return False

""" This has been modified with a loop for all stages """
if __name__ == "__main__":
    # Store the start time and run time for all stages
    starttime=[]
    runtime=[]
    try:
        for stage in range(0,  NUMBER_STAGES):
            print "Launching " + str(NUMBER_JOBS_PER_STAGE) + " jobs for Stage number " + str(stage)
            starttime.append(time.time())

            # submit via mj abstraction
            resource_list =  ( {"gram_url" : "gram://qb1.loni.org/jobmanager-pbs", "number_cores" : "8", "allocation" : "loni_cybertools", "queue" : "workq", "re_agent": "/home/yye00/ICAC/bigjob/advert_launcher.sh"},
                           {"gram_url" : "gram://qb1.loni.org/jobmanager-pbs", "number_cores" : "8", "allocation" : "loni_cybertools", "queue" : "workq", "re_agent": "/home/yye00/ICAC/bigjob/advert_launcher.sh"})
            print "Create manyjob service "
            mjs = many_job.many_job_service(resource_list, None)
        
            jobs = []
            job_start_times = {}
            job_states = {}
            cwd = os.getcwd()
            for i in range(0, NUMBER_JOBS_PER_STAGE):
                # create job description
                jd = saga.job.description()
                jd.executable = SIMULATION_EXE
                jd.number_of_processes = str(JOB_SIZES[i])
                jd.spmd_variation = "mpi"
                # toggle between these two statements depending on the parfile directory
                #jd.arguments = [SIMULATION_PAR_FILE_PREFIX+"%05d" %i +".par"]
                jd.arguments=[SIMULATION_PAR_FILE_PREFIX+"All.par"]
                jd.working_directory = WORK_DIR + SIMULATION_DIR_PREFIX + "%05d" %i + "/"
                jd.output =  jd.working_directory + "stdout.txt"
                jd.error = jd.working_directory + "stderr.txt"
                subjob = mjs.create_job(jd)
                subjob.run()
                print "Submited sub-job " + "%05d"%i + "."
                jobs.append(subjob)
                job_start_times[subjob]=time.time()
                job_states[subjob] = subjob.get_state()
            print "************************ All Jobs submitted ************************"
            print "And now we wait for each stage to finish"
            
            while 1: 
                finish_counter=0
                result_map = {}
                for i in range(0, NUMBER_JOBS_PER_STAGE):
                    old_state = job_states[jobs[i]]
                    state = jobs[i].get_state()
                    if result_map.has_key(state) == False:
                        result_map[state]=0
                    result_map[state] = result_map[state]+1
                    #print "counter: " + str(i) + " job: " + str(jobs[i]) + " state: " + state
                    if old_state != state:
                        print "Job " + str(jobs[i]) + " changed from: " + old_state + " to " + state
                    if old_state != state and has_finished(state)==True:
                         print "Job: " + str(jobs[i]) + " Runtime: " + str(time.time()-job_start_times[jobs[i]]) + " s."
                    if has_finished(state)==True:
                         finish_counter = finish_counter + 1
                    job_states[jobs[i]]=state
    
                print "Current states: " + str(result_map) 
                time.sleep(5)
                if finish_counter == NUMBER_JOBS_PER_STAGE:
                    break
            runtime.append( time.time()-starttime[stage])
            print "#################################################"
            print "Runtime for Stage "+ str(stage)+ " is: " + str(runtime[stage]) + " s; Runtime per Job: " + str(runtime[stage]/NUMBER_JOBS_PER_STAGE)
            print "finished with Stage number: " + str(stage)
            print "#################################################"
            print "Performing checking"
            print "Running Calibrate.py"
            if(Calibrate.Calibrate() == 1):
                print "Calibration successful"
            else:
                print "Calibration failed, proceed with caution"
            
            print "#################################################"
            print "Checking the actual output files"
            RERUN_LIST= FileTest.FileTest(WORK_DIR, SIMULATION_DIR_PREFIX, OUTPUT_FILE_EXTENSION, NUMBER_JOBS_PER_STAGE)
            if len(RERUN_LIST)!=0:
                print "THE FOLLOWING JOBS FAILED: " + str(out)
                print "Attempting to re-submit"
                rerun_jobs = []
                rerun_job_start_times = {}
                rerun_job_states = {}
                cwd = os.getcwd()
                for i in RERUN_LIST:
                    # create job description
                    jd = saga.job.description()
                    jd.executable = SIMULATION_EXE
                    jd.number_of_processes = str(JOB_SIZES[i])
                    jd.spmd_variation = "mpi"
                    # toggle between these two statements depending on the parfile directory
                    #jd.arguments = [SIMULATION_PAR_FILE_PREFIX+"%05d" %i +".par"]
                    jd.arguments=[SIMULATION_PAR_FILE_PREFIX+"All.par"]
                    jd.working_directory = WORK_DIR + SIMULATION_DIR_PREFIX + "%05d" %i + "/"
                    jd.output =  jd.working_directory + "stdout.txt"
                    jd.error = jd.working_directory + "stderr.txt"
                    subjob = mjs.create_job(jd)
                    subjob.run()
                    print "Submited sub-job " + "%05d"%i + "."
                    rerun_jobs.append(subjob)
                    rerun_job_start_times[subjob]=time.time()
                    rerun_job_states[subjob] = subjob.get_state()
                print "************************ All rerun_jobs submitted ************************"
                print "And now we wait for the reruns to finish"
                
                while 1: 
                    rerun_finish_counter=0
                    rerun_result_map = {}
                    for i in range(0, len(RERUN_LIST)):
                        old_state = rerun_job_states[rerun_jobs[i]]
                        state = rerun_jobs[i].get_state()
                        if rerun_result_map.has_key(state) == False:
                            rerun_result_map[state]=0
                        rerun_result_map[state] = rerun_result_map[state]+1
                        #print "counter: " + str(i) + " job: " + str(rerun_jobs[i]) + " state: " + state
                        if old_state != state:
                            print "Job " + str(rerun_jobs[i]) + " changed from: " + old_state + " to " + state
                        if old_state != state and has_finished(state)==True:
                             print "Job: " + str(rerun_jobs[i]) + " Runtime: " + str(time.time()-rerun_job_start_times[rerun_jobs[i]]) + " s."
                        if has_finished(state)==True:
                             finish_counter = finish_counter + 1
                        rerun_job_states[rerun_jobs[i]]=state
        
                    print "Current states: " + str(result_map) 
                    time.sleep(5)
                    if finish_counter == len(RERUN_LIST):
                        break
                print "Finished the resubmit, now we double check the files"
                RERUN_LIST2= FileTest.FileTest(WORK_DIR, SIMULATION_DIR_PREFIX, OUTPUT_FILE_EXTENSION, NUMBER_JOBS_PER_STAGE)
                if(len(RERUN_LIST2)!=0):
                    print "We have catastrophic failure that we cannot recover from, aborting"
                    sys.exit()
            else:
                print "Success: all jobs finished properly"
            
        mjs.cancel()

    except:
        traceback.print_exc(file=sys.stdout)
        try:
            mjs.cancel()
        except:
            pass
