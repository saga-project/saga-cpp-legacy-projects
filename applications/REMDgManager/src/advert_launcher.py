#!/home/luckow/sw/python-2.5.2/bin/python

import sys
import os
import saga
import subprocess
import socket
import threading
import time
import pdb

""" Config parameters (will move to config file in future) """
APPLICATION_NAME="REMD"

class advert_launcher:
    
    """NAMD-Launcher:
       - reads new job information from advert service
       - starts new jobs
       - monitors running jobs """
   
    """Constructor"""
    def __init__(self, database_host, advert_url):
        
        self.database_host = database_host
        # objects to store running jobs and processes
        self.jobs = []
        self.processes = {}
        self.freenodes = []
        self.busynodes = []
        
        self.init_pbs()
        
         # open advert service base url
        hostname = socket.gethostname()
        self.base_url = advert_url
        print "Open advert: " + self.base_url
        try:
            self.base_dir = saga.advert.directory(saga.url(self.base_url), saga.advert.Create | saga.advert.ReadWrite)
        except:
            print "No advert entry found at specified url: " + advert_url

        update_glidin_state()
        
        #start background thread for polling new jobs and monitoring current jobs
        self.launcher_thread=threading.Thread(target=self.start_background_thread())
        self.launcher_thread.start()
        
    def update_glidin_state(self):     
        return self.base_dir.set_attribute("state", str(saga.job.Running))
    
    def init_pbs(self):
        pbs_node_file = os.environ.get("PBS_NODEFILE")    
        if pbs_node_file != None:
            f = open(pbs_node_file)
            self.freenodes = f.readlines()
            f.close()
        
        
    def print_attributes(self, advert_directory):
        """ for debugging purposes 
        print attributes of advert directory """
        
        print "**** Attributes for: "+advert_directory.get_url().get_string()+ "**********"
        attributes = advert_directory.list_attributes()                
        for i in attributes:
            if (advert_directory.attribute_is_vector(i)==False):
                print "attribute: " + str(i) +  " value: " + advert_directory.get_attribute(i)
            else:
                print "attribute: " + str(i)
                vector = advert_directory.get_vector_attribute(i) 
                for j in vector:
                    print j
     
    def execute_job(self, job_dir):
        """ obtain job attributes from advert and execute process """
        
        self.print_attributes(job_dir)        
        if(job_dir.get_attribute("state")==str(saga.job.Unknown) or
           job_dir.get_attribute("state")==str(saga.job.New)):
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
            stdout = open(workingdirectory+"/"+output, "w")
            stderr = open(workingdirectory+"/"+error, "w")
            command = executable + " " + arguments
            
            # special setup for MPI NAMD jobs
            if (spmdvariation.lower( )=="mpi"):
		#pdb.set_trace()
                machinefile = self.allocate_nodes(job_dir)
                if(machinefile==None):
                    print "Not enough resources to run: " + job_dir.get_url().get_string() 
                    return # job cannot be run at the moment
                command = "mpirun -np " + numberofprocesses + " -machinefile " + machinefile + " " + command
                
            print "execute: " + command + " in " + workingdirectory
            p = subprocess.Popen(args=command, executable="/bin/bash",stderr=stderr,stdout=stdout,cwd=workingdirectory,shell=True)
            print "started " + command
            self.processes[job_dir] = p
            job_dir.set_attribute("state", str(saga.job.Running))
            
    def allocate_nodes(self, job_dir):
        """ allocate nodes
            allocated nodes will be written to machinefile advert-launcher-machines-<jobid>
            this method is only call by background thread and thus not threadsafe"""
        number_nodes = int(job_dir.get_attribute("NumberOfProcesses"))
        if (len(self.freenodes)>=number_nodes):
            machine_file_name = self.get_machine_file_name(job_dir)
            machine_file = open(machine_file_name, "w")
            machine_file.writelines(self.freenodes[:number_nodes])
            machine_file.close() 
            
            # update node structures
            self.busynodes.extend(self.freenodes[:number_nodes])
            del(self.freenodes[:number_nodes])            
            return machine_file_name
        return None
    
    def free_nodes(self, job_dir):
        print "Free nodes ..."
        number_nodes = int(job_dir.get_attribute("NumberOfProcesses"))
        machine_file_name = self.get_machine_file_name(job_dir)
        print "Machine file: " + machine_file_name
        machine_file = open(machine_file_name, "r")
        allocated_nodes = machine_file.readlines()
        machine_file.close()
        for i in allocated_nodes:
            self.busynodes.remove(i)
            self.freenodes.append(i)
	print "Delete " + machine_file_name
        os.remove(machine_file_name)
               
            
    def get_machine_file_name(self, job_dir):
        """create machinefile based on jobid"""
        job_dir_url =job_dir.get_url().get_string()        
        job_dir_url = job_dir_url[(job_dir_url.rindex("/", 0, len(job_dir_url)-1)+1)
                                  :(len(job_dir_url)-1)]        
	homedir = os.path.expanduser('~')
        return homedir  + "/advert-launcher-machines-"+ job_dir_url
        
    def poll_jobs(self):
        """Poll jobs from advert service. """
        jobs = self.base_dir.list()
        print "Found " + "%d"%len(jobs) + " jobs"
        for i in jobs:  
            #print i.get_string()
            job_dir = self.base_dir.open_dir(i.get_string(), saga.advert.Create | saga.advert.ReadWrite)
            self.execute_job(job_dir)
                
    
    def monitor_jobs(self):
        """Monitor running processes. """   
        for i in self.jobs:
            if self.processes.has_key(i): # only if job has already been starteds
                p = self.processes[i]
                p_state = p.poll()
                if p_state != None and p_state==0:
                    print i.get_attribute("Executable") + " finished. "
                    i.set_attribute("state", str(saga.job.Done))
                    self.free_nodes(i)
                    del self.processes[i]
                elif p_state!=0 and p_state != None:
                    i.set_attribute("state", str(saga.job.Failed))
                    self.free_nodes(i)
                    del self.processes[i]
		else:
		    print str(i) + "still running."
                                
    def monitor_checkpoints(self):
        """ parses all job working directories and registers files with Migol via SAGA/CPR """
        #get current files from AIS
        url = saga.url("advert_launcher_checkpoint");
        checkpoint = saga.cpr.checkpoint(url);
        files = checkpoint.list_files()
        for i in files:
            print i      
        dir_listing = os.listdir(os.getcwd())
        for i in dir_listing:
            filename = dir+"/"+i
            if (os.path.isfile(filename)):
                if(check_file(files, filename==False)):
                      url = self.build_url(filename)
                      print str(self.build_url(filename))
                        
    def build_url(self, filename):
        """ build gsiftp url from file path """
        hostname = socket.gethostname()
        file_url = saga.url("gsiftp://"+hostname+"/"+filename)
        return file_url
                
    def check_file(self, files, filename):
        """ check whether file has already been registered with CPR """
        for i in files:
            file_path = i.get_path()
            if (filename == filepath):
                return true
        return false
                        
    def start_background_thread(self):
        self.stop=False
        while True and self.stop==False:
            self.poll_jobs()
	    #pdb.set_trace()
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
    if (num_args!=3):
        print "Usage: \n " + args[0] + " <advert-host> <advert-director>"
        sys.exit(1)
    
    # init cpr
    jd=None
    try:
        js = saga.cpr.service()
    except:
        sys.exc_traceback
    
    advert_launcher = advert_launcher(args[1], args[2])    

