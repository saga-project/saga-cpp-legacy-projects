import sys
import os
import time
import logging
import json

logging.basicConfig(level=logging.DEBUG)

sys.path.append(os.path.join(os.path.dirname(__file__), "../.."))
from bigdata import *

if __name__ == "__main__":        
    
    if len(sys.argv)==2:
        reconnect_url=sys.argv[1]
    else:
        print "Usage: " + sys.executable + " " + __file__ + " <Pilot Data URL to Reconnect to>"
        sys.exit(-1)
        
    # create pilot store service (factory for pilot stores (physical, distributed storage))
    pilot_store_service = PilotStoreService()
    ps_new = pilot_store_service.create_pilotstore({
                                'service_url': "ssh://localhost/tmp/pilotstore-reconnect/",
                                'size':100,
                               'affinity_datacenter_label': "eu-de-south",              
                               'affinity_machine_label': "mymachine-1"
                                })
    
    
    logging.debug("Pilot Store URL: %s"%pilot_store_service.url)
    
    
    ###########################################################################
    # PD should only be scheduled to machine 1    
    logging.debug("Connect to PD URL: %s"%reconnect_url)
    pd = PilotData(pd_url=reconnect_url)
    
    #ps_old = pd.get_pilot_stores()[0]
    #ps_old.copy_pd(pd, ps_new)
    
    # Move PD to another pilot store    
    pd.add_pilot_store(ps_new)
    
    #time.sleep(120)
    #pilot_store_service.cancel()