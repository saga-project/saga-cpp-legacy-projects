#!/usr/bin/env python

"""Module big_job.

This Module is used to launch jobs via the advert service. 

It assumes that an bigjob_agent.py is available on the remote machine.
bigjob_agent.py will poll the advert service for new jobs and run these jobs on the respective
machine .

Background: This approach avoids queueing delays since the igjob_agent_launcher.py must be just started via saga.job or saga.cpr
once. All shortrunning task will be started using the protocol implemented by subjob() and bigjob_agent.py

Installation:
Set environment variable BIGJOB_HOME to installation directory

"""

import sys
import getopt
import saga
import time
import pdb
import socket
import os
import traceback
import logging

# import other BigJob packages
# import API
import api.base

if sys.version_info < (2, 5):
    sys.path.append(os.path.dirname( __file__ ) + "/ext/uuid-1.30/")
    sys.stderr.write("Warning: Using unsupported Python version\n")
if sys.version_info < (2, 4):
    sys.path.append(os.path.dirname( __file__ ) + "/ext/subprocess-2.6.4/")
    sys.stderr.write("Warning: Using unsupported Python version\n")
if sys.version_info < (2, 3):
    sys.stderr.write("Error: Python versions <2.3 not supported\n")
    sys.exit(-1)

import uuid

def get_uuid():
    wd_uuid=""
    #uuid_str = os.popen("/usr/bin/uuidgen").read()
    #wd_uuid += uuid_str.rstrip("\n")
        
    #preparation for fail-safe exit
    #sys.stderr.write('Incompatible Python version found! Please use Python 2.5 or higher with BigJob!') 
    #sys.exit(-1)
    #else:
    #    import uuid
    wd_uuid += str(uuid.uuid1())
    return wd_uuid


""" Config parameters (will move to config file in future) """
APPLICATION_NAME="BigJob/BigJob"
#ADVERT_URL_SCHEME = "advert://"
ADVERT_URL_SCHEME="advert://"

class BigJobError(Exception):
    def __init__(self, value):
        self.value = value
    
    def __str__(self):
        return repr(self.value)

class bigjob(api.base.bigjob):
    
    def __init__(self, database_host):        
        self.database_host = database_host
        print "init advert service session at host: " + database_host
        self.uuid = get_uuid()
        self.app_url = saga.url(ADVERT_URL_SCHEME+ database_host + "/"+APPLICATION_NAME + "-" + str(self.uuid) + "/")
        self.app_dir = saga.advert.directory(self.app_url, saga.advert.Create | saga.advert.CreateParents | saga.advert.ReadWrite)
        self.state=saga.job.Unknown
        self.pilot_url=""
        self.job = None
        print "created advert directory for application: " + self.app_url.get_string()
    
    def start_pilot_job(self, 
                 lrms_url, 
                 bigjob_agent_executable,
                 number_nodes,
                 queue,
                 project,
                 working_directory,
                 userproxy,
                 walltime,
                 processes_per_node=1):
        
        
        if self.job != None:
            raise BigJobError("One BigJob already active. Please stop BigJob first.") 
            return


        #register advert entry
        lrms_saga_url = saga.url(lrms_url)
        self.pilot_url = self.app_url.get_string() + "/" + lrms_saga_url.host
        print "create advert entry: " + self.pilot_url
        self.pilot_dir = saga.advert.directory(saga.url(self.pilot_url), saga.advert.Create | saga.advert.CreateParents | saga.advert.ReadWrite)
        # application level state since globus adaptor does not support state detail
        self.pilot_dir.set_attribute("state", str(saga.job.Unknown)) 
        self.pilot_dir.set_attribute("stopped", "false")
        logging.debug("set pilot state to: " + self.pilot_dir.get_attribute("state"))
        self.number_nodes=int(number_nodes)        
        
        # discover location of agent executable
        if bigjob_agent_executable==None:            
            if os.getenv("BIGJOB_HOME", None)!=None:
                bigjob_agent_executable=os.getenv("BIGJOB_HOME")+"/bigjob_agent_launcher.sh"
            else:
                bigjob_agent_executable=os.getcwd()+"/bigjob_agent_launcher.sh"
 
        # create job description
        jd = saga.job.description()
        jd.number_of_processes = str(int(number_nodes) * int(processes_per_node) )
        jd.processes_per_host=str(processes_per_node)
        jd.spmd_variation = "single"
        jd.arguments = [bigjob_agent_executable, self.database_host, self.pilot_url]
        jd.executable = "/bin/bash"
        #jd.executable = bigjob_agent_executable
        if queue != None:
            jd.queue = queue
        if project !=None:
            jd.job_project = [project]
        if walltime!=None:
            jd.wall_time_limit=str(walltime)

        # XXX Isn't the working directory about the remote site?
        if working_directory != None:
            if not os.path.isdir(working_directory) and lrms_saga_url.scheme=="fork":
                os.mkdir(working_directory)
            jd.working_directory = working_directory
        else:
            jd.working_directory = "$(HOME)"
            
        print "Working directory: " + jd.working_directory
        
        jd.output = "stdout-bigjob_agent-" + str(self.uuid) + ".txt"
        jd.error = "stderr-bigjob_agent-" + str(self.uuid) + ".txt"
           
        # Submit job
        js = None	
        if userproxy != None and userproxy != '':
      	    s = saga.session()
            os.environ["X509_USER_PROXY"]=userproxy
            ctx = saga.context("x509")
            ctx.set_attribute ("UserProxy", userproxy)
            s.add_context(ctx)
            print "use proxy: " + userproxy
            js = saga.job.service(s, lrms_saga_url)
        else:
            print "use standard proxy"
            js = saga.job.service(lrms_saga_url)

        self.job = js.create_job(jd)
        print "Submit pilot job to: " + str(lrms_saga_url)
        self.job.run()
        #return self.job
     
    def get_state(self):        
        """ duck typing for get_state of saga.cpr.job and saga.job.job  """
        try:
            return self.job.get_state()
        except:
            return None
    
    def get_state_detail(self): 
        try:
            return self.pilot_dir.get_attribute("state")
        except:
            return None
    
    def get_free_nodes(self):
        jobs = self.pilot_dir.list()
        number_used_nodes=0
        for i in jobs:
            job_dir=None
            try:
                job_dir = self.pilot_dir.open_dir(i.get_string(), saga.advert.Create | saga.advert.ReadWrite) 
            except:
                pass
            if job_dir != None and job_dir.attribute_exists("state") == True\
                and str(job_dir.get_attribute("state"))==str(saga.job.Running):
                job_np = "1"
                if (job_dir.attribute_exists("NumberOfProcesses") == True):
                    job_np = job_dir.get_attribute("NumberOfProcesses")

                number_used_nodes=number_used_nodes + int(job_np)
        return (self.number_nodes - number_used_nodes)

    
    def stop_pilot_job(self):
        """ mark in advert directory of pilot-job as stopped """
        try:
            #self.app_dir.change_dir("..")
            print "stop pilot job: " + str(self.app_url)
            self.pilot_dir.set_attribute("stopped", "true")
            self.job=None
        except:
            pass
    
    def cancel(self):        
        """ duck typing for cancel of saga.cpr.job and saga.job.job  """
        print "Cancel Pilot Job"
        try:
            self.job.cancel()
        except:
            pass
            #traceback.print_stack()
        try:
            #self.app_dir.change_dir("..")
            print "delete pilot job: " + str(self.app_url)
            self.app_dir.remove(self.app_url, saga.name_space.Recursive)    
        except:
            pass
            #traceback.print_stack()

    def __repr__(self):
        return self.pilot_url 

    def __del__(self):
        self.cancel()

                    
                    
