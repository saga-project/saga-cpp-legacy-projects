""" Module API

Bigjob Implementation with Diane backend

"""

import api.base
import saga
import os
import sys
from stat import *
from subprocess import *
import shutil

def get_uuid():
    wd_uuid=""
    if sys.version_info < (2, 5):
        uuid_str = os.popen("/usr/bin/uuidgen").read()
        wd_uuid += uuid_str.rstrip("\n")
            
        #preparation for fail-safe exit
        #sys.stderr.write('Incompatible Python version found! Please use Python 2.5 or higher with BigJob!') 
        #sys.exit(-1)
    else:   
        import uuid
        wd_uuid += str(uuid.uuid1())
    return wd_uuid


class bigjob_diane(api.base.bigjob):
       
    def __init__(self, database_host=None):  
        self.pilot_url = "diane://"
        self.uuid = get_uuid()
        self.state = saga.job.Unknown

        print "Launching Diane Master"
        os.system("~/proj/bigjob/diane/run_master.sh")
    
    def start_pilot_job(self, 
                 lrms_url, 
                 bigjob_agent_executable,
                 number_nodes,
                 queue,
                 project,
                 working_directory,
                 userproxy,
                 walltime,
                 processes_per_node):
        
        print "Submit Diane Worker"

        # Layer violation!
        if bigjob_agent_executable is not None:
            print "Agent Executable is specified, but ignoring it!"

        os.system("~/proj/bigjob/diane/submit_worker.sh")
                 
         
    def get_state(self):        
        return self.state
    
    def get_state_detail(self): 
        pass
    
    def cancel(self):        
        pass
                    
                    
class subjob(api.base.subjob):
    
    def __init__(self, database_host=None):
        self.state = saga.job.Unknown
        self.uuid = get_uuid()
        p1 = Popen(["diane-ls"], stdout=PIPE)
        p2 = Popen(["head", "-n1"], stdin=p1.stdout, stdout=PIPE)
        p3 = Popen(["cut", "-f1", "-d "], stdin=p2.stdout, stdout=PIPE)
        p2.stdout.close()  # Allow p2 to receive a SIGPIPE if p2 exits.
        p1.stdout.close()  # Allow p1 to receive a SIGPIPE if p2 exits.
        self.rundir = p3.communicate()[0].strip()

        print 'Rundir:', self.rundir
    
    def submit_job(self, pilot_url, jd):
        print "Starting Submit_job ..."

        scriptname = '/tmp/' + self.uuid + ".sh"
        f = open(scriptname, 'w')
        f.write('#!/bin/sh\n')
        f.write(jd.executable)
        for a in jd.arguments:
            f.write(' ' + a)
        f.write('\n')
        f.close()
        os.chmod(scriptname, 0755)

        shutil.move(scriptname,os.path.join(self.rundir,'submitted'))

    def get_state(self):        
        return self.state
    
    def delete_job(self):
        pass
