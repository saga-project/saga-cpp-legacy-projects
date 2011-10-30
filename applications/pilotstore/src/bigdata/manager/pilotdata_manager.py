""" TROY PilotData 
""" 
import sys
import os
import logging
logging.basicConfig(level=logging.DEBUG)

import uuid
import random
import threading
import time
import pdb
import Queue


import saga
sys.path.append(os.path.join(os.path.dirname(__file__), "../.."))

from bigdata.coordination.ssh import BigDataCoordination
from bigdata.troy.data.api import PilotData, DataUnit, PilotDataService, State
from bigdata.manager.pilotstore_manager import *
from bigdata.scheduler.random_scheduler import Scheduler

PILOTDATA_URL_SCHEME="pilotdata://"
DATA_UNIT_URL_SCHEME="dataunit://"

#
# TROY PilotDataService
# 
class PilotDataService(PilotDataService):
    """ TROY PilotDataService (PDS).   
    """

    # Class members
    __slots__ = (
        'id',                # id to this PilotDataService
        'state',             # Status of the PilotDataService
        'pilot_store_services',      # PilotStores connected to this PD
        'pilot_data',         # List of PDs
        'scheduler',           # Scheduler
        'queue',
        'stop',
        'scheduler_thread'
    )

    def __init__(self, pds_id=None):
        """ Create a PilotDataService.

            Keyword arguments:
            pds_id -- Reconnect to an existing PilotDataService 
        """
        self.id=uuid.uuid1()
        self.pilot_data={}
        self.pilot_store_services=[]
        self.scheduler = Scheduler()
        self.queue = Queue.Queue()
        self.stop=threading.Event()
        self.scheduler_thread=threading.Thread(target=self.__schedule_pd_thread)
        self.scheduler_thread.start()

    def add(self, pss):
        """ Add a PilotStoreService 

            Keyword arguments:
            pss -- The PilotStoreService to add.

            Return:
            None
        """
        self.pilot_store_services.append(pss)

    
    def remove(self, pss):
        """ Remove a PilotStoreService 
            
            Keyword arguments:
            pss -- The PilotStoreService to remove 
            
            Return:
            None
        """
        self.pilot_store_services.remove(pss)
    
    
    def list_pilotstores(self):
        """ List all PDs of PDS """
        return self.pilot_store_services
    
    
    def list_pilotdata(self):
        """ List all PDs of PDS """
        return self.pilot_data.items()
    
    
    def get_pilotdata(self, pd_id):
        if self.pilot_data.has_key(pd_id):
            return self.pilot_data[pd_id]
        return None
    
    
    def create_pilotdata(self, pilot_data_description):
        """ creates a pilot data object and binds it to a physical resource (a pilotstore) """
        pd = PilotData(self, pilot_data_description)
        self.pilot_data[pd.id]=pd
        self.queue.put(pd)
        return pd
    
    def cancel(self):
        """ Cancel the PDS. 
            All associated PD objects are deleted and removed from the associated pilot stores.            
            
            Keyword arguments:
            None

            Return:
            None
        """
        # terminate background thread
        self.stop.set()
    
        
    def __schedule_pd(self, pd):
        """ Schedule PD to a suitable pilot store
        
            Currently one level of scheduling is used:
                1.) Add all resources managed by PSS of this PSS
                2.) Select one resource
        """ 
        ps = [i.list_pilotstores() for i in self.pilot_store_services]
        #ps.append(i.list_pilotstores())
        #pdb.set_trace()
        self.scheduler.set_pilot_stores(ps)
        selected_pilot_store = self.scheduler.schedule()
        return selected_pilot_store
    
    def __schedule_pd_thread(self):
        while True and self.stop.isSet()==False:
            #pdb.set_trace()
            pd = self.queue.get()  
            # check whether this is a real pd object  
            if isinstance(pd, PilotData):
                ps=self.__schedule_pd(pd)                
                if(ps!=None):
                    ps.put_pd(pd)
                    logging.debug("Transfer to PS finished.")
                    pd.update_state(State.Running)
                    pd.add_pilot_store(ps)                    
                else:
                    self.queue.put(pd)
            time.sleep(5)        

        logging.debug("Re-Scheduler terminated")

    
    def __repr__(self):
        return str(self.id)


    def __del__(self):
        self.cancel()
        
    

