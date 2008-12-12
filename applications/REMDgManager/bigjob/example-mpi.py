""" Example application demonstrating job submission via bigjob 
    advert_job implementation of BigJob is used
"""

import saga
import os
import advert_job
import time

advert_host = "fortytwo.cct.lsu.edu"

""" Test Job Submission via Advert """
if __name__ == "__main__":

    # Parameter for BigJob
    re_agent = os.getcwd() + "/advert_launcher.sh" # path to agent
    nodes = 64 # number nodes for agent
    lrms_url = "gram://qb1.loni.org/jobmanager-pbs" # resource url
    project = "loni_jha_big" #allocation
    queue = "workq" # queue (PBS)
    workingdirectory="/tmp"  # working directory
    userproxy = None # userproxy (not supported yet due to context issue w/ SAGA)

    # start Glide-In job (Replica-Agent)
    print "Start Glide-In at: " + lrms_url
    advert_glidin_job = advert_job.advert_glidin_job(advert_host)
    advert_glidin_job.start_glidin_job(lrms_url,
                                        re_agent,
                                        nodes,
                                        queue,
                                        project,
                                        workingdirectory, 
                                        userproxy,
                                        None)
    print "BigJob URL: " + advert_glidin_job.glidin_url

    # submit sub-job through big-job
    jd = saga.job.description()
    jd.executable = "/home/luckow/src/REMDgManager/bigjob/main"
    jd.number_of_processes = "2"
    jd.spmd_variation = "mpi"
    jd.arguments = [""]
    # !!Adjust!!
    jd.working_directory = "/home/luckow"
    jd.output = "output.txt"
    jd.error = "error.txt"
    advert_job = advert_job.advert_job(advert_host)
    advert_job.submit_job(advert_glidin_job.glidin_url, jd)
    
    # busy wait for completion
    while 1:
        state = str(advert_job.get_state())
        print "state: " + state
        if(state=="Failed" or state=="Done"):
            break
        time.sleep(10)

    #Cleanup
    advert_glidin_job.cancel()
