""" Example application demonstrating job submission via bigjob 
    advert implementation of BigJob is used
"""

import sys
sys.path.append("..")
sys.path.append("../cloud")

import saga
import os
import bigjob
import time

advert_host = "fortytwo.cct.lsu.edu"

""" Test Job Submission via Advert """
if __name__ == "__main__":

    start=time.time()
    # Parameter for BigJob
    re_agent = "/home/luckow/src/bigjob/bigjob_agent_launcher.sh" # path to agent
    nodes = 32 # number nodes for agent
    lrms_url = "gram://qb1.loni.org/jobmanager-pbs" # resource url
    #lrms_url = "gram://qb1.loni.org/jobmanager-fork" # resource url
    project = "" #allocation
    queue = "workq" # queue (PBS)
    workingdirectory="/home/luckow/"  # working directory
    userproxy = None # userproxy (not supported yet due to context issue w/ SAGA)

    # start Glide-In job (Replica-Agent)
    print "Start Glide-In at: " + lrms_url
    bj_tg = bigjob.bigjob(advert_host)
    bj_tg.start_pilot_job(lrms_url,
                           re_agent,
                           nodes,
                           queue,
                           project,
                           workingdirectory, 
                           userproxy,
                           "120")
    
    while bj_tg.get_state_detail()!="Failed" or bj_tg.get_state_detail()!="Done" or bj_tg.get_state_detail()!="Running":
        print "BigJob URL: " + bj_tg.pilot_url + " State: " + str(bj_tg.get_state_detail())
        time.sleep(10)

    print "BigJob start time:" + str(time.time()-start)

    # submit sub-job through big-job
    jd = saga.job.description()
    jd.executable = "/usr/local/packages/namd-2.6-mvapich-1.0-intel10.1/namd2"
    jd.number_of_processes = "32"
    jd.spmd_variation = "mpi"
    jd.arguments = ["/home/luckow/run/NPT.conf"]
    # !!Adjust!!
    jd.working_directory = "/home/luckow/run/"
    jd.output = "output.txt"
    jd.error = "error.txt"
    subjob = bigjob.subjob(advert_host)
    subjob.submit_job(bj_tg.pilot_url, jd)
    
    # busy wait for completion
    while 1:
        state = str(subjob.get_state())
        print "state: " + state
        if(state=="Failed" or state=="Done"):
            break
        time.sleep(10)

    #Cleanup
    bj_tg.cancel()
    print "Runtime: " + str(time.time()-start)
