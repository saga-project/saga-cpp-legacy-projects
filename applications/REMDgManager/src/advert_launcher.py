#!/usr/bin/env python
import sys
import os
import saga
import subprocess
import socket
import threading
import time

APPLICATION_NAME="REMD"

"""NAMD-Launcher:
   - reads new job information from advert service
   - starts new jobs
   - monitors running jobs """
class advert_launcher:
    
    """Constructor"""
    def __init__(self, database_host):
        
        self.database_host = database_host
        # objects to store running jobs and processes
        self.jobs = []
        self.processes = {}
        
         # open advert service base url
        hostname = socket.gethostname()
        self.base_url = "advert://" +  self.database_host + "/"+APPLICATION_NAME + "/" + hostname
        self.base_dir = saga.advert.directory(saga.url(self.base_url), saga.advert.Create | saga.advert.ReadWrite)
        
        #start background thread for polling new jobs and monitoring current jobs
        self.launcher_thread=threading.Thread(target=self.start_background_thread())
        self.launcher_thread.start() 
        

    """ for debugging purposes 
        print attributes of advert directory """
    def print_attributes(self, advert_directory):
        attributes = advert_directory.list_attributes()                
        for i in attributes:
            if (advert_directory.attribute_is_vector(i)==False):
                print "attribute: " + str(i) +  " value: " + advert_directory.get_attribute(i)
            else:
                print "attribute: " + str(i)
                vector = advert_directory.get_vector_attribute(i) 
                for j in vector:
                    print j
     
    def execute_replica_job(self, job_dir):
        
        self.print_attributes(job_dir)        
        if(job_dir.get_attribute("state")==str(saga.job.Unknown)):
            job_dir.set_attribute("state", str(saga.job.New))
            numberofprocesses = "1"
            if (job_dir.attribute_exists("NumberOfProcesses") == True):
                numberofprocesses = job_dir.get_attribute("NumberOfProcesses")
            
            spmdvariation="single"
            if (job_dir.attribute_exists("SPMDVariation") == True):
                spmdvariation = job_dir.get_attribute("SPMDVariation")
            
            arguments = ""
            if (job_dir.attribute_exists("Arguments") == True):
                for i in job_dir.get_vector_attribute("Arguments"):
                    arguments = arguments + " " + i
             
            executable = job_dir.get_attribute("Executable")
            
            workingdirectory = os.getcwd() 
            if (job_dir.attribute_exists("WorkingDirectory") == True):
                    workingdirectory =  job_dir.get_attribute("WorkingDirectory")
            
            output="stdout"
            if (job_dir.attribute_exists("Output") == True):
                    output = job_dir.get_attribute("Output")
                    
            error="stderr"
            if (job_dir.attribute_exists("Error") == True):
                   error = job_dir.get_attribute("Error")
           
            # append job to job list
            self.jobs.append(job_dir)
            
            # create stdout/stderr file descriptors
            stdout = open(output, "w")
            stderr = open(error, "w")
            command = executable + " " + arguments
            # special setup for MPI NAMD jobs
            if (spmdvariation.lower( )=="mpi"):
                command = "mpirun -np " + numberofprocesses + " -machinefile $PBS_NODEFILE " + command
                
            print "execute: " + command 
            p = subprocess.Popen(args=command, executable="/bin/bash",stdout=stdout,cwd=workingdirectory,shell=True)
            print "started " + command
            self.processes[job_dir] = p
            job_dir.set_attribute("state", str(saga.job.Running))
        
    """Poll jobs from advert service. """
    def poll_jobs(self):
        jobs = self.base_dir.list()
        print "Found " + "%d"%len(jobs) + " jobs"
        for i in jobs:  
            print i.get_string()
            job_dir = self.base_dir.open_dir(i.get_string(), saga.advert.Create | saga.advert.ReadWrite)
            self.execute_replica_job(job_dir)
                
    """Monitor running processes. """   
    def monitor_jobs(self):
        #while True:
            for i in self.jobs:
                p = self.processes[i]
                p_state = p.poll()
                if p_state != None and p_state==0:
                    print i.get_attribute("Executable") + " finished. "
                    i.set_attribute("state", str(saga.job.Done))
                    
    def start_background_thread(self):
        self.stop=False
        while True and self.stop==False:
            self.poll_jobs()
            self.monitor_jobs()
            time.sleep(30)
            
    def stop_background_thread(self):        
        self.stop=True

#########################################################
#  main                                                 #
#########################################################
if __name__ == "__main__" :
    args = sys.argv
    
    num_args = len(args)
    if (num_args<2):
        print "Usage: \n " + args[0] + " <advert-host>"
        sys.exit(1)
    
    for arg in args:
        print arg
    advert_launcher = advert_launcher(args[1])    
    time.sleep(80)
    advert_launcher.stop_background_thread()
        