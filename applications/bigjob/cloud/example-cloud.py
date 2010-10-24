""" Example application demonstrating job submission via bigjob 
    advert_job implementation of BigJob is used
"""

import saga
import os
import bigjob_cloud
import time
import pdb


NUMBER_JOBS=2


""" Test Job Submission via Cloud BigJob """
if __name__ == "__main__":

    ##########################################################################################
    # Start BigJob
    # Parameter for BigJob
    nodes = 1 # number nodes for agent
    current_directory=os.getcwd() +"/agent"  # working directory for agent

    # start pilot job (bigjob_agent)
    print "Start Pilot Job/BigJob in the cloud. "
    bj = bigjob_cloud.bigjob_cloud()
    bj.start_pilot_job(number_nodes=nodes, 
                       working_directory=current_directory,
                       cloud_type="EC2")
    print "Pilot Job/BigJob URL: " + bj.pilot_url + " State: " + str(bj.get_state())

    ##########################################################################################
    # Submit SubJob through BigJob
    jd = saga.job.description()
    jd.executable = "/bin/date"
    jd.number_of_processes = "1"
    jd.spmd_variation = "single"
    jd.arguments = [""]
    jd.working_directory = "$HOME"
    jd.output = "stdout.txt"
    jd.error = "stderr.txt"
    
    jobs = []
    for i in range (0, NUMBER_JOBS):
        print "Start job no.: " + str(i)
        sj = bigjob_cloud.subjob(bigjob=bj)
        sj.submit_job(jd)
        jobs.append(sj)
    
    # busy wait for completion
    while 1:
        try:
            number_done = 0
            for i in jobs:
                state = str(i.get_state())
                print "job: " + str(i) + " state: " + str(state)
                if(state=="Failed" or state=="Done"):
                    number_done = number_done + 1                    
                
            if (number_done == len(jobs)):
                break
            time.sleep(10)
        except KeyboardInterrupt:
            break

    ##########################################################################################
    # Cleanup - stop BigJob
    bj.cancel()