class PilotData(PilotData):
    """ TROY PilotData. Holds a set of data units.
    
        State model:
            New: PD object created
            Pending: PD object is currently updated  
            Running: At least 1 replica of PD is persistent in a pilot store            
    """

    # Class members
    __slots__ = (
        'id',                  # Reference
        'url',                  # url for referencing the store 
        'pilot_data_service',  # Reference to Pilot Data Service
        'description',      # Description
        'state',            # State
        'data_units',        # DU managed by PilotData object
        'pilot_stores'      # List of pilot stores that store a replica of PD        
    )

    def __init__(self, pilot_data_service, pilot_data_description):
        self.id = uuid.uuid1()
        self.url = PILOTDATA_URL_SCHEME + "localhost/" + str(self.id)
        self.description = pilot_data_description        
        self.pilot_data_service = pilot_data_service
        self.pilot_stores=[]
        self.data_units = DataUnit.create_data_unit_list(self.description["file_urls"]) 
        self.state = State.New
        
    def cancel(self):
        """ Cancel the PD. """
        self.state = State.Done    
            
    def add_data_units(self, data_units):
        self.data_units.append(data_units)    
    
    def remove_data_unit(self, data_unit):
        self.data_units.remove(data_unit)
    
    def list_data_units(self):
        return self.data_units
        
    def get_state(self):
        return self.state    
    
    def __repr__(self):
        repr_dict = {
                     "url": self.url,
                    }        
        
        ps = []
        for i in self.pilot_stores:
            ps.append(i.url_for_pd(self))        
        repr_dict["pilot_stores"]=ps            
        return str(repr_dict)
    
    
    ###########################################################################
    # BigData Internal Methods
    def update_state(self, state):
        self.state=state
        
    def add_pilot_store(self,pilot_store):
        self.pilot_stores.append(pilot_store) 
    
    
    

class DataUnit(DataUnit):
    """ TROY DataUnit """

    __slots__ = (
        'id',        
        'url',      # url in PD container
        'local_url' # local url of file
    )
    
    def __init__(self, local_url):
        self.local_url = local_url        
        
    @classmethod    
    def __exists_file(cls, url):   
        """ return True if file at url exists. Otherwise False """
        file_url = saga.url(url)
        if file_url.host == "":
            if os.path.exists(str(file_url)):
                return True
            else:
                return False            
        elif file_url.host=="localhost":
            if os.path.exists(file_url.path):
                return True
            else:
                return False
        else:
            # TODO check for remote existences of file
            return True
         
         
    @classmethod
    def create_data_unit_list(cls, urls):
        """ Creates a list of DUs from URL list
        """    
        du_list = []    
        for i in urls:            
            if DataUnit.__exists_file(i):
                du = DataUnit(i)
                du_list.append(du)
    
        return du_list
    
###############################################################################
    
if __name__ == "__main__":        
    
    # What files? Create Pilot Data Description
    base_dir = "/Users/luckow/workspace-saga/applications/pilot-store/test/data1"
    url_list = os.listdir(base_dir)
    absolute_url_list = []
    for i in url_list:
        if os.path.isdir(i)==False:
            absolute_url_list.append(os.path.join(base_dir, i))
    pilot_data_description = {"file_urls":absolute_url_list}
    
    # create pilot data service
    pilot_data_service = PilotDataService()
    pd = pilot_data_service.create_pilotdata(pilot_data_description)
    
    # create pilot store service (factory for pilot stores (physical, distributed storage))
    pilot_store_service = PilotStoreService()
    ps = pilot_store_service.create_pilotstore({
                                'service_url': "ssh://localhost/tmp/pilotstore/",
                                'size':100                                
                                })
    
    # add resources to pilot data service    
    pilot_data_service.add(pilot_store_service) 
    
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
    
    
    
    
    