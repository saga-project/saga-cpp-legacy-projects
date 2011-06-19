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

import bigjob

# configurationg
#RESOURCE_URL = "gram://louie1.loni.org/jobmanager-pbs"
#DEPLOYMENT_LOCATION = 'gsiftp://louie1.loni.org/work/marksant/diane'
#RESOURCE_URL = "gram://painter1.loni.org/jobmanager-pbs"
#DEPLOYMENT_LOCATION = 'gsiftp://painter1.loni.org/work/marksant/diane'
RESOURCE_URL = "gram://eric1.loni.org/jobmanager-pbs"
DEPLOYMENT_LOCATION = 'gsiftp://eric1.loni.org/work/marksant/diane'


""" Test Job Submission via Advert """
if __name__ == "__main__":
    ###########################################################################

    print ('SAGA API Version: 0x%x' % (saga.get_api_version()))
    print ('SAGA Package version: 0x%x' % (saga.get_version()))

    # Start BigJob
    # Parameter for BigJob

    jd = bigjob.bj_description()

    jd.set_attribute('NumberOfProcesses', '1') # total number of agents
    jd.set_attribute('Queue', 'workq')
    #jd.set_attribute('JobProject', 'randomstring')
    jd.set_attribute('WorkingDirectory', DEPLOYMENT_LOCATION)
    jd.set_attribute('WallTimeLimit', '12')
    jd.set_attribute('ProcessesPerHost', '1')

    # start pilot job (bigjob_agent)
    print "Start BigJob/Diane Pilot Job"
    bj = bigjob.Bigjob()
    bj.add_resource(bigjob.bigjob_type.DIANE, RESOURCE_URL, jd)
    #bj.add_resource(bigjob.bigjob_type.Advert, RESOURCE_URL, jd)

    print "Pilot Job/BigJob URL: ", bj.list_resources()


    #######################################################################
    # Submit UoW through BigJob
    uowd = bigjob.uow_description()
    uowd.set_attribute('Executable', '/bin/date')
    uowd.set_attribute('NumberOfProcesses', '1')
    uowd.set_attribute('SPMDVariation', 'single')
    uowd.set_vector_attribute('Arguments', \
            ['-u', '-R', ';', '/bin/sleep', '60'])
    uowd.set_attribute('WorkingDirectory', os.getcwd())
    uowd.set_attribute('Output', 'stdout.txt')
    uowd.set_attribute('Error', 'stderr.txt')

    uow = bj.assign_uow(uowd)
    
    # busy wait for completion
    while 1:
        uow_state = uow.get_state()
        print 'UoW state:', uow_state
        if(uow_state == saga.job.job_state.Failed or \
                uow_state == saga.job.job_state.Done):
            break
        time.sleep(10)

    ######################################################################
    # Cleanup - stop BigJob
    bj.cancel()
