""" Module API

Bigjob Implementation with Diane backend

"""


"""
Job States

* Unknown
  Not part of the SPEC...

* New
  This state identies a newly constructed job instance which has not yet run.
  This state corresponds to the BES state Pending. This state is initial.

* Running     
  The run() method has been invoked on the job, either explicitly or implicitly.
  This state corresponds to the BES state Running. This state is initial.

* Done    
  The synchronous or asynchronous operation has finished successfully. It
  corresponds to the BES state Finished. This state is final.

* Canceled    
  The asynchronous operation has been canceled, i.e. cancel() has been called on
  the job instance. It corresponds to the BES state Canceled. This state is final.

* Failed  
  The synchronous or asynchronous operation has finished unsuccessfully. It
  corresponds to the BES state Failed. This state is final.

* Suspended   
  Suspended identifies a job instance which has been suspended. This state
  corresponds to the BES state Suspend. 

"""

import api.base
import saga
import os
import sys
from stat import *
from subprocess import *
import shutil
from multiprocessing.connection import Client

def get_rundir():
    p1 = Popen(["diane-ls"], stdout=PIPE)
    p2 = Popen(["head", "-n1"], stdin=p1.stdout, stdout=PIPE)
    p3 = Popen(["cut", "-f1", "-d "], stdin=p2.stdout, stdout=PIPE)
    p2.stdout.close()  # Allow p2 to receive a SIGPIPE if p2 exits.
    p1.stdout.close()  # Allow p1 to receive a SIGPIPE if p2 exits.
    rundir = p3.communicate()[0].strip()

    return rundir



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


class bigjob(api.base.bigjob):
       
    def __init__(self, database_host=None):  
        self.pilot_url = "diane://"
        self.uuid = get_uuid()
        print 'uuid of bigjob:', self.uuid
        self.state = 'Unknown'

        # TODO: create DIANE master object
        print "Launching Diane Master"
        os.system("~/proj/bigjob/diane/run_master.sh")

    def __del__(self):
        # TODO: kill the right master, not the most recent
        print 'Killing the master'
        os.system("diane-master-ping kill")
        self.conn.close()
    
    def start_pilot_job(self, 
                 resource_url, 
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

        print 'submitting worker, resource url: %s workdir: %s number of nodes: %s' % (resource_url, working_directory, number_nodes)
        os.system("~/proj/bigjob/diane/submit_worker.sh %s %s %s" % \
                (resource_url, working_directory, number_nodes))

        self.rundir = get_rundir()
        print 'Rundir:', self.rundir

        self.state = 'New'

        self.__setup_connection()

    def __setup_connection(self):

        address = (self.rundir + '/socket')
        print 'connecting to', address
        self.conn = Client(address, authkey='D I A N E !')
        

    def get_state(self):        
        # TODO

        print 'get_state:', self.state
        req = "status?"

        #self.sock_conn.send(req)
        self.conn.send(req)

        if self.conn.poll():
            rep = self.conn.recv()
            print 'received answer from socket:', rep
            self.state = rep
        else:
            print 'no data received'
                 
        print 'Bigjob get_state:', self.state
        return self.state
         
    
    def get_state_detail(self): 
        # TODO
        print 'get_state_detail'
        return "running"
    
    def cancel(self):        
        # TODO: kill master? agents?
        pass
                    
                    
class subjob(api.base.subjob):
    
    def __init__(self, database_host=None):
        print 'constructor of subjob'
        self.state = 'Unknown'
        self.uuid = get_uuid()
        print 'uuid of subjob:', self.uuid

        # TODO: this incorrectly assumes that we use the most recent rundir
        self.rundir = get_rundir()
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

        scriptname = self.uuid + '.sh'

        if os.path.isfile(self.rundir + '/submitted/' + scriptname ):
            pass
        if os.path.isfile(self.rundir + '/scheduled/' + scriptname ):
            self.state = 'New'
        if os.path.isfile(self.rundir + '/running/' + scriptname):
            self.state = 'Running'
        if os.path.isfile(self.rundir + '/done/' + scriptname):
            self.state = 'Done'
        if os.path.isfile(self.rundir + '/cancelled/' + scriptname):
            self.state = 'Canceled'
        if os.path.isfile(self.rundir + '/failed/' + scriptname):
            self.state = 'Failed'
            
        print 'DEBUG: subjob state:', self.state
        return self.state
    
    def cancel(self):
        # TODO: move and remove in directory structure
        pass
