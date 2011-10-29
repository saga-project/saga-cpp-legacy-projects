'''
SSH-based coordination scheme between manager and agent
'''
import paramiko
import urlparse
import pdb
import state

class BigDataCoordination(object):
    
    def __init__(self, service_url):        
        result = urlparse.urlparse(service_url)
        #pdb.set_trace()
        self.host = result.netloc
        self.path = result.path        
        
        # initialize ssh client
        self.__client = paramiko.SSHClient()
        self.__client.load_system_host_keys()
        self.__client.connect(self.host)
        self.__sftp = self.__client.open_sftp()
        self.__state=state.New
                        
        
    def initialize_pilotstore(self):
        # check whether directory exists
        try:
            self.__sftp.chdir(self.path)            
        except IOError:
            # directory does not exist
            self.__sftp.mkdir(self.path)        
        self.__state=state.Running
        
        
    def get_pilotstore_size(self):
        # check size
        size = self.__sftp.stat(self.path).st_size
        return size
    
    
    def delete_pilotstore(self):
        self.__sftp.rmdir(self.path)
        self.__state=state.Done
        
        
    def get_state(self):
        if self.__client.get_transport().is_active()==True:
            return self.__state
        else:
            self.__state=state.Failed
            return self.__state
            
            
        
    
    
    