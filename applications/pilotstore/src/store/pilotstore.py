""" Pilot store / data
    
"""

import saga
import traceback
import sys
import os
import uuid
import pdb

# for logging
import logging
logging.basicConfig(level=logging.DEBUG)

APPLICATION_NAME="pilot-data"
DATABASE_HOST="localhost"

class pilot_store:  
    """ abstractions for managing a group of files 
        files within a pilot_data object have the affinity 'weight' to each other
        pilot_data object owns physical file, i.e. location of file 
        is changed or file is deleted on request  
    """
      
    def __init__(self, name="", base_dir="", pd=None, weight=0):
        """ name: string, base_dir: saga.url, weight: int"""
        #self.pilot_store=pilot_store
        self.name=name
        self.base_dir=saga.url(str(base_dir)) # make sure base_dir is a SAGA URL!!
        self.weight=weight
        self.file_registry=[]
        self.number_of_chunks=1
        if pd!=None: 
            self.pd_url=pd.app_url
        self.uuid = uuid.uuid1()
            
    ##############################################################################
    def register_file(self, file_url):
        """ adds file_url reference to pilot_data container """
        #try to parse URL
        logging.debug("URL: " + str(file_url) + " Host: " + file_url.host)
            
        # not an URL tree as local path relative to basedir
        if file_url.host == "":
            if os.path.exists(self.base_dir.path + "/" + str(file_url)):
                self.file_registry.append(saga.url(str(self.base_dir) + "/" + str(file_url)))
            else:
                raise IOError("File not found")
        elif file_url.host=="localhost":
            if os.path.exists(file_url.path):
                self.file_registry.append(file_url)
            else:
                raise IOError("File not found")
        else:
            self.file_registry.append(file_url)
        pilot_store.to_advert(self, self.pd_url)


    def register_files(self, file_urls):
        """ adds file_url references to pilot_data container """
        #pdb.set_trace()
        for i in file_urls:
            #try to parse URL
            file_url = saga.url(i)
            logging.debug("URL: " + file_url.get_string() + " Host: " + file_url.host)
            
            # not an URL tree as local path relative to basedir
            if file_url.host == "":
                if os.path.exists(self.base_dir.path + "/" + str(file_url)):
                    self.file_registry.append(saga.url(str(self.base_dir) + "/" + str(file_url)))
                else:
                    raise IOError("File not found")
            elif file_url.host=="localhost":
                if os.path.exists(file_url.path):
                    self.file_registry.append(saga.url(file_url))
                else:
                    raise IOError("File not found")
            else:
                self.file_registry.append(saga.url(file_url))
        pilot_store.to_advert(self, self.pd_url)

    def deregister_file(self, file_url):
        """ removes file_url reference to pilot_data container """
        self.file_registry.remove(file_url)
        pilot_store.to_advert(self, self.pd_url)

    ##############################################################################
    def add_file(self, file_url):
        """ adds file_url to pilot_data container 
            TODO: file will be copied to base dir
        """
        self.register_file(file_url);
        pilot_store.to_advert(self, self.pd_url)

    def remove_file(self, file_url):
        """ removes file_url from pilot_data container 
            TODO: file will be deleted from disk
        """
        self.file_registry.remove(file_url)
        pilot_store.to_advert(self, self.pd_url)

    
    ##############################################################################
    def list_files(self):
        return self.file_registry 

    def list_files_for_chunk(self, chunk_id):
        """ returns files for a certain chunk """        
        c = self.chunks(self.file_registry, self.number_of_chunks)
        logging.debug(str(c))
        return c[chunk_id] 
    
    def chunks(self, l, n):
        return [ l[i::n] for i in xrange(n) ]
        
    def join(self, ps):
        """ joins pilot_data object with self
            i.e. self will be expanded with the files 
            in pilot_data
            TODO: specify whether files should be 
            relocated to self base dir
        """
        for i in ps.list_files():
            self.add_file(i)
        pilot_store.to_advert(self, self.pd_url)
    
    def move(self, new_pilot_data_url):
        """ move files in pilot data container to new location
            old files will be deleted
        """
        self.base_dir=saga.url(new_pilot_data_url)
        new_file_locations = self.copy_files_to_location(new_pilot_data_url)
        self.delete_files_at_location(self.file_registry)
        self.file_registry=new_file_locations
        pilot_store.to_advert(self, self.pd_url)
    
    def copy(self, new_pilot_data_name, new_pilot_data_url):
        """ creates a new pilot_data object at the passed URL
            copy files to new base dir
        """
        new_pilot_store = pilot_store(new_pilot_data_name, new_pilot_data_url)
        new_file_locations = self.copy_files_to_location(new_pilot_data_url)
        for i in new_file_locations:
            new_pilot_store.add_file(i)
        pilot_store.to_advert(self, self.pd_url)
        
    def delete_files_at_location(self, file_list):
        """ deletes files in file list """
        for i in file_list:
            file = saga.filesystem.file(i)    
            file.remove()
        
        
    def copy_files_to_location(self, new_pilot_data_url):    
        new_file_locations = []
        for i in self.file_registry:
            logging.debug(i)
            try:
                dir = saga.filesystem.directory(saga.url(new_pilot_data_url), 
                                                saga.filesystem.Create |  saga.filesystem.ReadWrite)            
            except:                
                traceback.print_exc(file=sys.stdout)
                print "Could not create: " + str(new_pilot_data_url)
            
            # copy files
            try:
                source_url = saga.url(str(i))
                filename = os.path.split(source_url.path)[1]
                
                dest_url = saga.url(os.path.join(new_pilot_data_url, filename))
                print "copy file: " + str(i) + " to " + str(dest_url)
                sagafile = saga.filesystem.file(source_url)
                sagafile.copy(dest_url, saga.filesystem.Overwrite)
                new_file_locations.append(dest_url)                                
            except saga.exception, e:
                traceback.print_exc(file=sys.stdout)
                error_msg = "file %s failed to be copied to"%(i)
                logging.error(error_msg)
        return new_file_locations
    
    def get_resource(self):
        if str(self.base_dir.host) != "":
            return self.base_dir.host
        return "localhost"
    
    ##############################################################################
    # serialization to/from advert service
    @staticmethod
    def to_advert(ps, pd_url):
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
    def from_advert(ps_url):
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
        
    ##############################################################################
    def __del__(self): #deletes all files
        pass
    
    def __repr__(self): 
        return self.name
    
