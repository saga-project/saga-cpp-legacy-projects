""" Example application demonstrating job submission via bigjob 
    diane implementation of BigJob is used
"""

import saga
import os
import time
import sys

# BigJob implementation can be swapped here by importing another implementation,
# e.g. condor, cloud, azure, diane
BIGJOB_HOME= "/home/marksant/proj/bigjob/branches/bigjob_overhaul"
sys.path.append(BIGJOB_HOME)

import troy

# configurationg
#RESOURCE_URL = "gram://louie1.loni.org/jobmanager-pbs"
#DEPLOYMENT_LOCATION = 'gsiftp://louie1.loni.org/work/marksant/diane'
#RESOURCE_URL = "gram://painter1.loni.org/jobmanager-pbs"
#DEPLOYMENT_LOCATION = 'gsiftp://painter1.loni.org/work/marksant/diane'
#RESOURCE_URL = "gram://eric1.loni.org/jobmanager-pbs"
#DEPLOYMENT_LOCATION = '/work/marksant/saga'
#DEPLOYMENT_LOCATION = 'gsiftp://eric1.loni.org/work/marksant/diane'
#RESOURCE_URL = "gram://poseidon1.loni.org/jobmanager-pbs"
#DEPLOYMENT_LOCATION = 'gsiftp://poseidon1.loni.org/work/marksant/diane'
#RESOURCE_URL = "fork://localhost"
#DEPLOYMENT_LOCATION = '/home/marksant/proj/bigjob/branches/bigjob_overhaul/examples/tmp'


""" Test Job Submission via SAGA """
if __name__ == "__main__":
    ###########################################################################

    print ('SAGA API Version: 0x%x' % (saga.get_api_version()))
    print ('SAGA Package version: 0x%x' % (saga.get_version()))

    # Start BigJob
    bj = troy.Bigjob()

    jd = troy.bj_description()
    RESOURCE_URL = "gram://poseidon1.loni.org/jobmanager-pbs"
    DEPLOYMENT_LOCATION = 'gsiftp://poseidon1.loni.org/work/marksant/diane'
    jd.set_attribute('NumberOfProcesses', '1') # total number of agents
    jd.set_attribute('ProcessesPerHost', '1') # Ignored?
    jd.set_attribute('Queue', 'workq')
    jd.set_vector_attribute('JobProject', ['randomstring'])
    jd.set_attribute('WorkingDirectory', DEPLOYMENT_LOCATION)
    jd.set_attribute('WallTimeLimit', '12')
    bj.add_resource(troy.bigjob_type.DIANE, RESOURCE_URL, jd)

    jd = troy.bj_description()
    RESOURCE_URL = "gram://eric1.loni.org/jobmanager-pbs"
    DEPLOYMENT_LOCATION = '/work/marksant/saga'
    jd.set_attribute('NumberOfProcesses', '1') # total number of agents
    jd.set_attribute('ProcessesPerHost', '1') # Ignored?
    jd.set_attribute('Queue', 'workq')
    jd.set_vector_attribute('JobProject', ['randomstring'])
    jd.set_attribute('WorkingDirectory', DEPLOYMENT_LOCATION)
    jd.set_attribute('WallTimeLimit', '12')
    bj.add_resource(troy.bigjob_type.SAGA, RESOURCE_URL, jd)

    print "Pilot Job/BigJob URL: ", bj.list_resources()


    #######################################################################
    # Submit UoW through BigJob
    uows = []
    WORKING_DIRECTORY = '/work/marksant/saga'
    for i in range (15):
        uowd = troy.uow_description()
        uowd.set_attribute('Executable', '/bin/hostname')
        uowd.set_attribute('NumberOfProcesses', '1')
        uowd.set_attribute('SPMDVariation', 'single')
        uowd.set_attribute('WorkingDirectory', WORKING_DIRECTORY)
        uowd.set_attribute('Output', 'stdout.txt')
        uowd.set_attribute('Error', 'stderr.txt')

        uowd.set_vector_attribute('Arguments', \
            ['-f', '>', 'hostname%s.txt'%i, ';', '/bin/sleep', '60'])
        uows.append(bj.assign_uow(uowd))
    
    # busy wait for completion
    while 1:
        uow_states = set()
        for i in range(len(uows)):
            uow_states.add(uows[i].get_state())
        print 'UoW state:', uow_states
        if set([saga.job.job_state.New,saga.job.job_state.Unknown, \
                saga.job.job_state.Running]).isdisjoint(uow_states):
            break
        try:
            time.sleep(5)
        except:
            break
            

    ######################################################################
    # Cleanup - stop BigJob
    bj.cancel()
