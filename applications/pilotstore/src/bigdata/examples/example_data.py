import sys
import os
import time
import logging
logging.basicConfig(level=logging.DEBUG)

sys.path.append(os.path.join(os.path.dirname(__file__), "../.."))
from bigdata import *

if __name__ == "__main__":        
    
    # What files? Create Pilot Data Description using absolute URLs
    base_dir = "/Users/luckow/workspace-saga/applications/pilot-store/test/data1"
    url_list = os.listdir(base_dir)
    # make absolute paths
    absolute_url_list = [os.path.join(base_dir, i) for i in url_list]
    pilot_data_description1 = {
                               "file_urls":absolute_url_list,
                               'affinity_datacenter_label': "eu-de-south",              
                               'affinity_machine_label': "mymachine-1" 
                               }
    logging.debug("Pilot Data Description 1: \n%s"%str(pilot_data_description1))
    
    
    # What files? Create Pilot Data Description using remote SSH URLs
    # make remotete paths
    remote_url_list = ["ssh://localhost"+os.path.join(base_dir, i) for i in url_list]
    pilot_data_description2 = {
                               "file_urls":remote_url_list,
                               'affinity_datacenter_label': "eu-de-south",              
                               'affinity_machine_label': "mymachine-2" 
                              }
    
    logging.debug("Pilot Data Description 2: \n%s"%str(pilot_data_description2))
        
    
    # create pilot data service
    pilot_data_service = WorkDataService()
    
    # create pilot store service (factory for pilot stores (physical, distributed storage))
    pilot_store_service = PilotStoreService()
    ps1 = pilot_store_service.create_pilotstore({
                                'service_url': "ssh://localhost/tmp/pilotstore-1/",
                                'size':100,
                               'affinity_datacenter_label': "eu-de-south",              
                               'affinity_machine_label': "mymachine-1"
                                })
    
    ps2 = pilot_store_service.create_pilotstore({
                                'service_url': "ssh://localhost/tmp/pilotstore-2/",
                                'size':100,
                               'affinity_datacenter_label': "eu-de-south",              
                               'affinity_machine_label': "mymachine-2"
                                })
    
    
    
    # add resources to pilot data service    
    pilot_data_service.add_pilot_store_service(pilot_store_service) 
    
    ###########################################################################
    # PD1 should only be scheduled to machine 1
    # PD2 should only be scheduled to machine 2
    pd1 = pilot_data_service.submit_pilot_data(pilot_data_description1)
    pd2 = pilot_data_service.submit_pilot_data(pilot_data_description2)
    
    
    logging.debug("Finished setup of PSS and PDS. Waiting for scheduling of PD")
    
    while True:
        state1 = pd1.get_state()
        print "PD URL: %s State: %s"%(pd1, state1)
        
        state2 = pd2.get_state()
        print "PD URL: %s State: %s"%(pd2, state2)
        
        if state1==State.Running and state2==State.Running:
            break
        time.sleep(2)  
    
    logging.debug("Export files of PD")
    pd1.export("/tmp/pilot-store-export/pd1/")
    pd2.export("/tmp/pilot-store-export/pd2/")
        
    print "***************************************************************"
    print "To reconnect to Pilot Data 1 use the following URL: %s"%pd1.url
    print "Run:\n\n " + sys.executable + " example_data_reconnect.py %s"%pd1.url
    print "\n\n******************* SLEEPING *********************************"
    time.sleep(1200)
    
    logging.debug("Terminate Pilot Data/Store Service")
    pilot_data_service.cancel()
    pilot_store_service.cancel()