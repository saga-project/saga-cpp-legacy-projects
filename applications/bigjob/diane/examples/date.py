""" Example application demonstrating job submission via bigjob 
    diane implementation of BigJob is used
"""

import saga
import os
import time
import sys

# BigJob implementation can be swapped here by importing another implementation,
# e.g. condor, cloud, azure, diane
BIGJOB_HOME= "/home/marksant/proj/bigjob"
sys.path.append(BIGJOB_HOME)
import api.base

import diane.bigjob_diane_frontend as bigjob

# configurationg
#advert_host = "localhost"


""" Test Job Submission via Advert """
if __name__ == "__main__":
    ###########################################################################
    print "Bigjob/Diane"

    # Start BigJob
    # Parameter for BigJob
    #resource_url = "gram://eric1.loni.org/jobmanager-pbs"
    resource_url = "gram://oliver1.loni.org/jobmanager-pbs"
    bigjob_agent = None
    number_nodes = 1 # number nodes for agent
    queue = None
    project = None
    workingdirectory = "gsiftp://oliver1.loni.org/work/marksant/diane"
    userproxy = None # userproxy (not supported yet due to context issue w/ SAGA)
    walltime = None
    processes_per_node = 1

    # start pilot job (bigjob_agent)
    print "Start BigJob/Diane Pilot Job at: " + resource_url
    bj = bigjob.bigjob()
    bj.start_pilot_job(resource_url,
                            bigjob_agent,
                            number_nodes,
                            queue,
                            project,
                            workingdirectory, 
                            userproxy,
                            walltime,
                            processes_per_node)

    print "Pilot Job/BigJob URL: " + bj.pilot_url + \
            " State: " + str(bj.get_state())

    #######################################################################
    # Submit SubJob through BigJob
    jd = saga.job.description()
    jd.executable = "/bin/date"
    jd.number_of_processes = "1"
    jd.spmd_variation = "single"
    jd.arguments = ["-u", "-R"]
    jd.working_directory = os.getcwd() 
    jd.output = "stdout.txt"
    jd.error = "stderr.txt"
    sj = bigjob.subjob()
    sj.submit_job(bj.pilot_url, jd)
    
    # busy wait for completion
    while 1:
        sj_state = str(sj.get_state())
        bj_state = str(bj.get_state())
        print "Subjob state: " + sj_state
        print "Bigjob state: " + bj_state
        if(sj_state=="Failed" or sj_state=="Done"):
            break
        time.sleep(10)

    ######################################################################
    # Cleanup - stop BigJob
    bj.cancel()
