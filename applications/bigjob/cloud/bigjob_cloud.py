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

#Nimbus
NIMBUS_CLIENT="/opt/nimbus/bin/cloud-client.sh"
NIMBUS_URL="nimbus://tp-vm1.ci.uchicago.edu"
DEFAULT_WALLTIME=60   #in minutes

#EC2
EC2_ENV_FILE="/Users/luckow/.ec2/ec2rc"
EC2_KEYNAME="lsu-keypair"
EC2_SSH_PRIVATE_KEY_FILE="/Users/luckow/.ec2/id-lsu-keypair"
EC2_INSTANCE_TYPE="m1.large"

# EUCA
EUCA_ENV_FILE="/Users/luckow/.euca/eucarc"
EUCA_KEYNAME="euca-key"
EUCA_SSH_PRIVATE_KEY_FILE="/Users/luckow/.euca/euca-key.private"
EUCA_INSTANCE_TYPE="m1.small"

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
                 userproxy=None,                    # optional: path to user credential (X509 cert or proxy cert)
                 walltime=None,                     # optional: walltime
                 cloud_type=None,
                 image_name=None):                  # optional: EC2 or Nimbus
        """ The start_pilot_job method will initialize the requested number of images """           

        print "Working directory: " + working_directory
        if not os.path.isdir(working_directory):
            os.mkdir(working_directory)
        self.working_directory=working_directory    
        self.walltime=walltime       
        self.nodes = [] 
        self.free_nodes = []
        self.busynodes = []
        self.subjobs = {}
        
        self.job_service_cache={}
        #EC2 environment
        self.env_dict={}
        self.cloud_type = cloud_type
        self.image_name = image_name
         
        # for locking 
        self.resource_lock = threading.RLock()       
        
        # spawn Cloud images
        start=time.time()
        host=socket.gethostname()
        if cloud_type == "EC2":
            self.pilot_url="ec2://"+host
            
            # SSH Context
            self.ssh_context = saga.context("ssh")
            self.ssh_context.set_attribute("UserKey", EC2_SSH_PRIVATE_KEY_FILE)
            self.session = saga.session()
            self.session.add_context(self.ssh_context)  
            
            self.key_name = EC2_KEYNAME
            
            #setup environment
            self.env_dict=self.read_ec2_environments(EC2_ENV_FILE)   
            self.start_ec2_images(number_nodes)
                        
        elif cloud_type == "EUCA":
            self.pilot_url="euca://"+host
            
            self.ssh_context = saga.context("ssh")
            self.ssh_context.set_attribute("UserKey", EUCA_SSH_PRIVATE_KEY_FILE)
            self.session = saga.session()
            self.session.add_context(self.ssh_context)   
            
            self.key_name = EUCA_KEYNAME
            
             #setup environment
            self.env_dict=self.read_ec2_environments(EUCA_ENV_FILE)   
            #1
            self.start_ec2_images(number_nodes)
            
        elif cloud_type ==  "NIMBUS":
            self.pilot_url="nimbus://"+host
            self.ssh_context = saga.context("ssh")
            self.session = saga.session() # use default
            self.session.add_context(self.ssh_context)   
            
            self.start_nimbus_images_as_thread(number_nodes)
        else:
            raise UnsupportedCloudType("Cloud Type not supported")
        
        # for fast debugging     
        #self.nodes=[{"hostname":"149.165.228.103", "vmid":"i-48F80882", "private_hostname":"192.168.8.2", "cpu_count":1},
        #            {"hostname":"149.165.228.108", "vmid":"i-40820878", "private_hostname":"192.168.8.3", "cpu_count":1},
        #            ]
