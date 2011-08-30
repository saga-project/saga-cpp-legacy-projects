#!/usr/bin/env python


#############################################################
# SAGA ManyJob 
#############################################################


print("############################################################")
print("Make sure you are running this example on head node")
print("############################################################")

# import required packages and check for their availability
try:
    import sys
    import getopt
    import saga
    import time
    import pdb
    import os
    import traceback
    import logging
    import sys
except:
    print("Error: Libraries are missing. Please check SAGA Installation, SAGA Python bindings and other imported packages")
    print(" Error :Please launch python; import saga; if it works then python bindings & saga installation is successful")
    sys.exit(0);

#############################################################
#  Set BigJob Path
#############################################################
if os.getenv("BIGJOB_HOME")!=None:
    BIGJOB_HOME= os.getenv("BIGJOB_HOME") 
else:
    BIGJOB_HOME= os.getcwd() + "/../"
    os.environ["BIGJOB_HOME"]=BIGJOB_HOME

sys.path.insert(0, BIGJOB_HOME)
try:
    from bigjob import *
    import many_job
except:
    print("Error: SAGA BigJob & ManyJob modules are not found")

#############################################################
# Set number of jobs
#############################################################

NUMBER_JOBS=40

#############################################################
# Grid certificate validation
#############################################################
grid_validity=os.system("globus-job-run localhost /bin/date > /dev/null 2>&1")
if grid_validity != 0:
    print(" Grid proxy not initated. Please use grid-proxy-init to initate grid proxy")
    sys.exit(0);

############################################################
# Functions used to determine state of jobs
############################################################

def has_finished(state):
        state = state.lower()
        if state=="done" or state=="failed" or state=="canceled":
            return True
        else:
            return False



if __name__ == "__main__":
    try:

        print " Number of jobs submitted " + str(NUMBER_JOBS) + " jobs."
        starttime=time.time()

        resource_list = []
        #resource_list.append( {"resource_url" : "gram://qb1.loni.org/jobmanager-pbs", "number_nodes" : "64", "allocation" : "<your allocation>", "queue" : "workq", "bigjob_agent": (os.getcwd() + "/bigjob_agent_launcher.sh") , "working_directory": (os.getcwd() + "/agent"), "walltime":10 })
        #resource_list.append({"resource_url" : "gram://oliver1.loni.org/", "number_nodes" : "1", "processes_per_node":"4", "allocation" : None, "queue" : None, "bigjob_agent": (BIGJOB_HOME + "/bigjob_agent_launcher.sh"), "working_directory": (os.getcwd() + "/agent"), "walltime":30 }) 
        resource_list.append({"resource_url" : "gram://eric1.loni.org/", "number_nodes" : "1", "processes_per_node":"4", "allocation" : None, "queue" : None, "bigjob_agent": (BIGJOB_HOME + "/bigjob_agent_launcher.sh"), "working_directory": (os.getcwd() + "/agent"), "walltime":30 }) 

       
        print "Create manyjob service "
        pdb.set_trace()
        mjs = many_job.many_job_service(resource_list, "advert.cct.lsu.edu")
        jobs = []
        job_start_times = {}
        job_states = {}
        job_ids={}
        cwd = os.getcwd()
        for i in range(0, NUMBER_JOBS):
            # create job description
            jd = saga.job.description()
            jd.executable = "/bin/date"
            jd.number_of_processes = "1"
            jd.spmd_variation = "single"
            if i%2 == 0:
                jd.arguments = ["/home/pmantha/hi0"]
            jd.working_directory = "/home/pmantha/"
            jd.output =  "stdout-" + str(i) + ".txt"
            jd.error = "stderr-" + str(i) + ".txt"
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
            print("JOB_ID     Status           Machine       Job_Attributes")
            print("==========================================================")
            for i in range(0, NUMBER_JOBS):
                old_state = job_states[jobs[i]]
                state = jobs[i].get_state()
                if result_map.has_key(state) == False:
                    result_map[state]=0
                result_map[state] = result_map[state]+1
                k = str(jobs[i])
                if k is not "None":
                    j = (k.split("/"))[6]
                    jobid = (j.split("-"))[0]
		    machine = (k.split("/"))[5] 
                    exe = jobs[i].get_exe() 
		    arguments = jobs[i].get_arguments()
		    strings = str(jobid) + "   " +  "%7s     %10s %10s,%6s" % (str(state),str(machine),str(exe), str(arguments))
                    print(strings)
                ##print "counter: " + str(i) + " job: " + str(jobs[i]) + " state: " + state
                #if old_state != state:
                    #print "Job " + str(jobs[i]) + " changed from: " + old_state + " to " + state
                #if old_state != state and has_finished(state)==True:
                     #print "Job: " + str(jobs[i]) + " Runtime: " + str(time.time()-job_start_times[jobs[i]]) + " s."
                if has_finished(state)==True:
                     finish_counter = finish_counter + 1
                job_states[jobs[i]]=state
                
            print "Current states: " + str(result_map) 
            time.sleep(5)
            if finish_counter == NUMBER_JOBS:
                break

        print(" All Jobs completed. Cleaning up the bigjob..." )
        mjs.cancel()
        runtime = time.time()-starttime
        print "Runtime: " + str(runtime) + " s; Runtime per Job: " + str(runtime/NUMBER_JOBS)
    except:
        traceback.print_exc(file=sys.stdout)
        try:
            k=0
            mjs.cancel()
        except:
            pass

