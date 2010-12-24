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
      
    def __init__(self, name="", base_dir="", weight=0):
        """ name: string, base_dir: saga.url, weight: int"""
        #self.pilot_store=pilot_store
        self.name=name
        self.base_dir=saga.url(str(base_dir)) # make sure base_dir is a SAGA URL!!
        self.weight=weight
        self.file_registry=[]
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

    def deregister_file(self, file_url):
        """ removes file_url reference to pilot_data container """
        self.file_registry.remove(file_url)
    

    ##############################################################################
    def add_file(self, file_url):
        """ adds file_url to pilot_data container 
            TODO: file will be copied to base dir
        """
        self.register_file(file_url);

    def remove_file(self, file_url):
        """ removes file_url from pilot_data container 
            TODO: file will be deleted from disk
        """
        self.file_registry.remove(file_url)

    
    ##############################################################################
    def list_files(self):
        return self.file_registry 

    def join(self, pilot_data):
        """ joins pilot_data object with self
            i.e. self will be expanded with the files 
            in pilot_data
            TODO: specify whether files should be 
            relocated to self base dir
        """
        for i in pilot_data.list_files():
            self.add_file(i)
    
    def move(self, new_pilot_data_url):
        """ move files in pilot data container to new location
            old files will be deleted
        """
        self.base_dir=saga.url(new_pilot_data_url)
        new_file_locations = self.copy_files_to_location(new_pilot_data_url)
        self.delete_files_at_location(self.file_registry)
        self.file_registry=new_file_locations
    
    def copy(self, new_pilot_data_name, new_pilot_data_url):
        """ creates a new pilot_data object at the passed URL
            copy files to new base dir
        """
        new_pilot_store = pilot_store(new_pilot_data_name, new_pilot_data_url)
        new_file_locations = self.copy_files_to_location(new_pilot_data_url)
        for i in new_file_locations:
            new_pilot_store.add_file(i)
        
        
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
        ps_dir = saga.advert.directory(saga.url(pd_url.get_string()+"/"+str(ps.uuid)), saga.advert.Create | 
                                          saga.advert.CreateParents | 
                                          saga.advert.ReadWrite)
        ps_dir.set_attribute("name", ps.name)
        ps_dir.set_attribute("base_dir", ps.base_dir.get_string())
        ps_dir.set_attribute("weight", str(ps.weight))
        #pdb.set_trace()
        ps_dir.set_vector_attribute("file_registry", [x.get_string() for x in ps.file_registry])
            
    @staticmethod
    def from_advert(pd_dir):
        ps = pilot_store()
        ps.base_dir=pd_dir.get_attribute("base_dir")
        ps.name=pd_dir.get_attribute("name")
        ps.weight=pd_dir.get_attribute("weight")
        ps.file_registry=pd_dir.get_vector_attribute("file_registry")
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
    
    ##############################################################################
    def create_pilot_store(self, name, base_dir):
        new_pilot_store = pilot_store(name, base_dir)
        self.add_pilot_store(new_pilot_store)
        return new_pilot_store
    
    def add_pilot_store(self, pilot_store):
        self.pilot_store[pilot_store.name] = pilot_store
    
    def remove_pilot_store(self, pilot_data):
        self.pilot_store.remove(pilot_store)
    
    def list_pilot_store(self):
        return self.pilot_data.values()

    # helper methods for convinient access to pilot_data elements of the pilot store  
    def __getitem__(self, name):
        return pilot_store[name]
    
    def __iter__(self):
        return self.pilot_store.itervalues()

    @staticmethod
    def to_advert(pilot_data):
        pilot_data.app_url = saga.url("advert://" + DATABASE_HOST + "/"+APPLICATION_NAME + "-" + str(pilot_data.uuid) + "/")
        pilot_data.app_dir = saga.advert.directory(pilot_data.app_url, saga.advert.Create | 
                                                           saga.advert.CreateParents | 
                                                           saga.advert.ReadWrite)
        
        for i in pilot_data:
            pilot_store.to_advert(i, pilot_data.app_url)
            
    @staticmethod
    def from_advert():
        pd = pilot_data()
        return pd
    
    ##############################################################################
    def __del__(self):
        pass
    
    def __repr__(self): 
        pass
