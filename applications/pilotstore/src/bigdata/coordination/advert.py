import logging
import saga

class BigDataCoordination:
    
    @staticmethod
    def put_pilot_data(pilot_data):
        pilot_data.app_dir = saga.advert.directory(pilot_data.app_url, saga.advert.Create | 
                                                           saga.advert.CreateParents | 
                                                           saga.advert.ReadWrite)
        
        pilot_data.app_dir.set_attribute("uuid", str(pilot_data.uuid))
        
        for i in pilot_data.pilot_store.values():
            pilot_store.to_advert(i, pilot_data.app_url)
            
    @staticmethod
    def get_pilot_data(pd_url):
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
    def put_pilot_store(ps, pd_url):
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
            
    @staticmethod
    def get_pilot_store(ps_url):
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