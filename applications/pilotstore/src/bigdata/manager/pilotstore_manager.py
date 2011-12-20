"""
Implementation of PilotStoreService and PilotDataService (incl. associated objects)
"""
import os
import sys
import uuid
sys.path.append(os.path.join(os.path.dirname(__file__), "../.."))

import logging

import bigdata
from bigdata.filemanagement.ssh import SSHFileAdaptor
from bigdata.coordination.advert import AdvertCoordinationAdaptor as CoordinationAdaptor



from bigdata.troy.data.api import PilotStore, PilotStoreService


class PilotStore(PilotStore):
    """ TROY PilotStore. """   
    PS_ID_PREFIX="ps-"   

    # Class members
    __slots__ = (
        'id',           # Unique identity
        'url',          # URL as a distributed reference
        'service_url',  # Resource  URL          
        'size',         # max size of allocated storage
        'pilot_store_description', # PS description    
        'state',        # State of the PilotStore
        'pilot_data',   # pilot_data
        '__filemanager',  # File Adaptor (SSH, ...)
        '__coordination'  # Distributed Coordination (Advert,...)
    )
    
    def __init__(self, pilot_store_description=None, ps_url=None):    
        """ 
            Initialize PilotStore at given service url:
            
            ssh://<hostname>
            gsissh://<hostname>
            
            Currently only ssh schemes are supported. In the future all 
            SAGA URL schemes/adaptors should be supported.        
        """ 
        if ps_url==None:      # new ps          
            self.id = self.PS_ID_PREFIX+str(uuid.uuid1())
            self.pilot_data={}
        else:
            self.id = self.__get_ps_id(ps_url)
            self.pilot_data={} #TODO restore state
            
        self.service_url=pilot_store_description["service_url"]
        self.size = pilot_store_description["size"]
        self.pilot_store_description = pilot_store_description
        
        # initialize file adaptor
        self.__filemanager = SSHFileAdaptor(self.service_url)
        self.__filemanager.initialize_pilotstore()
        self.__filemanager.get_pilotstore_size()
                

    def __get_ps_id(self, ps_url):
        start = ps_url.index(self.PS_ID_PREFIX)
        end =ps_url.index("/", start)
        return ps_url[start:end]

    def cancel(self):        
        """ Cancel PilotStore 

            Keyword arguments:
            None
        """
        self.__filemanager.delete_pilotstore()
        
        
    def url_for_pd(self, pd):
        if self.pilot_data.has_key(pd.id):
            return self.service_url + "/" + str(pd.id)
        return None
    
    
    def put_pd(self, pd):
        logging.debug("Put PD: %s to PS: %s"%(pd.id,self.service_url))
        self.__filemanager.create_pd(pd.id)
        self.__filemanager.put_pd(pd)
        self.pilot_data[pd.id] = pd
        
        
    def remove_pd(self, pd):
        """ Remove pilot data from pilot store """
        if self.pilot_data.has_key(pd.id):
            self.__filemanager.remove_pd(pd)
            del self.pilot_data[pd.id]
    
    
    def list_pilotdata(self):           
        return self.pilot_data.values()
    
    
    def get_state(self):
        return self.__filemanager.get_state()
    
    
    def export_pd(self, pd, target_directory):
        self.__filemanager.get_pd(pd, target_directory)
    
    
    def __repr__(self):
        return self.service_url
    

class PilotStoreService(PilotStoreService):
    """ TROY PilotStoreService (PSS)."""
    
    PSS_ID_PREFIX="pss-"

    # Class members
    __slots__ = (
        'id',             # Reference to this PJS
        'url',            # URL for referencing PilotStoreService
        'state',          # Status of the PJS
        'pilot_stores'    # List of PJs under this PJS
        'affinity_list'   # List of PS on that are affine to each other
    )

    def __init__(self, pss_url=None):
        """ Create a PilotStoreService

            Keyword arguments:
            pss_id -- restore from pss_id
        """        
        self.pilot_stores={}
        
        if pss_url == None:
            self.id = self.PSS_ID_PREFIX + str(uuid.uuid1())
            application_url = CoordinationAdaptor.get_base_url(bigdata.application_id)
            self.url = CoordinationAdaptor.add_pss(application_url, self)
        else:
            self.id = self.__get_pss_id(pss_url)
    
    
    def __get_pss_id(self, pss_url):
        start = pss_url.index(self.PSS_ID_PREFIX)
        end =pss_url.index("/", start)
        return pss_url[start:end]
    
    def __restore_ps(self, pss_url):
        ps_list=CoordinationAdaptor.list_ps(pss_url) 
        for i in ps_list:
            ps = PilotStore(ps_url=i)
            self.pi

    def create_pilotstore(self, pilot_store_description):
        """ Create a PilotStore 

            Keyword arguments:
            pilot_store_description -- PilotStore Description    
            {
                'service_url': "ssh://<hostname>/base-url/"                
                'size': "1000"
            }
            Return value:
            A PilotStore handle
        """
        ps = PilotStore(pilot_store_description)
        self.pilot_stores[ps.id]=ps
        
        # store pilot store in central data space
        CoordinationAdaptor.add_ps(self.url, ps)        
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
        
        
        
        
        
            