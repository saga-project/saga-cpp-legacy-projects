"""
Implementation of PilotStoreService and PilotDataService (incl. associated objects)
"""
import os
import sys
import uuid
sys.path.append(os.path.join(os.path.dirname(__file__), "../.."))

import logging

from bigdata.coordination.ssh import BigDataCoordination
from bigdata.troy.data.api import PilotStore, PilotStoreService


class PilotStore(PilotStore):
    """ TROY PilotStore. """   
            

    # Class members
    __slots__ = (
        'id',           # Unique identity
        'service_url',  # Resource  URL              
        'state',        # State of the PilotStore
        'pilot_data',   # pilot_data
        '__coordination'
    )
    
    def __init__(self, service_url, size):    
        """ 
            Initialize PilotStore at given service url:
            
            ssh://<hostname>
            gsissh://<hostname>
            
            Currently only ssh schemes are supported. In the future all 
            SAGA URL schemes/adaptors should be supported.        
        """
            
        self.id = uuid.uuid1()
        self.service_url=service_url
        self.pilot_data={}
        self.__coordination = BigDataCoordination(service_url)
        self.__coordination.initialize_pilotstore()
        self.__coordination.get_pilotstore_size()
                

    def cancel(self):        
        """ Cancel PilotStore 

            Keyword arguments:
            None
        """
        self.__coordination.delete_pilotstore()
        
        
    def url_for_pd(self, pd):
        if self.pilot_data.has_key(pd.id):
            return self.service_url + "/" + str(pd.id)
        return None
    
    
    def put_pd(self, pd):
        logging.debug("Put PD: %s to PS: %s"%(pd.id,self.service_url))
        self.__coordination.create_pd(pd.id)
        self.__coordination.put_pd(pd)
        self.pilot_data[pd.id] = pd
        
        
    def remove_pd(self, pd):
        """ Remove pilot data from pilot store """
        if self.pilot_data.has_key(pd.id):
            self.__coordination.remove_pd(pd)
            del self.pilot_data[pd.id]
                
        
    def get_state(self):
        return self.__coordination.get_state()
    
    
    def __repr__(self):
        return self.service_url
    

class PilotStoreService(PilotStoreService):
    """ TROY PilotStoreService (PSS)."""
    

    # Class members
    __slots__ = (
        'id',             # Reference to this PJS
        'state',          # Status of the PJS
        'pilot_stores'    # List of PJs under this PJS
        'affinity_list'   # List of PS on that are affine to each other
    )

    def __init__(self, pss_id=None):
        """ Create a PilotStoreService

            Keyword arguments:
            pss_id -- restore from pss_id
        """
        self.pilot_stores={}
       

    def create_pilotstore(self, pilotstore_desc):
        """ Create a PilotStore 

            Keyword arguments:
            pilotstore_desc -- PilotStore Description    
            {
                'service_url': "ssh://<hostname>/base-url/"                
                'size': "1000"
            }
            Return value:
            A PilotStore handle
        """
        ps = PilotStore(pilotstore_desc["service_url"], pilotstore_desc["size"])
        self.pilot_stores[ps.id]=ps
        return ps
    
    def get_pilotstore(self, ps_id):
        if self.pilot_stores.has_key(ps_id):
            return self.pilot_stores[ps_id]
        return None

    def list_pilotstores(self):
        """ List all PSs of PSS """
        return self.pilot_stores.values()
    

    def cancel(self):
        """ Cancel the PilotStoreService.
            
            Keyword arguments:
            None

            Return value:
            Result of operation
        """
        for i in self.pilot_stores.values():
            i.cancel()
    
    
    def __del__(self):
        self.cancel()         
            


            
            
if __name__ == "__main__":        
    pss = PilotStoreService()
    ps = pss.create_pilotstore({
                                'service_url': "ssh://localhost/tmp/pilotstore/",
                                'size':100                                
                                })
    print ps.get_state()
    
    
    ps.cancel()
    print ps.get_state()
        
        
        
        
        
            