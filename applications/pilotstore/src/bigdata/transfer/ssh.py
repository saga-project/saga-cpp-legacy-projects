"""
Implement SSH/SCP/SFTP based transfer

"""
import paramiko
import urlparse
import pdb

class BigDataTransfer(object):

    def __init__(self, service_url):
        # initialize ssh client
        self.__client = paramiko.SSHClient()
        self.__client.load_system_host_keys()
        self.__client.connect(self.host)
        self.__sftp = self.__client.open_sftp()
        
    
    def put_pd(self, pilot_data):
        pass
    
    
    def get_pd(self, pilot_data):
        pass
        