""" Example application demonstrating pilot store
    (implementation currently based on local SAGA adaptors)
"""

from pilotstore import *
import os
import pdb
import saga
import sys
import time

if os.getenv("BIGJOB_HOME")!=None:
    BIGJOB_HOME= os.getenv("BIGJOB_HOME")
else:
    BIGJOB_HOME= os.getcwd() + "/../../../bigjob/trunk/"
    os.environ["BIGJOB_HOME"]=BIGJOB_HOME

sys.path.append(BIGJOB_HOME)
import many_job_affinity

NUMBER_JOBS=2

def has_finished(state):
        state = state.lower()
        if state=="done" or state=="failed" or state=="canceled":
            return True
        else:
            return False

if __name__ == "__main__":

    starttime=time.time()

    pd = pilot_data()
    
    ##########################################################################################
    # Variant 1
    base_dir = saga.url("file://localhost" + os.getcwd()+"/test/data1/")
    ps1 = pilot_store("affinity1", base_dir, pd)
    # files can be added either relative to base_dir
    ps1.register_file(saga.url("test1.txt")) 
    # files can be added as absolute URL
    ps1.register_file(saga.url("file://localhost" + os.getcwd()+"/test/data1/test2.txt"))
    pd.add_pilot_store(ps1)
    
    ##########################################################################################
    # Variant 2
    ps2 = pd.create_pilot_store("affinity2", base_dir)
    ps2.register_file(saga.url("file://localhost" + os.getcwd()+"/test/data1/test3.txt"))
    
    # move store to other resource
    #ps3 = ps1.copy("affinity3", "file://localhost" + os.getcwd()+"/test/data2/")
    
    ps1.move("file://localhost" + os.getcwd()+"/test/data2/")
    ps1.move("file://localhost" + os.getcwd()+"/test/data1/")
    
    
    print "Pilot Store contains the following pilot data containers: "
    for ps_element in pd:
        print str(ps_element) + "@" + ps_element.get_resource()
        
    print "Start some BigJob w/ affinity"
   
    try:
        resource_list = []
        resource_list.append( {"resource_url" : "fork://localhost/", "number_nodes" : "64", "allocation" : "<your allocation>", 
                               "queue" : "workq", "bigjob_agent": (BIGJOB_HOME+"/bigjob_agent_launcher.sh"), 
                               "affinity" : "affinity1"})

        print "Create manyjob service "
        mjs = many_job_affinity.many_job_affinity_service(resource_list, "localhost")
        
        jobs = []
        job_start_times = {}
        job_states = {}
        cwd = os.getcwd()
        for i in range(0, NUMBER_JOBS):
            # create job description
            jd = saga.job.description()
            jd.executable = "/bin/date"
            jd.number_of_processes = "1"
            jd.spmd_variation = "single"
            jd.arguments = [""]
            jd.working_directory = "/home/luckow"
            jd.output =  os.getcwd() + "/stdout-" + str(i) + ".txt"
            jd.error = os.getcwd() + "/stderr-" + str(i) + ".txt"
            jd.environment = ["affinity=affinity1"]
            subjob = mjs.create_job(jd)
            subjob.run()
            print "Submited sub-job " + "%d"%i + "."
            jobs.append(subjob)
            job_start_times[subjob]=time.time()
            job_states[subjob] = subjob.get_state()
        print "************************ All Jobs submitted ************************"
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
    except:
        traceback.print_exc(file=sys.stdout)
        try:
            mjs.cancel()
        except:
            pass
        
    
    del pd #deletes file from resource
    
