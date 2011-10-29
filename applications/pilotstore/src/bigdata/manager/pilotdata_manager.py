"""
Implementation of PilotStoreService and PilotDataService (incl. associated objects)
"""
import os
import sys
sys.path.append(os.path.join(os.path.dirname(__file__), "../.."))

from bigdata.coordination.ssh import BigDataCoordination
from bigdata.troy.data.api import PilotStore, PilotStoreService

class PilotStore(PilotStore):
    """ TROY PilotStore.    
            """

    # Class members
    __slots__ = (
        'id',           # Reference to this PJ
        'description',  # Description of PilotStore
        'context',      # SAGA context
        'service_url',  # Resource  URL       
        'state',        # State of the PilotStore
        'state_detail', # Adaptor specific state of the PilotStore       
        '__coordination'
    )
    
    def __init__(self, service_url, size):        
        self.__coordination = BigDataCoordination(service_url)
        self.__coordination.initialize_pilotstore()
        self.__coordination.get_pilotstore_size()        

    def cancel(self):        
        """ Cancel PilotStore 

            Keyword arguments:
            None
        """
        self.__coordination.delete_pilotstore()
        
    def get_state(self):
        return self.__coordination.get_state()
    
    
    

class PilotStoreService(PilotStoreService):
    """ TROY PilotStoreService (PSS).
    """

    # Class members
    __slots__ = (
        'id',           # Reference to this PJS
        'state',       # Status of the PJS
        'pilot_stores'    # List of PJs under this PJS
    )

    def __init__(self, pss_id=None):
        """ Create a PilotStoreService

            Keyword arguments:
            pss_id -- restore from pss_id
        """
        self.pilot_stores=[]
       

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
        self.pilot_stores.append(ps)
        return ps
    
    def get_pilotstore(self, ps_id):
        pass

    def list_pilotstores(self):
        """ List all PSs of PSS """
        return self.pilot_stores
    

    def cancel(self):
        """ Cancel the PilotStoreService.
            
            Keyword arguments:
            None

            Return value:
            Result of operation
        """
        for i in self.pilot_stores:
            i.cancel()
            
            


            
            
if __name__ == "__main__":        
    pss = PilotStoreService()
    ps = pss.create_pilotstore({
                                'service_url': "ssh://localhost/tmp/pilotstore/",
                                'size':100                                
                                })
    print ps.get_state()
    
    ps.cancel()

    print ps.get_state()
        
        
        
        
        
            