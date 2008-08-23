"""Module advert_job.

This Module is used to launch jobs via the advert service. 

It assumes that an advert-launcher.py agent is  running on the remote machine.
advert-launcher.py will poll the advert service for new jobs and run these jobs on the respective
machine  

Background: This approach avoids queueing delays since the advert-launcher.py must be just started via saga.job or saga.cpr
once. All shortrunning task will be started using the protocol implemented by advert_job() and advert_launcher.py


"""

import sys
import getopt
import saga
import time
import uuid
import socket

APPLICATION_NAME="REMD"
        
class advert_job():
    
    """Constructor"""
    def __init__(self, database_host):
        self.database_host = database_host
        print "init advert service sesssion for Application: " + database_host
        advert_dir = saga.url("advert://" + database_host + "/"+APPLICATION_NAME)
        print "create advert directory for job: " + advert_dir.get_string()                
        self.advert = saga.advert.directory(advert_dir, saga.advert.Create | saga.advert.ReadWrite)
        print "created advert directory for job: " + advert_dir.get_string()                
        
        
    """ submit job via advert service to NAMD-Launcher """
    def submit_job(self, dest_url, jd):
        
        if dest_url=="":
            dest_url = socket.gethostname()
        
        # create dir for destination url
        base_url = "advert://" +  self.database_host + "/"+APPLICATION_NAME + "/" + dest_url
        self.location_dir = saga.advert.directory(saga.url(base_url), saga.advert.Create | saga.advert.ReadWrite)

        # create dir for job
        self.uuid = uuid.uuid1()
        self.job_url = "advert://" +  self.database_host + "/"+APPLICATION_NAME + "/" + dest_url + "/" + str(self.uuid)
        self.job_dir = saga.advert.directory(saga.url(self.job_url), saga.advert.Create | saga.advert.ReadWrite)

        print "initialized advert directory for job: " + self.job_url
        
        # put job description attributes to advert
        attributes = jd.list_attributes()                
        for i in attributes:          
            if jd.attribute_is_vector(i):
                self.job_dir.set_vector_attribute(i, jd.get_vector_attribute(i))
            else:
                print "Add attribute: " + str(i) + " Value: " + jd.get_attribute(i)
                self.job_dir.set_attribute(i, jd.get_attribute(i))
       
        self.job_dir.set_attribute("state", str(saga.job.Unknown))
        # return self object for get_state() query    
        return "", self    
        
    def get_state(self):        
        return self.job_dir.get_attribute("state")


""" Test Job Submission via Advert """
if __name__ == "__main__":
    if (len(sys.argv)!=2):
        print "Usage: \n" + sys.argv[0] + " <database_host>"
        sys.exit(1)
        
    a = advert_job(sys.argv[1])
    jd = saga.cpr.description()
    jd.executable = "/bin/date"
    jd.numberofprocesses = "2"
    jd.spmdvariation = "single"
    jd.arguments = [""]  
    jd.workingdirectory = "/tmp/"
    jd.output = "output.txt"   
    jd.error = "error.txt"
    
    error, job = a.submit_job("", jd)    
    print "state: " + str(job.get_state())
    
    
