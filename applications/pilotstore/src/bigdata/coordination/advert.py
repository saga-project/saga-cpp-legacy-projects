import logging
import saga
import json

class AdvertCoordinationAdaptor:
    """
        BigData persists its data in a central data space, e.g. the Advert service
        to facilitate distributed coordination:
        
        advert://advert.cct.lsu.edu/pilot/3d0d5960-296d-11e1-8896-00264a13ca4c/data/ => namespace for pilot data
        
        advert://advert.cct.lsu.edu/pilot/3d0d5960-296d-11e1-8896-00264a13ca4c/data/pds => pilot data service
        advert://advert.cct.lsu.edu/pilot/3d0d5960-296d-11e1-8896-00264a13ca4c/data/pds/pilot-data-description  => pilot data description
        ...
        
        
        advert://advert.cct.lsu.edu/pilot/3d0d5960-296d-11e1-8896-00264a13ca4c/data/pss/ => pilot store service
        advert://advert.cct.lsu.edu/pilot/3d0d5960-296d-11e1-8896-00264a13ca4c/data/pss/pilot-store-description => pilot store description
    
        This class is stateless - the application's base_url needs to be passed into every method.    
    """
    BASE_URL="advert://localhost/"
    BASE_URL_QUERY_STRING="?dbtype=sqlite3"
    
    PILOT_PATH="pilot"
    PILOT_DATA_PATH=PILOT_PATH+"/data"
    PILOT_STORE_SERVICE_PATH=PILOT_DATA_PATH+"/pss"
    PILOT_DATA_SERVICE_PATH=PILOT_DATA_PATH+"/pds"
    
    
    ###########################################################################
    # Construct a base url for an application
    
    @classmethod
    def get_base_url(cls, application_id):
        surl = saga.url(cls.BASE_URL)
        base_url = surl.scheme + "://" + surl.host + "/" + application_id + "/"
        logging.debug(base_url)
        return base_url
    
    ###########################################################################
    # Pilot Store Service related methods
    
    
    @classmethod  
    def add_pss(cls, application_url, pss):
        pss_url = cls.get_pss_url(application_url, pss.id)
        pss_description_url = cls.__get_url(pss_url+"/description")
        logging.debug("PSS URL: %s, PSS Description URL: %s"%(pss_url, pss_description_url))
        # directory is recursively created
        pss_desc_entry = saga.advert.entry(saga.url(pss_description_url),
                                           saga.advert.Create | saga.advert.CreateParents | saga.advert.ReadWrite)
        logging.debug("initialized advert entry for pss: " + pss_description_url)
        pss_desc_entry.store_string(json.dumps(pss.pilot_store_description))
        return pss_url
    
    @classmethod
    def get_pss(cls, pss_url):
        pass
        
    
    @classmethod
    def list_pss(cls):
        pass
    
    @classmethod
    def delete_pss(cls):
        pass
    
    ###########################################################################
    # Pilot Store related methods
    
    @classmethod
    def add_ps(cls, application_url, ps):
        pass
    
    @classmethod
    def get_ps(cls, ps_url):
        logging.debug("Open pilot store at: " + ps_url.get_string())     
        ps_dir = saga.advert.directory(ps_url, saga.advert.Create | 
                                               saga.advert.CreateParents | 
                                               saga.advert.ReadWrite)
        ps = pilot_store()        
        ps.base_dir=saga.url(ps_dir.get_attribute("base_dir"))
        ps.name=ps_dir.get_attribute("name")
        ps.uuid=ps_dir.get_attribute("uuid")
        ps.weight=ps_dir.get_attribute("weight")
        ps.pd_url=saga.url(ps_dir.get_attribute("pd_url"))
        ps.number_of_chunks=int(ps_dir.get_attribute("number_of_chunks"))
        if (ps_dir.attribute_exists("file_registry") == True):
            ps.file_registry = [saga.url(x) for x in ps_dir.get_vector_attribute("file_registry")]
        else:
            ps.file_registry=[]
        return ps
        
    
    @classmethod
    def list_ps(cls):
        pass
    
    @classmethod
    def delete_ps(cls):
        pass
    
    
    ###########################################################################
    # Pilot Data Service related methods
    
    
    ###########################################################################
    # Pilot Data related methods
    
    
    
    
    @classmethod
    def get_pss_url(cls, application_url, pss_id):
        pss_url = application_url+AdvertCoordinationAdaptor.PILOT_STORE_SERVICE_PATH+"/"+pss_id        
        logging.debug("PSS URL: %s"%(pss_url))
        return pss_url
    
    
    
    ###########################################################################
    # internal methods
    
    @classmethod
    def __get_url(cls, url):
        """ appends advert querystring for dbtype to url """
        url = url + "/"+AdvertCoordinationAdaptor.BASE_URL_QUERY_STRING
        return url
    
    
    
    ###########################################################################
    # old - to be removed
    
    @staticmethod
    def put_pd(pilot_data):
        pilot_data.app_dir = saga.advert.directory(pilot_data.app_url, saga.advert.Create | 
                                                           saga.advert.CreateParents | 
                                                           saga.advert.ReadWrite)
        
        pilot_data.app_dir.set_attribute("uuid", str(pilot_data.uuid))
        
        for i in pilot_data.pilot_store.values():
            pilot_store.to_advert(i, pilot_data.app_url)
            
    @staticmethod
    def get_pd(pd_url):
        logging.debug("Open pilot data at: " + str(pd_url.get_string()))     
        pd_dir = saga.advert.directory(pd_url, saga.advert.Create | 
                                       saga.advert.CreateParents | 
                                       saga.advert.ReadWrite)
        
        pd = pilot_data()        
        pd.uuid=pd_dir.get_attribute("uuid")
        pd.app_url = saga.url("advert://" + DATABASE_HOST + "/"+APPLICATION_NAME + "-" + str(pd.uuid) + "/")
        #logging.debug("Open directory")
        pilot_stores = pd_dir.list()
        for i in pilot_stores:
            ps_url = pd_url.get_string()+"/" + i.get_string()
            # logging.debug("Open Pilot Store: " + ps_url)
            # job_entry = self.new_job_dir.open_dir(i)
            # ps_dir = pd_dir.open_dir(i.get_string(), saga.advert.Create | saga.advert.ReadWrite)
            ps = pilot_store.from_advert(saga.url(ps_url))
            pd.add_pilot_store(ps)
        return pd
    
    @staticmethod
    def put_ps(ps, pd_url):
        ps.url=saga.url(pd_url.get_string()+"/pilot-store-"+str(ps.uuid))
        ps_dir = saga.advert.directory(ps.url, saga.advert.Create | 
                                          saga.advert.CreateParents | 
                                          saga.advert.ReadWrite)
        ps_dir.set_attribute("name", ps.name)
        ps_dir.set_attribute("uuid", str(ps.uuid))
        ps_dir.set_attribute("base_dir", ps.base_dir.get_string())
        ps_dir.set_attribute("weight", str(ps.weight))
        ps_dir.set_attribute("pd_url", str(ps.pd_url))
        ps_dir.set_attribute("number_of_chunks", str(ps.number_of_chunks))
        #pdb.set_trace()
        if (len(ps.file_registry)>0):
            ps_dir.set_vector_attribute("file_registry", [x.get_string() for x in ps.file_registry])
            
   
        