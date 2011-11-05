'''
SSH-based coordination scheme between manager and agent
'''
import paramiko
import urlparse
import pdb
import errno
import sys
import os
import stat
import logging

sys.path.append(os.path.join(os.path.dirname(__file__), "../.."))
from bigdata.troy.compute.api import State

class BigDataCoordination(object):
    """ BigData Coordination File Management for Pilot Store """
    
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
        self.__remove_directory(self.path)
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
            remote_path = os.path.join(self.path, str(pd.id), os.path.basename(i.local_url))
            logging.debug("Put file: %s to %s"%(i.local_url, remote_path))
            if not stat.S_ISDIR(os.stat(i.local_url).st_mode):
                self.__sftp.put(i.local_url, remote_path, self.put_progress, True)
            else:
                logging.warning("Path %s is a directory. Ignored."%i.local_url)
    
    
    def get_pd(self, pd, target_directory):
        base_dir = self.__get_path_for_pd(pd)
        logging.debug("Copy PD from %s to %s"%(base_dir, target_directory))        
        if not os.path.exists(target_directory):
            os.makedirs(target_directory)
        for i in os.listdir(base_dir):
            self.__sftp.get(os.path.join(base_dir, i), os.path.join(target_directory, i))
    
        
    def remove_pd(self, pd):
        self.__remove_directory(os.path.join(self.path, pd.id))
    
        
    def put_progress(self, transfered_bytes, total_bytes):
        logging.debug("Bytes transfered %d/%d"%(transfered_bytes, total_bytes))
    
    
    
    
    ###########################################################################
    # Private support methods
    def __get_path_for_pd(self, pd):
        return os.path.join(self.path, str(pd.id))
    
    def __remove_directory(self, path):
        """Remove remote directory that may contain files.        
        """
        if self.__exists(path):
            for filename in self.__sftp.listdir(path):
                filepath = os.path.join(path, filename)
                logging.debug("Delete %s"%filepath)
                if stat.S_ISDIR(self.__sftp.stat(filepath).st_mode):
                    [self.__remove_directory(filepath)]
                else:
                    self.__sftp.remove(filepath)
            self.__sftp.rmdir(path)
    
    def __exists(self, path):
        """Return True if the remote path exists
        """
        try:
            self.__sftp.stat(path)
        except IOError, e:
            if e.errno == errno.ENOENT:
                return False
            raise
        else:
            return True
   
    