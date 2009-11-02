""" Example application demonstrating job submission via bigjob 
    advert_job implementation of BigJob is used
"""

import saga
import os
import bigjob_cloud
import time
import pdb


NUMBER_JOBS=1

""" Test Job Submission of NAMD via Cloud BigJob """
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
                       walltime=300,
                       cloud_type="EC2")
    print "Pilot Job/BigJob URL: " + bj.pilot_url + " State: " + str(bj.get_state())

    ##########################################################################################
    # Submit SubJob through BigJob
    # NAMD command: $NAMD_HOME/charmrun ++verbose ++remote-shell ssh ++nodelist nodefile +p4 /usr/local/namd2/namd2 NPT.conf
    # working directory: $HOME/run       
    jd = saga.job.description()
    jd.executable = "/usr/local/NAMD_2.7b1_Linux-x86/charmrun"
    #jd.executable = "/bin/date"
    jd.number_of_processes = "1"
    jd.spmd_variation = "single"
    jd.arguments = ["++remote-shell", "ssh", "++nodelist", "/root/machinefile", "+p1", "/usr/local/NAMD_2.7b1_Linux-x86/namd2", "/root/run/NPT.conf"]
    #jd.working_directory = "/root/run/"
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
    #bj.cancel()
