""" Example application demonstrating job submission via bigjob 
    advert_job implementation of BigJob is used
"""

import saga
import os
import advert_job
import time
import pdb

advert_host = "fortytwo.cct.lsu.edu"

""" Test Job Submission via Advert """
if __name__ == "__main__":

    # Parameter for BigJob
    re_agent = os.getcwd() + "/advert_launcher.sh" # path to agent
    nodes = 1 # number nodes for agent
    lrms_url = "fork://localhost" # resource url
    workingdirectory="/Users/luckow/workspace/REMD/bigjob/agent"  # working directory
    userproxy = None # userproxy (not supported yet due to context issue w/ SAGA)

    # start Glide-In job (Replica-Agent)
    print "Start Glide-In " + re_agent + " at: " + lrms_url
    advert_glidin_job = advert_job.advert_glidin_job(advert_host)
    advert_glidin_job.start_glidin_job(lrms_url,
                                       re_agent,
                                       nodes,
                                       None,
                                       None,
                                       workingdirectory, 
                                       userproxy,
                                       None)
    print "BigJob URL: " + advert_glidin_job.glidin_url + " State: " + str(advert_glidin_job.get_state())

    # submit sub-job through big-job
    jd = saga.job.description()
    jd.executable = "/bin/date"
    jd.number_of_processes = "1"
    jd.spmd_variation = "single"
    jd.arguments = [""]
    jd.working_directory = os.getcwd() 
    jd.output = "stdout.txt"
    jd.error = "stderr.txt"
    
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