#       self.nodes = [{"hostname":"tp-x001.ci.uchicago.edu", "vmid":"vm-049", "cpu_count":2},
#                      {"hostname":"tp-x002.ci.uchicago.edu", "vmid":"vm-050", "cpu_count":2},
#                      {"hostname":"tp-x004.ci.uchicago.edu", "vmid":"vm-050", "cpu_count":2},      
#                      {"hostname":"tp-x005.ci.uchicago.edu", "vmid":"vm-050", "cpu_count":2}]
        
        print "Started " + str(len(self.nodes)) + " nodes in " + str(time.time()-start)      
        
        # check whether all requested nodes have been started
        if len(self.nodes) < number_nodes:
            raise NoResourceAvailable("Not sufficient resource available.")  
        
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
            for i in range(0, number_of_images_to_start):
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
        command = NIMBUS_CLIENT + " --run " + " --name " + self.image_name + " --hours " + str(nimbus_walltime)
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
            self.setup_image(hostname)
            self.resource_lock.acquire()
            self.nodes.append({"hostname":hostname, "vmid":vmid, "cpu_count":2})
            self.resource_lock.release()
        else:
            print "Failed to launch VM."
            raise NoResourceAvailable("No resource available.")
    
        
    def setup_image(self, hostname):
        """ ensure ssh keys are properly setup (works for Nimbus, Eucalyptus and EC2 """
        jd = saga.job.description()
        jd.executable = "/usr/bin/cat"
        jd.number_of_processes = "1"
        jd.spmd_variation = "single"
        # ssh root@tp-x001.ci.uchicago.edu "cat ~/.ssh/id_rsa.pub >> ~/.ssh/authorized_keys"
        jd.arguments = ["~/.ssh/id_rsa.pub", ">>", "~/.ssh/authorized_keys" ]
        jd.output = "stdout.txt"
        jd.error = "stderr.txt"
        
        job_service_url = saga.url("ssh://root@" + hostname)
        job_service = saga.job.service(self.session, job_service_url)
        job = job_service.create_job(jd)
        job.run()
        
        # Cache job service object for later usage
        self.job_service_cache[job_service_url] =job_service
        
        # wait for completion of job
        job.wait()
        
        
    def start_ec2_images(self, number_nodes):
        """Start EC2 image (either on Eucalyptus or Amazon EC2) """
 
        
        command = self.env_dict["EC2_HOME"] + "/bin/ec2-run-instances " +  self.image_name \
                + " -k " + self.key_name + " -n " + str(number_nodes) + " -t " + EC2_INSTANCE_TYPE
        print "execute: " + command + " in " + self.working_directory
        
        stdout = self.execute_command(command, self.working_directory, self.env_dict)
        vmid_regex = re.compile("i-\w*")
        for i in stdout.splitlines():
            try:
                m = vmid_regex.search(i)
                vmid = m.group()
                print vmid
                self.nodes.append({"hostname":None, "vmid":vmid, "cpu_count":1, "state":"pending"})
            except:
                pass
        print "Started instances: " + str(self.nodes)
        
        #wait for instances to startup
        self.wait_for_ec2_instance_startup()
        
        # make sure image is booted
        time.sleep(30)
        
        #setup images
        for i in self.nodes:
            self.setup_image(i["hostname"])
                  
    def wait_for_ec2_instance_startup(self):
        """ polls EC2 for updated instance states """
        command = self.env_dict["EC2_HOME"]  + "bin/ec2-describe-instances"
        while self.check_all_ec2_nodes()==False:
            stdout = self.execute_command(command, self.working_directory, self.env_dict)
            for i in stdout.splitlines():
                if i.startswith("INSTANCE"):
                    line_components = i.split()
                    instance_id = line_components[1]
                    public_ip = line_components[3]
                    internal_ip = line_components[4]
                    state = line_components[5]
                    if state == "running":
                        for node in self.nodes:
                            if node["vmid"]==instance_id:
                                print "New running instance: " + instance_id
                                node["hostname"]=public_ip
                                node["private_hostname"]=internal_ip
                                node["state"]=state
                                
            time.sleep(10)
                        
    
    def check_all_ec2_nodes(self):
        """ check whether all nodes in self.nodes list are running 
            return TRUE if all nodes are running otherwise FALSE """
        counter = 0 
        for i in self.nodes:
            if i["state"]=="running":
                counter = counter + 1
                
        if counter == len(self.nodes):
            return True
        else: 
            return False
        
    def read_ec2_environments(self, env_file):
        """ read required env variables for Eucalyptus """
        euca_dir=os.path.dirname(env_file)
        #open file
        f = open(env_file)
        env = f.readlines()
        env_regex = re.compile("(export\s)(\w*)=(\S*)")
        env_dict = {}
        for i in env:
            try:
                m = env_regex.search(i)
                key = m.group(2)
                value = m.group(3)
                #expand EUCA_KEY_DIR environment variable
                value = value.replace("""${EUCA_KEY_DIR}""", euca_dir)
                env_dict[key]=value
            except:
                pass
        f.close() 
        return env_dict
    
    def stop_ec2_images(self):
        """ terminate all ec2 instances managed by this pilot job """
        instances = ""
        for i in self.nodes:
            instances = instances + i["vmid"] + " "
            
        command = self.env_dict["EC2_HOME"]  + "/bin/ec2-terminate-instances " + instances
        self.execute_command(command, self.working_directory, self.env_dict)
         
    
    def execute_command(self, command, working_directory, environment_variable):
        print "execute: " + command + " in " + working_directory              
        start=time.time()
        p = subprocess.Popen(args=command, 
                            stdin=None,
                            stderr=subprocess.STDOUT,
                            stdout=subprocess.PIPE,
                            cwd=working_directory, 
                            shell=True,
                            env=environment_variable)
        print "started " + command 
        p.wait()
        end = time.time()     
        print "Execution Time: " + str(end-start) + " s."  
        stdout = p.communicate()[0]
        return stdout  
        
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
        if self.cloud_type=="EC2":
            self.stop_ec2_images()
        else:
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
                    if self.job_service_cache.has_key(job_service_url):
                        job_service = self.job_service_cache[job_service_url]
                    else:
                        job_service = saga.job.service(self.session, job_service_url)
                        self.job_service_cache[job_service_url] =job_service
                        
                        
                    job = job_service.create_job(jd)
                    job.run()
                    
                    # store objects for later use
                    job_dict["job_service_url"]=job_service_url
                    #job_dict["job_service"]=job_service
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
        else:
                print "BigJob: " + str(self.pilot_url) + ": Not sufficient resources for job."
                self.resource_lock.release()
                return ""
            
    
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
            if i.has_key("private_hostname"):
                nodefile_string=nodefile_string + "host "+ i["private_hostname"] + " ++cpus " + str(i["cpu_count"]) + " ++shell ssh\n"
            else:
                nodefile_string=nodefile_string + "host "+ i["hostname"] + " ++cpus " + str(i["cpu_count"]) + " ++shell ssh\n"
            
        # copy nodefile to rank 0 node
        jd = saga.job.description()
        jd.executable = "echo"
        jd.number_of_processes = "1"
        jd.spmd_variation = "single"
        # ssh root@tp-x001.ci.uchicago.edu "cat ~/.ssh/id_rsa.pub >> ~/.ssh/authorized_keys"
        jd.arguments = ["\""+nodefile_string+"\"", ">", "machinefile"]
        jd.output = "stdout.txt"
        jd.error = "stderr.txt"
        
        job_service_url = saga.url("ssh://root@"+allocated_nodes[0]["hostname"])
        job_service = saga.job.service(self.session, job_service_url)
        job = job_service.create_job(jd)
        job.run()
        job.wait()
             
        
    
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
    
class UnsupportedCloudType(Exception):
    def __init__(self, value):
        self.value = value

    def __str__(self):
        return repr(self.value)
    

