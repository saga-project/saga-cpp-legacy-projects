""" Example application demonstrating pilot store
    (implementation currently based on local SAGA adaptors)
"""
import sys
sys.path.append("../../../BigJob/")
sys.path.append("../../../bigjob//")
sys.path.append("../store/")

from pilotstore import *
import many_job_affinity

import os
import pdb
import saga
import time
import uuid


APPLICATION_NAME="wordcount"
DATABASE_HOST="localhost"

#DATA_FILE_DIR=os.getcwd()+"/data-small"
DATA_FILE_DIR="/work/luckow/data"

WORKING_DIR="/work/luckow/mr-run/"
#WORKING_DIR=os.getcwd()

NUMBER_MAP_JOBS=16
NUMBER_REDUCE_JOBS=1

starttime=0

def has_finished(state):
    state = state.lower()
    if state=="done" or state=="failed" or state=="canceled":
        return True
    else:
        return False
        
def wait_for_all_jobs(jobs, job_start_times, poll_intervall=5):
    """ waits for all jobs that are in list to terminate """
    while 1: 
        finish_counter=0
        result_map = {}
        number_of_jobs = len(jobs)
        for i in range(0, number_of_jobs):
            old_state = job_states[jobs[i]]
            state = jobs[i].get_state()
            if result_map.has_key(state) == False:
                result_map[state]=0
            result_map[state] = result_map[state]+1
            #print "counter: " + str(i) + " job: " + str(jobs[i]) + " state: " + state
            if old_state != state:
                print "Job " + str(jobs[i]) + " changed from: " + old_state + " to " + state + " Time since start: " + str(time.time()-starttime)
            if old_state != state and has_finished(state)==True:
                print "Job: " + str(jobs[i]) + " Runtime: " + str(time.time()-job_start_times[jobs[i]]) + " s."
            if has_finished(state)==True:
                finish_counter = finish_counter + 1
            job_states[jobs[i]]=state

        print "Current states: " + str(result_map) 
        time.sleep(poll_intervall)
        if finish_counter == number_of_jobs:
            break
            
