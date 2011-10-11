""" 
This file contains the API for the (proposed) TROY PilotData Framework.

"""


#
# TROY PilotStore Description
#
class PilotStoreDescription(object):
    """ TROY PilotStoreDescription.
            """

    # Class members
    __slots__ = (
        'resource_url',  # url to job manager (for spawning agent)
        'base_url',      # url to directory
        'size'           # size of PD (in byte)
          
    )

    def __init__(self):
        pass
    

#
# TROY PilotStore
#
class PilotStore(object):
    """ TROY PilotStore.    
            """

    # Class members
    __slots__ = (
        'id',           # Reference to this PJ
        'description',  # Description of PilotStore
        'context',      # SAGA context
        'resource_url', # Resource  URL       
        'state',        # State of the PilotStore
        'state_detail', # Adaptor specific state of the PilotStore       
    )

    def cancel(self):        
        """ Cancel PilotStore 

            Keyword arguments:
            None
        """
        pass
    
    
    def get_state(self):
        pass
    
    def get_state_detail(self):
        pass

#
# TROY PilotStore Service
#
class PilotStoreService(object):
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
        pass

    def create_pilotstore(self, pilotstore_desc):
        """ Create a PilotStore 

            Keyword arguments:
            pilotstore_desc -- PilotStore Description            
            
            Return value:
            A PilotStore handle
        """
        pass

    def list_pilotstores(self):
        """ List all PSs of PSS """
        pass

    def cancel(self):
        """ Cancel the PilotStoreService.
            
            Keyword arguments:
            None

            Return value:
            Result of operation
        """
        pass





#
# TROY PilotDataService
# 
class PilotDataService(object):
    """ TROY PilotDataService (PDS).   
        
    """

    # Class members
    __slots__ = (
        'id',                # id to this PilotDataService
        'state',             # Status of the PilotDataService
        'pilot_stores',      # PilotStores connected to this PD
        'pilot_data'         # List of WUs that have been submitted to this WUS
    )

    def __init__(self, pds_id=None):
        """ Create a PilotDataService.

            Keyword arguments:
            wus_id -- Reconnect to an existing PilotDataService 
        """
        pass

    def add(self, pss):
        """ Add a PilotStoreService 

            Keyword arguments:
            pss -- The PilotStoreService to add.

            Return:
            Result
        """
        pass

    def remove(self, pss):
        """ Remove a PilotStoreService 

            
            Keyword arguments:
            pss -- The PilotStoreService to remove 
            Return:
            Result
        """
        pass
    
    def list_pilotdata(self):
        """ List all PDs of PDS """
        pass

    
    def cancel(self):
        """ Cancel the PDS.            
            
            Keyword arguments:
            None

            Return:
            Result
        """
        pass



#
# TROY PilotDataDescription
# 
class PilotDataDescription(object):
    """ TROY PilotDataDescription.
            
    """

    # Class members
    __slots__ = (
        'id',
        'url'
        'd'        
    )

    def __init__(self):
        pass




#
# TROY PilotData 
# 
class PilotData(object):
    """ TROY PilotData.
    
        
    """

    # Class members
    __slots__ = (
        'id',               # Reference 
        'description',      # Description
        'state',            # State
        'state_detail',     # Detailed (application specific) state         
    )

    def cancel(self):
        """ Cancel the WU. """
        pass
    
    def get_state(self):
        pass
    
    def get_state_detail(self):
        pass
    

class DataUnit(object):
    """ TROY DataUnit """

    __slots__ = (
        'id',        
        'url'
    )
    
    def __init__(self):
        pass