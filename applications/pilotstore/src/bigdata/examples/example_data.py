import sys
import os
import time
import logging
logging.basicConfig(level=logging.DEBUG)

sys.path.append(os.path.join(os.path.dirname(__file__), "../.."))
from bigdata.manager.pilotstore_manager import PilotStoreService
from bigdata.manager.pilotdata_manager import PilotDataService
from bigdata.troy.data.api import State

if __name__ == "__main__":        
    
    # What files? Create Pilot Data Description
    base_dir = "/Users/luckow/workspace-saga/applications/pilot-store/test/data1"
    url_list = os.listdir(base_dir)
    # make absolute paths
    absolute_url_list = [os.path.join(base_dir, i) for i in url_list]
        
        
    pilot_data_description = {"file_urls":absolute_url_list}
    logging.debug("Pilot Data Description: \n%s"%str(pilot_data_description))
    
    # create pilot data service
    pilot_data_service = PilotDataService()
    pd = pilot_data_service.submit_pilot_data(pilot_data_description)
    
    # create pilot store service (factory for pilot stores (physical, distributed storage))
    pilot_store_service = PilotStoreService()
    ps = pilot_store_service.create_pilotstore({
                                'service_url': "ssh://localhost/tmp/pilotstore/",
                                'size':100                  
                                })
    
    # add resources to pilot data service    
    pilot_data_service.add_pilot_store_service(pilot_store_service) 
    
    logging.debug("Finished setup of PSS and PDS. Waiting for scheduling of PD")
    
    while pd.get_state() != State.Done:
        state = pd.get_state()
        print "PD URL: %s State: %s"%(pd, state)
        if state==State.Running:
            break
        time.sleep(2)  
    
    logging.debug("Terminate Pilot Data/Store Service")
    pilot_data_service.cancel()
    pilot_store_service.cancel()