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

sys.path.append(BIGJOB_HOME+"/diane")
from bigjob_diane_frontend import bigjob_diane as bigjob, subjob

# configurationg
#advert_host = "localhost"


""" Test Job Submission via Advert """
if __name__ == "__main__":
    ###########################################################################
    print "Bigjob/Diane"

    # Start BigJob
    # Parameter for BigJob
    bigjob_agent = BIGJOB_HOME + "diane/bigjob_agent_launcher.sh"
    nodes = 1 # number nodes for agent
    lrms_url = "fork://localhost" # resource url
    workingdirectory=os.getcwd() +"/agent"  # working directory for agent
    userproxy = None # userproxy (not supported yet due to context issue w/ SAGA)

    # start pilot job (bigjob_agent)
    print "Start Pilot Job/BigJob: " + bigjob_agent + " at: " + lrms_url
    bj = bigjob()
    bj.start_pilot_job(lrms_url,
                            bigjob_agent,
                            nodes,
                            None,
                            None,
                            workingdirectory, 
                            userproxy,
                            None,
                            1)
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
    sj = subjob()
    sj.submit_job(bj.pilot_url, jd)
    
    # busy wait for completion
    while 1:
        state = str(sj.get_state())
        print "Subjob state: " + state
        if(state=="Failed" or state=="Done"):
            break
        time.sleep(10)

    ######################################################################
    # Cleanup - stop BigJob
    bj.cancel()
