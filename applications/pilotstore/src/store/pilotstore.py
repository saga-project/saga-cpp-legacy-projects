""" Pilot store / data
    
"""

import saga
import traceback
import sys
import os

# for logging
import logging
logging.basicConfig(level=logging.DEBUG)

class pilot_data:  
    """ abstractions for managing a group of files 
        files within a pilot_data object have the affinity 'weight' to each other
        pilot_data object owns physical file, i.e. location of file 
        is changed or file is deleted on request  
    """
      
    def __init__(self, name="", base_dir="", weight=0):
        """ name: string, base_dir: saga.url, weight: int"""
        self.pilot_store=pilot_store
        self.name=name
        self.base_dir=saga.url(str(base_dir)) # make sure base_dir is a SAGA URL!!
        self.weight=weight
        self.file_registry=[]
            
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
        new_pilot_data = pilot_data(new_pilot_data_name, new_pilot_data_url)
        new_file_locations = self.copy_files_to_location(new_pilot_data_url)
        for i in new_file_locations:
            new_pilot_data.add_file(i)
        
        
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
    def __del__(self): #deletes all files
        pass
    
    def __repr__(self): 
        return self.name
    
class pilot_store:
    """ 
        A pilot store consists of multiple 
        file groups (pilot data).
    """
    def __init__(self):
        self.pilot_data={}
    
    ##############################################################################
    def create_pilot_data(self, name, base_dir):
        new_pilot_data = pilot_data(name, base_dir)
        self.add_pilot_data(new_pilot_data)
        return new_pilot_data
    
    def add_pilot_data(self, pilot_data):
        self.pilot_data[pilot_data.name] = pilot_data
    
    def remove_pilot_data(self, pilot_data):
        self.pilot_data.remove(pilot_data)
    
    def list_pilot_data(self):
        return self.pilot_data.values()

    # helper methods for convinient access to pilot_data elements of the pilot store  
    def __getitem__(self, name):
        return pilot_data[name]
    
    def __iter__(self):
        return self.pilot_data.itervalues()

    
    ##############################################################################
    def __del__(self):
        pass
    
    def __repr__(self): 
        pass