if __name__ == "__main__":

    starttime=time.time()
    
    # create app url for passing arguments between mapping and reduce tasks
    app_url = saga.url("advert://" + DATABASE_HOST + "/"+APPLICATION_NAME + "-" + str(uuid.uuid1()) + "/")
    app_dir = saga.advert.directory(app_url, saga.advert.Create | 
                                             saga.advert.CreateParents | 
                                             saga.advert.ReadWrite)
    
    pd = pilot_data()
    
    #base_dir = saga.url("file://localhost" + os.getcwd()+"/data")
    base_dir = saga.url("file://localhost" + DATA_FILE_DIR)
    ps1 = pilot_store("affinity1", base_dir, pd)
    ps1.number_of_chunks=NUMBER_MAP_JOBS
    data_files=os.listdir(DATA_FILE_DIR);
    ps1.register_files(data_files)
    
    pd.add_pilot_store(ps1)
    print "time to register: " + str(time.time()-starttime)
    
    ##########################################################################################
    print "Start some BigJob w/ affinity"
    resource_list = []
    resource_list.append( {"gram_url" : "gram://oliver1.loni.org/jobmanager-pbs", "number_cores" : str(NUMBER_MAP_JOBS), "allocation" : "loni_jhabig10",
                           "queue" : "workq", "re_agent": (os.getcwd() + "/../../../bigjob/bigjob_agent_launcher.sh"),
                           "working_directory": (WORKING_DIR + "/agent"), "walltime":1800, "affinity" : "affinity1"})

    #resource_list.append( {"gram_url" : "gram://oliver1.loni.org/jobmanager-pbs", "number_cores" : "4", "allocation" : "loni_jhabig10",
    #                       "queue" : "workq", "re_agent": (os.getcwd() + "/../../../bigjob/bigjob_agent_launcher.sh"),
    #                       "working_directory": (os.getcwd() + "/agent"), "walltime":120, "affinity" : "affinity1"})

    print "Create manyjob service "
    mjs = many_job_affinity.many_job_affinity_service(resource_list, "localhost")
        
    ############################################################################################
    # Map Task        
    print "Pilot Store contains the following pilot data containers: "
    for p in pd.list_pilot_store():
        url = p.url.get_string()
        print url
        try:
            jobs = []
            job_start_times = {}
            job_states = {}
            cwd = os.getcwd()
            for i in range(0, NUMBER_MAP_JOBS):
                # create job description
                jd = saga.job.description()
                jd.executable = os.getcwd()+"/wordcount-map.py"
                jd.number_of_processes = "1"
                jd.spmd_variation = "single"
                jd.arguments = [pd.app_url.get_string(), p.name, str(i), app_url.get_string()]
                jd.working_directory = WORKING_DIR
                jd.output =  WORKING_DIR + "/map-stdout-" + str(i) + ".txt"
                jd.error = WORKING_DIR + "/map-stderr-" + str(i) + ".txt"
                jd.environment = ["affinity=affinity1"]
                subjob = mjs.create_job(jd)
                subjob.run()
                print "Submited sub-job " + "%d"%i + "."
                jobs.append(subjob)
                job_start_times[subjob]=time.time()
                job_states[subjob] = subjob.get_state()
            print "************************ All Jobs submitted ************************"
        except:
            traceback.print_exc(file=sys.stdout)
            try:
                mjs.cancel()
            except:
                pass      

    ############################################################################################
    # Wait for task completion of map tasks - synchronization    
    wait_for_all_jobs(jobs, job_start_times, 5)
    print "Runtime Mapping Phase: " + str(time.time()-starttime) + " s"
    

    ############################################################################################
    # process output of mapping phase
    result_stores = app_dir.list()
    result_store_names = []
    for x in result_stores:
        new_url = app_url.get_string() + "/" + str(x)
        print "open " + str(new_url)
        e = saga.advert.directory(saga.url(new_url), saga.advert.Read)        
        result_store_names.append(e.get_attribute("ps"))
        
    pd.refresh()    
    result_ps = pd[result_store_names[0]]
    
    for i in range(1, len(result_store_names)):
        result_ps.join(pd[result_store_names[i]])
                                  
    print "Number of result pilot stores: " + str(len(result_store_names)) + " joined into: " + str(result_ps)
    
    ############################################################################################
    # Start Reduce Task
    url = result_ps.url.get_string()
    print "Start Reduce phase using Pilot Store: " + result_ps.name + " URL: " + url
    try:
        jobs = []
        job_start_times = {}
        job_states = {}
        cwd = os.getcwd()
        for i in range(0, NUMBER_REDUCE_JOBS):
            # create job description
            jd = saga.job.description()
            jd.executable = os.getcwd()+"/wordcount-reduce.py"
            jd.number_of_processes = "1"
            jd.spmd_variation = "single"
            jd.arguments = [pd.app_url.get_string(), result_ps.name, "0", app_url.get_string()]
            jd.working_directory = WORKING_DIR
            jd.output =  WORKING_DIR + "/reduce-stdout-" + str(i) + ".txt"
            jd.error = WORKING_DIR + "/reduce-stderr-" + str(i) + ".txt"
            jd.environment = ["affinity=affinity1"]
            subjob = mjs.create_job(jd)
            subjob.run()
            print "Submited sub-job " + "%d"%i + "."
            jobs.append(subjob)
            job_start_times[subjob]=time.time()
            job_states[subjob] = subjob.get_state()
        print "************************ All Jobs submitted ************************"
    except:
        traceback.print_exc(file=sys.stdout)
        try:
            mjs.cancel()
        except:
            pass      

    ############################################################################################
    # Wait for task completion of reduce tasks - synchronization      
    wait_for_all_jobs(jobs, job_start_times, 5)            
    
    # Cleanup everything
    mjs.cancel()
    runtime = time.time()-starttime
    print "Overall Runtime: " + str(runtime) + " s"
    
    del pd #deletes file from resource
    
