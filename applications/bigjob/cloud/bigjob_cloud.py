#!/usr/bin/env python

"""Module bigjob_cloud.

This Module is used to launch a set of jobs via a defined set of cloud virtual machines. 

"""

import sys
import getopt
import saga
import time
import uuid
import pdb
import socket
import os
import subprocess
import traceback
import logging
import re
import Queue
import threading
import copy


NIMBUS_CLIENT="/opt/nimbus/bin/cloud-client.sh"
NIMBUS_URL="nimbus://tp-vm1.ci.uchicago.edu"
IMAGE_NAME="gentoo_saga-1.3.3_namd-2.7b1.gz"
DEFAULT_WALLTIME=60   #in minutes

class bigjob_cloud():
    
    """ Initialize BigJob
        In contrast to advert-based BigJob no database host is required.
    """
    def __init__(self, database_host=None):        # no database host as advert based bigjob
       self.uuid = uuid.uuid1()
       self.queue=Queue.Queue()
        
    def start_pilot_job(self, 
                 lrms_url=None,                     # in future version one can specify a URL for a cloud (ec2:// vs. nimbus:// vs. eu://)
                 bigjob_agent_executable=None,      # n/a
                 number_nodes=1,                    # number of images requested    
                 queue=None,                        # n/a
                 project=None,                      # n/a
                 working_directory=None,            # working directory
                 userproxy=None,               # optional: path to user credential (X509 cert or proxy cert)
                 walltime=None):               # optional: walltime
        """ The start_pilot_job method will initialize the requested number of images """           

        if userproxy != None and userproxy != '':
            os.environ["X509_USER_PROXY"]=userproxy
            print "use proxy: " + userproxy
        else:
            print "use standard proxy"

        print "Working directory: " + working_directory
        if not os.path.isdir(working_directory):
            os.mkdir(working_directory)
        self.working_directory=working_directory    
        self.walltime=walltime       
        self.nodes = [] 
        self.free_nodes = []
        self.busynodes = []
        self.subjobs = {}
        self.pilot_url=NIMBUS_URL
        
        self.resource_lock = threading.RLock()       
        
        # spawn Nimbus images
        self.start_nimbus_images_as_thread(number_nodes)
        # for fast debugging     
        #self.nodes = [{"hostname":"tp-x001.ci.uchicago.edu", "vmid":"vm-049", "cpu_count":2},
        #              {"hostname":"tp-x002.ci.uchicago.edu", "vmid":"vm-050", "cpu_count":2}]      
        self.free_nodes=copy.copy(self.nodes)
                
        self.launcher_thread=threading.Thread(target=self.start_background_thread)
        self.launcher_thread.start()
        print "Finished launching of pilot jobs"
        
        
    def start_nimbus_images_as_thread(self, number_nodes):
        """ Launches specified number of nimbus images 
            For each image launch a separate thread is spawned """
        init_threads = []
        number_of_images_to_start = number_nodes
        retry_number = 0 
        # restart mechanism in case an allocation fails
        while number_of_images_to_start > 0 and retry_number < 3:
            print "Start " + str(number_of_images_to_start) + " images."
            for i in range(0, number_nodes):
                thread=threading.Thread(target=self.start_nimbus_image)
                thread.start()      
                init_threads.append(thread)
             
            # join threads
            for t in init_threads:
                t.join()    
            
            number_of_images_to_start = number_nodes - len(self.nodes)
            retry_number = retry_number + 1
                    
    
    def start_nimbus_image(self):
        """ Starts a single Nimbus image """
        #return {"hostname":"tp-x012.ci.uchicago.edu", "vmid":"vm-061", "cpu_count":1}
        nimbus_walltime=1;
        if self.walltime!=None:
            nimbus_walltime=self.walltime/60
        command = NIMBUS_CLIENT + " --run " + " --name " + IMAGE_NAME + " --hours " + str(nimbus_walltime)
        print "execute: " + command + " in " + self.working_directory              
        start=time.time()
        p = subprocess.Popen(args=command, 
                            stdin=None,
                            stderr=subprocess.STDOUT,
                            stdout=subprocess.PIPE,
                            cwd=self.working_directory, shell=True)
        print "started " + command 
        p.wait()
        end = time.time()     
        print "VM Start Time: " + str(end-start) + " s - get result now"
        stdout = p.communicate()[0]   # get stdout as string
        
        # grep for vm id (vm-xxx)
        vmid_regex = re.compile('vm-\\d\\d\\d')
        vmin_mo = vmid_regex.search(stdout)
        vmid=None
        if vmin_mo:
            vmid = vmin_mo.group()
            
        # grep for vm hostname (tgxxx.domain.com)
        # from output: Hostname: tp-x001.ci.uchicago.edu
        hostname_regex=re.compile("(Hostname:\s)([\w\-\.]*)")
        hostname_mo = hostname_regex.search(stdout)
        hostname=None
        if hostname_mo:
            hostname=hostname_mo.group(2)
        if (vmid!=None and hostname!=None):
            print "Sucessfully launched image. VMID: " + vmid + " Hostname: " + hostname
            # wait for images to properly come up
            time.sleep(60)
            self.setup_nimbus_image(hostname)
            self.resource_lock.acquire()
            self.nodes.append({"hostname":hostname, "vmid":vmid, "cpu_count":2})
            self.resource_lock.release()
        else:
            print "Failed to launch VM."
            raise NoResourceAvailable("No resource available.")
    
    
    
    def setup_nimbus_image(self, hostname):
        """ ensure ssh keys are properly setup """
        jd = saga.job.description()
        jd.executable = "/usr/bin/cat"
        jd.number_of_processes = "1"
        jd.spmd_variation = "single"
        # ssh root@tp-x001.ci.uchicago.edu "cat ~/.ssh/id_rsa.pub >> ~/.ssh/authorized_keys"
        jd.arguments = ["~/.ssh/id_rsa.pub", ">>", "~/.ssh/authorized_keys" ]
        jd.output = "stdout.txt"
        jd.error = "stderr.txt"
        
        job_service_url = saga.url("ssh://root@" + hostname)
        job_service = saga.job.service(job_service_url)
        job = job_service.create_job(jd)
        job.run()
        
        
    def get_state(self):        
        """ duck typing for get_state of saga.cpr.job and saga.job.job  """
        if (self.nodes.count>0):
            return "running";
        else:
            return "failed";
    
    def get_state_detail(self):
        """for compatibility reason with advert bigjob""" 
        return self.get_state()
    
    def cancel(self):        
        """ duck typing for cancel of saga.cpr.job and saga.job.job  """
        self.stop_background_thread()
        print "Cancel Cloud VMs"
        stop_threads = [] 
        for i in self.nodes:
             thread=threading.Thread(target=self.stop_nimbus_image,
                                     args=[i["vmid"]])
             thread.start()      
             stop_threads.append(thread)
             
        # join threads
        for t in stop_threads:
            t.join()    
            
        self.nodes=[]
        
            
    def stop_nimbus_image(self, vmid):
        """ stops Nimbus image with passed vmid """
        print "Terminate VM: " + vmid
        command = NIMBUS_CLIENT + " --terminate " + "--handle " + vmid
        print "Command: " + str(command)
        p = subprocess.Popen(args=command, 
                                stdin=None,
                                stderr=subprocess.STDOUT,
                                stdout=subprocess.PIPE,
                                cwd=self.working_directory, shell=True)
        p.wait()
        print "Successfully terminated VM " + vmid 
            
    def add_subjob(self, jd):
        print "add subjob to queue"
        job_id =  uuid.uuid1()
        self.queue.put({"job_id":job_id, "job_description":jd, "state":saga.job.New})
        return job_id
            
    def get_job_url(self, pilot_url):
            self.saga_pilot_url = saga.url(pilot_url)
            if(self.saga_pilot_url.scheme=="advert"): #
                pass

            else: # any other url, try to guess pilot job url
                host=""
                try:
                    host = self.saga_pilot_url.host
                except:
                    pass
                if host =="":
                    host=socket.gethostname()
                # create dir for destination url
                self.saga_pilot_url = saga.url("advert://" +  self.database_host + "/"+APPLICATION_NAME + "/" + host)

            # create dir for job
            self.job_url = self.saga_pilot_url.get_string() + "/" + str(self.uuid)
            return self.job_url

    def execute_job(self, job_dict):
        """ execute job on free node"""
        """ obtain job attributes from advert and execute process """
        jd = job_dict["job_description"]
        jobid = job_dict["job_id"]
        state = job_dict["state"]
        print "Execute job: " + str(jobid)+ " state: " + str(state)
        if(str(state)==str(saga.job.Unknown) or
           str(state)==str(saga.job.New)):
            try: 
                job_service_url = self.allocate_nodes(int(jd.number_of_processes))
                if str(job_service_url) != "":
                    print "Execute subjob: " + str(jobid) + " at: " + str(job_service_url)
                    job_service = saga.job.service(job_service_url)
                    job = job_service.create_job(jd)
                    job.run()
                    
                    # store objects for later use
                    job_dict["job_service_url"]=job_service_url
                    job_dict["job_service"]=job_service
                    job_dict["job"]=job
                    self.subjobs[str(jobid)]=job_dict
                else:
                    raise NoResourceAvailable("No resource available")
            except:
                traceback.print_exc(file=sys.stdout)
                raise
        
    def allocate_nodes(self, number_of_nodes):
        """ allocate nodes - remove nodes from free nodes list
            return SAGA-URL to resource ssh://tx.domain.org
        """
        allocated_nodes = []
        self.resource_lock.acquire()
        if (len(self.free_nodes)>=number_of_nodes): 
            for i in self.free_nodes[:]:
                number = i["cpu_count"]
                print "allocate: " + i["hostname"] + " number cores: " + str(number)
                if(number_of_nodes > 0):
                        allocated_nodes.append(i)
                        self.free_nodes.remove(i)                
                        self.busynodes.append(i)
                        number_of_nodes = number_of_nodes - 1
                else:
                        break
        
        self.resource_lock.release()
        self.setup_charmpp_nodefile(allocated_nodes)
        return saga.url("ssh://root@" + allocated_nodes[0]["hostname"])
            
    
    def setup_charmpp_nodefile(self, allocated_nodes):
        """ Setup charm++ nodefile to use for executing NAMD  
            HACK!! Method violates layering principle
            File $HOME/machinefile in charm++ nodefileformat is written to first node in list
        """
        # Nodelist format:
        # 
        # host tp-x001 ++cpus 2 ++shell ssh 
        # host tp-x002 ++cpus 2 ++shell ssh
        nodefile_string=""
        for i in allocated_nodes:
            nodefile_string=nodefile_string + "host "+ i["hostname"] + " ++cpus " + str(i["cpu_count"]) + " ++shell ssh\n"
            
        # copy nodefile to rank 0 node
        jd = saga.job.description()
        jd.executable = "/usr/bin/echo"
        jd.number_of_processes = "1"
        jd.spmd_variation = "single"
        # ssh root@tp-x001.ci.uchicago.edu "cat ~/.ssh/id_rsa.pub >> ~/.ssh/authorized_keys"
        jd.arguments = ["\""+nodefile_string+"\"", ">", "machinefile" ]
        jd.output = "stdout.txt"
        jd.error = "stderr.txt"
        
        job_service_url = saga.url("ssh://root@"+allocated_nodes[0]["hostname"])
        job_service = saga.job.service(job_service_url)
        job = job_service.create_job(jd)
        job.run()
             
        
    
    def deallocate_nodes(self, job_dict):
         """ add nodes back to free node list """
         number_nodes = int(job_dict["job_description"].number_of_processes)
         hostname = job_dict["job_service_url"].host
         self.resource_lock.acquire()
         for i in self.busynodes:
             if i["hostname"]==hostname:
                 self.busynodes.remove(i) 
                 self.free_nodes.append(i)
        
         self.resource_lock.release()
        
        
    
    def get_state_of_subjob(self, jobid):
        """returns state of specified subjob"""
        if self.subjobs.has_key(str(jobid)):
            job_dict = self.subjobs[str(jobid)]
            job = job_dict["job"]
            return job.get_state()
        return None
   
    def cancel_subjob(self, jobid):
        """returns state of specified subjob"""
        if self.subjobs.has_key(str(jobid)):
            job_dict = self.subjobs[str(jobid)]
            job = job_dict["job"]
            return job.cancel()
        return None     
    
    def monitor_jobs(self):
        """Monitor running processes. """   
        for i in self.subjobs.values():
            job = i["job"]
            state = job.get_state()
            if (i.has_key("freed")==False and (str(state)=="Failed" or str(state)=="Done")):
                    self.deallocate_nodes(i)   
                    i["freed"]=True
                                 
        
    def start_background_thread(self):
        self.stop=False
        print "\n"
        print "##########################    ########### New POLL/MONITOR cycle ##################################"
        print "Free nodes: " + str(len(self.free_nodes)) + " Busy Nodes: " + str(len(self.busynodes))
        while self.stop==False:
            try:
                print "Poll/Monitor job queue"
                job_dict = self.queue.get(True, 2)
                if job_dict != None:
                    try:
                        self.execute_job(job_dict)
                    except NoResourceAvailable:
                        print "No resources available - put job back into queue."
                        self.queue.put(job_dict)
                self.monitor_jobs()            
                if self.queue.empty():
                    time.sleep(10)
            except KeyboardInterrupt:
                print "Keyboard Interrupt"
                self.stop=True
                raise
            except Queue.Empty:
                pass
            except saga.exception:
                traceback.print_exc(file=sys.stdout)
                break
            
    
    def stop_background_thread(self):        
        self.stop=True
    
    def __repr__(self):
        return NIMBUS_URL

    def __del__(self):
        self.stop_background_thread()
        self.cancel()

                    
                    
class subjob():
    
    def __init__(self, 
                 database_host=None,
                 bigjob=None):
        """Constructor"""
        self.bigjob=bigjob
        self.job_url=None
        self.job_id = None

    def submit_job(self, jd):
        """ submit job via ssh adaptor to cloud"""
        print "submit job to: " + str(self.bigjob.pilot_url)
       
        #queue subjob add bigjob
        self.job_id=self.bigjob.add_subjob(jd)
        self.job_url=self.bigjob.pilot_url + "/compute/"+ str(self.job_id)

    def get_state(self):     
        return self.bigjob.get_state_of_subjob(self.job_id)
    
    def delete_job(self):
        print "delete subjob: " + self.job_url
        return self.bigjob.cancel_subjob(self.job_id)

    def __del__(self):
        self.delete_job()
    
    def __repr__(self):        
        if(self.job_url==None):
            return "None"
        else:
            return self.job_url
        
class NoResourceAvailable(Exception):
    def __init__(self, value):
        self.value = value

    def __str__(self):
        return repr(self.value)

