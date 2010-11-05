import saga
import traceback
import sys

# for logging
import logging
logging.basicConfig(level=logging.DEBUG)

class pilot_data:    
    def __init__(self, name="", weight=0):
        self.pilot_store=pilot_store
        self.name=name
        self.weight=weight
        self.file_registry=[]
            
    ##############################################################################
    def add_file(self, file_url):
        self.file_registry.append(file_url)

    def remove_file(self, file_url):
        self.file_registry.remove(file_url)
    
    def list_files(self):
        return self.file_registry 

    def join(self, pilot_data):
        pass
    
    def copy(self, new_pilot_data_url):
        for i in self.file_registry:
            logging.debug(i)
            try:
                saga.filesystem.directory(saga.url(new_pilot_data_url), saga.filesystem.Create |  saga.filesystem.ReadWrite)            
            except:                
                traceback.print_exc(file=sys.stdout)
                print "Could not create: " + str(new_pilot_data_url)
            
            # copy files
            try:
                source_url = saga.url(str(i))
                dest_url = saga.url(new_pilot_data_url)
                print "copy file: " + str(i) + " to " + str(new_pilot_data_url)
                sagafile = saga.filesystem.file(source_url)
                sagafile.copy(dest_url)                
            except saga.exception, e:
                traceback.print_exc(file=sys.stdout)
                error_msg = "file %s failed to be copied to"%(i)
                logging.error(error_msg)

    
    ##############################################################################
    def __del__(self): #deletes all files
        pass
    
    def __repr__(self): 
        return self.group_name
    
class pilot_store:
    """ 
        A pilot store consists of multiple 
        file groups (pilot data).
    """
    def __init__(self):
        self.pilot_data={}
    
    ##############################################################################
    def create_pilot_data(self, name):
        new_pilot_data = pilot_data(name)
        self.add_pilot_data(new_pilot_data)
        return new_pilot_data
    
    def add_pilot_data(self, pilot_data):
        self.pilot_data[pilot_data.name] = pilot_data
    
    def remove_pilot_data(self, pilot_data):
        self.pilot_data.remove(pilot_data)
    
    def get_pilot_data(self, name):
        return pilot_data[name]
    
    ##############################################################################
    def __del__(self):
        pass
    
    def __repr__(self): 
        pass