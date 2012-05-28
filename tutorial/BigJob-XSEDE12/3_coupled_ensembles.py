""" Example application demonstrating how to submit a jobs with PilotJob.  """

import os
import time
import sys
from pilot import PilotComputeService, ComputeDataService, State
	

### This is the number of jobs you want to run
NUMBER_JOBS=4

if __name__ == "__main__":

    start_time=time.time()
    pilot_compute_service = PilotComputeService()
    pilot_compute_description=[]

    pilot_compute_description.append({ "service_url": "ssh://localhost",
                                       "number_of_processes": 12,
                                       "allocation": "TG-MCB090174",
	                               "queue": "normal",
                                       "processes_per_node":12,
                                       "working_directory": os.getcwd()+"/agent",
                                       "walltime":10,
                                       "affinity_machine_label": "mymachine" 
                                     })

    for pcd in pilot_compute_description:
        pilot_compute_service.create_pilot(pilot_compute_description=pcd)

    compute_data_service = ComputeDataService()
    compute_data_service.add_pilot_compute_service(pilot_compute_service)

    print ("Finished Pilot-Job setup. Submitting compute units")

    # submit a set of CUs, call it A
    for i in range(NUMBER_JOBS):
        compute_unit_description = {
                "executable": "/bin/echo",
                "arguments": ["Hello","$ENV1","$ENV2"],
                "environment": {'ENV1':'env_arg1','ENV2':'env_arg2'},
                "total_cpu_count": 4,            
                "spmd_variation":"mpi",
                "output": "A_stdout.txt",
                "error": "A_stderr.txt",
                "affinity_machine_label": "mymachine" 
                }    
        compute_unit = compute_data_service.submit_compute_unit(compute_unit_description)


    # submit a set of CUs, call it B
    for i in range(NUMBER_JOBS):
        compute_unit_description = {
                "executable": "/bin/date",
                "arguments": [],
                "environment": {},
                "total_cpu_count": 1,
                "spmd_variation":"single",
                "output": "B_stdout.txt",
                "error": "B_stderr.txt",
                "affinity_machine_label": "mymachine"
                }
        compute_unit = compute_data_service.submit_compute_unit(compute_unit_description)



    print ("Wait for CUs of task set A & B to complete")
    compute_data_service.wait()


    # submit a set of CUs, call it C
    for i in range(NUMBER_JOBS):
        compute_unit_description = {
                "executable": "/bin/echo",
                "arguments": ["Hello","$ENV1","$ENV2"],
                "environment": {'ENV1':'env_arg1','ENV2':'env_arg2'},
                "total_cpu_count": 1,
                "spmd_variation":"single",
                "output": "C_stdout.txt",
                "error": "C_stderr.txt",
                "affinity_machine_label": "mymachine"
                }
        compute_unit = compute_data_service.submit_compute_unit(compute_unit_description)

    print ("Wait for CUs of task set C to complete")
    compute_data_service.wait()

    print ("Terminate Pilot Jobs")
    compute_data_service.cancel()    
    pilot_compute_service.cancel()
    end_time=time.time()
    print "Total time to solution-" + str(round(end_time-start_time,2))
