""" Example application demonstrating pilot store
    (implementation currently based on local SAGA adaptors)
"""
import sys
sys.path.append("../../../../BigJob/")
sys.path.append("../../../../bigjob//")
sys.path.append("../../store/")

from pilotstore import *
import many_job_affinity

import os
import pdb
import saga
import time
import uuid

# for logging
import logging
logging.basicConfig(level=logging.DEBUG)


APPLICATION_NAME="wordcount"
DATABASE_HOST="localhost"

#DATA_FILE_DIR=os.getcwd()+"/data-small"
#DATA_FILE_DIR="/work/luckow/data"
DATA_FILE_DIR="/Users/luckow/workspace-saga/bfast/DATA_BFAST/hg18chr21_10"
READ_FILE_DIR="/Users/luckow/workspace-saga/bfast/DATA_BFAST/reads_14"

#BFAST executable
BFAST_HOME = "/usr/local/bfast-0.6.4e/bin/"
BFAST = BFAST_HOME + "/bfast"
SOLID2FASTQ = BFAST_HOME + "/solid2fastq"

NUMBER_JOBS=1

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
    model_ps = pilot_store("model", base_dir, pd)
    index_ps = pilot_store("index", base_dir, pd)
    base_dir_read_files = saga.url("file://localhost" + READ_FILE_DIR)
    read_ps = pilot_store("read", base_dir_read_files, pd)
    
    model_files = []
    index_files = []
    read_files = []
    print "Process data files"
    data_files=os.listdir(DATA_FILE_DIR)
    for i in data_files:
        if i.endswith(".fa"):
            model_files.append(i)
        elif i.endswith(".bif"):
            index_files.append(i)
    
    
    read_files_list=os.listdir(READ_FILE_DIR)
    print "Process " + str(len(read_files_list)) + " read files"
    for i in read_files_list:
        print i
        if i.endswith(".fastq"):
            read_files.append(i)
    
    print "Register model files"              
    model_ps.register_files(model_files)
    print "Register index files"              
    index_ps.register_files(index_files)
    print "Register read files"              
    read_ps.register_files(read_files)
    
    pd.add_pilot_store(model_ps)
    pd.add_pilot_store(index_ps)
    pd.add_pilot_store(read_ps)
    
    ##########################################################################################
    print "Start some BigJob w/ affinity"
    resource_list = []
    resource_list.append( {"gram_url" : "fork://localhost", "number_cores" : "1", "allocation" : "loni_jhabig10",
                           "queue" : "workq", "re_agent": (os.getcwd() + "/../../../../bigjob/bigjob_agent_launcher.sh"),
                           "working_directory": (os.getcwd() + "/agent"), "walltime":120, "affinity" : "affinity1"})

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
            for i in range(0, NUMBER_JOBS):
                # create job description
                jd = saga.job.description()
                jd.executable = BFAST
                jd.number_of_processes = "1"
                jd.spmd_variation = "single"
                jd.arguments = ["match", "-f", "alt_HuRef_chr21.fa", 
                                "-A",  "1", "-r", 
                                READ_FILE_DIR  + "/reads.1.fastq",  
                                "-n", "1", "-T", DATA_FILE_DIR + "/matchtmp/" ]
                jd.working_directory = DATA_FILE_DIR
                jd.output =  os.getcwd() + "/bfast.matches.file.hg21.1.bmf.out"
                jd.error = os.getcwd() + "/bfast.matches.file.hg21.1.bmf.err" 
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
    print "Runtime Sub-Jobs: " + str(time.time()-starttime) + " s"
        
    # Cleanup everything
    mjs.cancel()
    runtime = time.time()-starttime
    print "Overall Runtime: " + str(runtime) + " s"
    
    del pd #deletes file from resource
    
