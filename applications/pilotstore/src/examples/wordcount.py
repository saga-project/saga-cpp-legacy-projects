""" Example application demonstrating pilot store
    (implementation currently based on local SAGA adaptors)
"""
import sys
sys.path.append("../../../BigJob/")
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

NUMBER_JOBS=1

def has_finished(state):
        state = state.lower()
        if state=="done" or state=="failed" or state=="canceled":
            return True
        else:
            return False

if __name__ == "__main__":

    starttime=time.time()
    
    # create app url for passing arguments between mapping and reduce tasks
    app_url = saga.url("advert://" + DATABASE_HOST + "/"+APPLICATION_NAME + "-" + str(uuid.uuid1()) + "/")
    app_dir = saga.advert.directory(app_url, saga.advert.Create | 
                                             saga.advert.CreateParents | 
                                             saga.advert.ReadWrite)
    
    pd = pilot_data()
    ##########################################################################################
    # Variant 1
    base_dir = saga.url("file://localhost" + os.getcwd()+"/data")
    ps1 = pilot_store("affinity1", base_dir, pd)
    # files can be added either relative to base_dir
    ps1.register_file(saga.url("pg20417.txt")) 
    ps1.register_file(saga.url("pg5000.txt")) 
    
    pd.add_pilot_store(ps1)
    #pilot_data.to_advert(pd)
    
    ##########################################################################################
    print "Start some BigJob w/ affinity"
    resource_list = []
    resource_list.append( {"gram_url" : "fork://localhost/", "number_cores" : "64", "allocation" : "<your allocation>", 
                           "queue" : "workq", "re_agent": (os.getcwd() + "/../../../bigjob/bigjob_agent_launcher.sh"), 
                           "affinity" : "affinity1"})

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
            for i in range(0, NUMBER_JOBS):
                # create job description
                jd = saga.job.description()
                jd.executable = os.getcwd()+"/wordcount-map.py"
                jd.number_of_processes = "1"
                jd.spmd_variation = "single"
                jd.arguments = [pd.app_url.get_string(), p.name, app_url.get_string()]
                jd.working_directory = os.getcwd()
                jd.output =  os.getcwd() + "/map-stdout-" + str(i) + ".txt"
                jd.error = os.getcwd() + "/map-stderr-" + str(i) + ".txt"
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
                
    while 1: 
        finish_counter=0
        result_map = {}
        for i in range(0, NUMBER_JOBS):
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
        if finish_counter == NUMBER_JOBS:
            break

    ############################################################################################
    # process output of mapping phase
    result_stores = app_dir.list()
    result_store_names = []
    for x in result_stores:
        new_url = app_url.get_string() + "/" + str(x)
        print "open " + str(new_url)
        e = saga.advert.directory(saga.url(new_url), saga.advert.Read)        
        result_store_names.append(e.get_attribute("ps"))
                                  
    print "Result pilot stores: " + str(result_store_names)
    # Reduce Task
    for p in pd.list_pilot_store():
        if p.name in result_store_names:
            url = p.url.get_string()
            print "Start reduce for PS: " + p.name + " URL: " + url
            try:
                jobs = []
                job_start_times = {}
                job_states = {}
                cwd = os.getcwd()
                for i in range(0, NUMBER_JOBS):
                    # create job description
                    jd = saga.job.description()
                    jd.executable = os.getcwd()+"/wordcount-reduce.py"
                    jd.number_of_processes = "1"
                    jd.spmd_variation = "single"
                    jd.arguments = [pd.app_url.get_string(), p.name, app_url.get_string()]
                    jd.working_directory = os.getcwd()
                    jd.output =  os.getcwd() + "/reduce-stdout-" + str(i) + ".txt"
                    jd.error = os.getcwd() + "/reduce-stderr-" + str(i) + ".txt"
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
                
    while 1: 
        finish_counter=0
        result_map = {}
        for i in range(0, NUMBER_JOBS):
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
        if finish_counter == NUMBER_JOBS:
            break

    mjs.cancel()
    runtime = time.time()-starttime
    print "Runtime: " + str(runtime) + " s; Runtime per Job: " + str(runtime/NUMBER_JOBS)
    print "Pilot Stores"
    for p in pd.list_pilot_store():
        print p
        
    
    del pd #deletes file from resource
    