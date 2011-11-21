import sys
import os
import time
import logging
import pdb
logging.basicConfig(level=logging.DEBUG)

sys.path.insert(0,os.path.join(os.path.dirname(__file__), "../.."))

from bigdata.manager.pilotjob_manager import PilotJobService
from bigdata.manager.pilotstore_manager import PilotStoreService
from bigdata.manager.pstar_manager import WorkDataService
from bigdata.troy.compute.api import State


def has_finished(state):
    state = state.lower()
    if state=="done" or state=="failed" or state=="canceled":
        return True
    else:
        return False

def wait_for_all_jobs(jobs, job_start_times, job_states, poll_intervall=5):
    """ waits for all jobs that are in list to terminate """
    while 1:
        finish_counter=0
        result_map = {}
        number_of_jobs = len(jobs)
        for i in range(0, number_of_jobs):
            old_state = job_states[jobs[i]]
            state = jobs[i].get_state()
            if result_map.has_key(state)==False:
                result_map[state]=1
            else:
                result_map[state] = result_map[state]+1
            if old_state != state:
                print "Job " + str(jobs[i].get_id()) + " changed from: " + old_state + " to " + state
            if old_state != state and has_finished(state)==True:
                print "Job: " + str(jobs[i].get_id()) + " Runtime: " + str(time.time()-job_start_times[jobs[i]]) + " s."
            if has_finished(state)==True:
                finish_counter = finish_counter + 1
            job_states[jobs[i]]=state

        if finish_counter == number_of_jobs:
            break
        time.sleep(2)
 
if __name__ == "__main__":      
    starttime=time.time()

    pilotjobs = []
    pilotjobs.append({ "service_url": 'pbs-ssh://sierra.futuregrid.org',
                       "number_of_processes": 1,                             
                       "working_directory": os.getcwd() + "/agent",
                       "walltime":10,
                       'affinity_datacenter_label': "eu-de-south",              
                       'affinity_machine_label': "mymachine" 
                      })

    pilotjobs.append({ "service_url": 'pbs-ssh://india.futuregrid.org',
                        "number_of_processes": 1,                             
                        "working_directory": os.getcwd() + "/agent",
                        "walltime":10,
                        'affinity_datacenter_label': "eu-de-south",              
                        'affinity_machine_label': "mymachine"
                      })

    pilot_job_ser = []  
    work_data_service = WorkDataService()
    pilot_job_service = PilotJobService()
    for pilot_job_desc in pilotjobs:
        pilotjob = pilot_job_service.create_pilotjob(pilot_job_description=pilot_job_desc)
    work_data_service.add_pilot_job_service(pilot_job_service)

    jobs=[]
    job_start_times = {}
    job_states = {}
    
    for i in range(0,8):
        # start work unit
        work_unit_description = {
            "executable": "/bin/date",
            "arguments": [],
            "total_core_count": 1,
            "number_of_processes": 1,
            "working_directory": os.getcwd(),
            "output": "stdout"+str(i)+".txt",
            "error": "stderr"+str(i)+".txt",   
            "affinity_datacenter_label": "eu-de-south",              
            "affinity_machine_label": "mymachine" 
            }    
        work_unit = work_data_service.submit_work_unit(work_unit_description)
        jobs.append(work_unit)
        print " submitted job -- " + str(i)  
        job_start_times[work_unit]=time.time()
        job_states[work_unit] = work_unit.get_state()

    
    logging.debug("Finished setup. Waiting for scheduling of PD")
    wait_for_all_jobs(jobs, job_start_times, job_states,  5)
    logging.debug(" Finshed jobs ...... " )

    runtime = time.time()-starttime
    print "Runtime: " + str(runtime) + " s; Runtime per Job: " 

    ##########################################################################################
    logging.debug("Terminate Pilot Data/Store Service")
    work_data_service.cancel()    
    pilot_job_service.cancel()