class pilot_data:
    """ 
        A pilot store consists of multiple 
        file groups (pilot data).
    """
    def __init__(self):
        self.pilot_store={}
        self.uuid = uuid.uuid1()
        self.app_url = saga.url("advert://" + DATABASE_HOST + "/"+APPLICATION_NAME + "-" + str(self.uuid) + "/")
        pilot_data.to_advert(self)
    
    ##############################################################################
    def create_pilot_store(self, name, base_dir):
        new_pilot_store = pilot_store(name, base_dir, self)
        self.add_pilot_store(new_pilot_store)
        pilot_data.to_advert(self)
        return new_pilot_store
    
    def add_pilot_store(self, pilot_store):
        self.pilot_store[pilot_store.name] = pilot_store
        pilot_data.to_advert(self)
    
    def remove_pilot_store(self, pilot_data):
        self.pilot_store.remove(pilot_store)
        pilot_data.to_advert(self)
    
    def list_pilot_store(self):
        self=pilot_data.from_advert(self.app_url)
        return self.pilot_store.values()

    # helper methods for convinient access to pilot_data elements of the pilot store  
    def __getitem__(self, name):
        return self.pilot_store[name]
    
    def __iter__(self):
        return self.pilot_store.itervalues()
    
    def refresh(self):
        """udpate pilot data state from advert service"""
        new_pd = pilot_data.from_advert(self.app_url)
        self.pilot_store=new_pd.pilot_store

    @staticmethod
    def to_advert(pilot_data):
        pilot_data.app_dir = saga.advert.directory(pilot_data.app_url, saga.advert.Create | 
                                                           saga.advert.CreateParents | 
                                                           saga.advert.ReadWrite)
        
        pilot_data.app_dir.set_attribute("uuid", str(pilot_data.uuid))
        
        for i in pilot_data.pilot_store.values():
            pilot_store.to_advert(i, pilot_data.app_url)
            
    @staticmethod
    def from_advert(pd_url):
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
    
    ##############################################################################
    def __del__(self):
        pass
    
    def __repr__(self): 
        return self.app_url
