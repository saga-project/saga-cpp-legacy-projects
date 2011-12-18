import sys
import os
import time
import logging
logging.basicConfig(level=logging.DEBUG)

sys.path.append(os.path.join(os.path.dirname(__file__), "../.."))

from bigdata.manager.pilotjob_manager import PilotJobService
from bigdata.manager.pilotstore_manager import PilotStoreService
from bigdata.manager.pstar_manager import WorkDataService
from bigdata.troy.compute.api import State
 
if __name__ == "__main__":      
    
    pilot_job_service = PilotJobService()

    # create pilot job service and initiate a pilot job
    pilot_job_description = {
                             "service_url": 'fork://localhost',
                             "number_of_processes": 1,                             
                             "working_directory": "/tmp/pilotjob/",
                             'affinity_datacenter_label': "eu-de-south",              
                             'affinity_machine_label': "mymachine-1" 
                            }
    
    pilotjob = pilot_job_service.create_pilotjob(pilot_job_description=pilot_job_description)
    
    
    # create pilot store service (factory for pilot stores (physical, distributed storage))
    # and pilot stores
    pilot_store_service = PilotStoreService()
    pilot_store_description={
                                "service_url": "ssh://localhost/tmp/pilotstore/",
                                "size": 100,   
                                "affinity_datacenter_label": "eu-de-south",              
                                "affinity_machine_label": "mymachine-1"                              
                             }
    ps = pilot_store_service.create_pilotstore(pilot_store_description=pilot_store_description)
     
    work_data_service = WorkDataService()
    work_data_service.add_pilot_job_service(pilot_job_service)
    work_data_service.add_pilot_store_service(pilot_store_service)
    
    # Create Pilot Data Description
    base_dir = "/Users/luckow/workspace-saga/applications/pilot-store/test/data1"
    url_list = os.listdir(base_dir)
    # make absolute paths
    absolute_url_list = [os.path.join(base_dir, i) for i in url_list]
    pilot_data_description = {"file_urls":absolute_url_list}    
      
    
    # submit pilot data to a pilot store    
    pd = work_data_service.submit_pilot_data(pilot_data_description)
    logging.debug("Pilot Data URL: %s Description: \n%s"%(pd, str(pilot_data_description)))
    
    
    # start work unit
    work_unit_description = {
            "executable": "/bin/cat",
            "arguments": ["test1.txt"],
            "total_core_count": 1,
            "number_of_processes": 1,
            "working_directory": pd.url,
            "output": "stdout.txt",
            "error": "stderr.txt",   
            "affinity_datacenter_label": "eu-de-south",              
            "affinity_machine_label": "mymachine-1" 
    }    
    work_unit = work_data_service.submit_work_unit(work_unit_description)
    
    logging.debug("Finished setup of PSS and PDS. Waiting for scheduling of PD")
    while pd.get_state() != State.Done and work_unit != State.Done:
        logging.debug("Check state")
        state_pd = pd.get_state()
        state_wu = work_unit.get_state()
        print "PJS State %s" % pilot_job_service
        print "PD: %s State: %s"%(pd, state_pd)
        print "WU: %s State: %s"%(work_unit, state_wu)
        if state_wu==State.Done and state_pd==State.Running:
            break
        time.sleep(2)  
    
    logging.debug("Terminate Pilot Data/Store Service")
    work_data_service.cancel()
    pilot_store_service.cancel()
    pilot_job_service.cancel()