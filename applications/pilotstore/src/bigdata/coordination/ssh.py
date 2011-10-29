'''
SSH-based coordination scheme between manager and agent
'''
import paramiko
import urlparse
import pdb
import sys
import os
import logging

sys.path.append(os.path.join(os.path.dirname(__file__), "../.."))
from bigdata.troy.data.api import State

class BigDataCoordination(object):
    
    def __init__(self, service_url):        
        self.service_url = service_url
        result = urlparse.urlparse(service_url)
        self.host = result.netloc
        self.path = result.path        
        
        # initialize ssh client
        self.__client = paramiko.SSHClient()
        self.__client.load_system_host_keys()
        self.__client.connect(self.host)
        self.__sftp = self.__client.open_sftp()
        self.__state=State.New
                        
        
    def initialize_pilotstore(self):
        # check whether directory exists
        try:
            self.__sftp.chdir(self.path)            
        except IOError:
            # directory does not exist
            self.__sftp.mkdir(self.path)        
        self.__state=State.Running
        
        
    def get_pilotstore_size(self):
        # check size
        size = self.__sftp.stat(self.path).st_size
        return size
    
    
    def delete_pilotstore(self):
        self.__sftp.rmdir(self.path)
        self.__state=State.Done
        
        
    def get_state(self):
        if self.__client.get_transport().is_active()==True:
            return self.__state
        else:
            self.__state=State.Failed
            return self.__state            
            
    def create_pd(self, pd_id):
        self.__sftp.mkdir(os.path.join(self.path, str(pd_id)))
        
        
    def put_pd(self, pd):
        for i in pd.list_data_units():      
            remote_path = os.path.join(self.path, str(pd.id), os.path.basename(i.url))
            logging.debug("Put file: %s to %s"%(i.url, remote_path))
            self.__sftp.put(i.url, remote_path)
        
    
    
    