class subjob(api.base.subjob):
    
    def __init__(self, database_host):
        """Constructor"""
        self.database_host = database_host
        self.job_url=None
        self.uuid = get_uuid()
        self.job_url = None
        
    def get_job_url(self, pilot_url):
        self.saga_pilot_url = saga.url(pilot_url)
        if(self.saga_pilot_url.scheme=="advert" or self.saga_pilot_url.scheme=="sqlfastadvert"):  #
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
            self.saga_pilot_url = saga.url(ADVERT_URL_SCHEME +  self.database_host + "/"+APPLICATION_NAME + "/" + host)

        # create dir for job
        self.job_url = self.saga_pilot_url.get_string() + "/" + str(self.uuid)
        return self.job_url

    def submit_job(self, pilot_url, jd):
        """ submit job via advert service to NAMD-Launcher 
            dest_url - url reference to advert job or host on which the advert job is going to run"""
        print "submit job: " + str(pilot_url)
        if self.job_url==None:
            self.job_url=self.get_job_url(pilot_url)

        for i in range(0,3):
            try:
                print "create job entry "
                self.job_dir = saga.advert.directory(saga.url(self.job_url), 
                                             saga.advert.Create | saga.advert.CreateParents | saga.advert.ReadWrite)
                print "initialized advert directory for job: " + self.job_url
                # put job description attributes to advert
                attributes = jd.list_attributes()                
                for i in attributes:          
                        if jd.attribute_is_vector(i):
                            self.job_dir.set_vector_attribute(i, jd.get_vector_attribute(i))
                        else:
                            logging.debug("Add attribute: " + str(i) + " Value: " + jd.get_attribute(i))
                            self.job_dir.set_attribute(i, jd.get_attribute(i))

                self.job_dir.set_attribute("state", str(saga.job.Unknown))
                
                print "create notification flag in NEW dir"
                self.new_job_url = self.saga_pilot_url.get_string() + "/new/" + str(self.uuid)
                self.new_job_dir = saga.advert.directory(saga.url(self.new_job_url), 
                                             saga.advert.Create | saga.advert.CreateParents | saga.advert.ReadWrite)
                self.new_job_dir.set_attribute("joburl", self.job_url)
                                
                # return self object for get_state() query    
                #logging.debug("Submission time (time to create advert entries): " + str(time.time()-start) + " s")
                return self    
            except:
                traceback.print_exc(file=sys.stdout)
                time.sleep(2)
                #raise Exception("Unable to submit job")      

    def get_state(self):        
        """ duck typing for get_state of saga.cpr.job and saga.job.job  """
        return self.job_dir.get_attribute("state")

    def get_exe(self):
        return self.job_dir.get_attribute("Executable")
   
    def get_arguments(self):
        arguments=""
        for  i in  self.job_dir.get_vector_attribute("Arguments"):
              arguments = arguments + " " + i
        return arguments
 

    def cancel(self):
        print "delete job and close dirs: " + self.job_url
        try:
            self.job_dir.change_dir("..")
            self.job_dir.remove(saga.url(self.job_url), saga.name_space.Recursive)
            self.job_dir.close()
        except:
            pass

    def __del__(self):
        self.cancel()
    
    def __repr__(self):        
        if(self.job_url==None):
            return "None"
        else:
            return self.job_url

