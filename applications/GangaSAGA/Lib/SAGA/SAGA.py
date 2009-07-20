# Copyright information
__author__  = "Ole Weidner <oweidner@cct.lsu.edu>"
__date__    = "20 July 2009"
__version__ = "1.0"

# Ganga GPIDev Imports
from Ganga.GPIDev.Adapters.IBackend import IBackend
from Ganga.GPIDev.Schema import *
from Ganga.GPIDev.Lib.File import FileBuffer

# Setup Utility Logger
import Ganga.Utility.logging
logger = Ganga.Utility.logging.getLogger()

# Setup Utility Config
import Ganga.Utility.Config
config = Ganga.Utility.Config.makeConfig('SAGA','parameters of the SAGA backend')
config.addOption('remove_workdir', True, 'remove automatically the local working directory when the job completed')

# Other Utility Imports
import Ganga.Utility.util
import Ganga.Utility.logic

# SAGA Job Package Imports
import saga.job

# Python System Imports
import os,sys
import os.path,re,errno
import subprocess
import string

##############################################################################
##
class SAGA(IBackend):
    
    """Run jobs in the background using SAGA.
       The job is run localy or remotely - depending on which SAGA adaptor is selected. 
    """
    
    # Set category & unique backend name
    _category = 'backends'
    _name = 'SAGA'
    
    # Setup Job Attributes
    _schema = Schema(Version(1,2), {
        'status' : SimpleItem(defvalue=None,typelist=None,protected=1,copyable=0,hidden=1,doc='*NOT USED*'),
        'actualCE' : SimpleItem(defvalue='',protected=1,copyable=0,doc='Hostname where the job was submitted.'),
        'wrapper_pid' : SimpleItem(defvalue=-1,protected=1,copyable=0,hidden=1,doc='(internal) process id of the execution wrapper'),
                                    
        ## SAGA specific attributes                           
        'workdir' : SimpleItem(defvalue='',protected=1,copyable=0,doc='JSDL Attribute: Working Directory'),
        'queue' : SimpleItem(defvalue='',protected=1,copyable=0,doc='JSDL Attribute: Queue.'),
        'rm_url' : SimpleItem(defvalue='gram://qb1.loni.org', doc='Resource manager URL that will be passed to SAGA'),
        'saga_job_id' : SimpleItem(defvalue='',protected=1,copyable=0,doc='SAGA-internal Process id.') 
    })
    
    _GUIPrefs = [ { 'attribute' : 'rm_url', 'widget' : 'String' },
                  { 'attribute' : 'id', 'widget' : 'Int' },
                  { 'attribute' : 'status' , 'widget' : 'String' },
                  { 'attribute' : 'exitcode', 'widget' : 'String' } ]
                  
    _GUIAdvancedPrefs = [ { 'attribute' : 'nice', 'widget' : 'String' },
                          { 'attribute' : 'exitcode', 'widget' : 'String' } ]    


    ##########################################################################
    ##
    def __init__(self):
      super(SAGA,self).__init__()
        

    ##########################################################################
    ## Tries to submit a ganga job through saga
    ##
    def submit(self, jobconfig, master_input_sandbox):
        try: 
            jd = saga.job.description()
            jd = self.makesagajobdesc(self.getJobObject())
                  
            self.run(self.rm_url, jd)
                
        except saga.exception, e:
            logger.error('exception caught while submitting job: %s', str(e))
            return 0
      
        return 1 # sets job to 'submitted'

    ##########################################################################
    ## Tries to resubmit an existing ganga job through saga
    ##
    def resubmit(self):                
        try: 
            jd = saga.job.description()
            jd = self.makesagajobdesc(self.getJobObject())
                  
            self.run(self.rm_url, jd)
                
        except saga.exception, e:
            logger.error('exception caught while submitting job: %s', str(e))
            return 0
      
        return 1 # sets job to 'submitted'

           
      
    ##########################################################################
    ## Run a SAGA job
    ##
    def run(self, js_contact, jd):
        # actualCE is the same as the resource manager (for now)
        self.actualCE = self.rm_url;
        
        try:
            js_url = saga.url(self.rm_url)
            js =  saga.job.service(js_url)
            saga_job = js.create_job(jd)
      
            # execute the job
            saga_job.run()
            self.saga_job_id = saga_job.get_job_id()  
                  
        except saga.exception, e:
            logger.error('exception caught while submitting job: %s', str(e))
            return 0
        
        return 1


    ##########################################################################
    ## Tries to kill a running saga job
    ##        
    def kill(self):
        job = self.getJobObject()
        
        # create a new service object in order
        # to reconnect to the job we want to kill
        try :
            js_url = saga.url(job.backend.rm_url)
            js = saga.job.service(js_url)
            saga_job = js.get_job(job.backend.saga_job_id)
            
            # KILL KILL KILL
            saga_job.cancel()
                            
        except saga.exception, e:
            logger.error('exception caught while killing job: %s', str(e))
            return 0
            
        return 1 # sets job to 'killed'

    ##########################################################################
    ## Creates and returns a saga.job.description from a given
    ## job object
    ##
    def makesagajobdesc(self, job):
        jd = saga.job.description()
        
        # executable
        jd.executable = job.application.exe
        
        # arguments
        argList = []
        for arg in job.application.args:
            argList.append( arg ) #"\\'%s\\'" % arg ) 
        jd.arguments = argList

        # environment
        envList = []
        for env in job.application.env:
            envList.append( arg ) #"\\'%s\\'" % arg ) 
        jd.environment = envList

        # workdir
        jd.workdir = job.backend.workdir
        
        # queue
        jd.queue = job.backend.queue
        
        return jd
                

    ##########################################################################
    ## Method gets triggered by a ganga monitoring thread periodically 
    ## in order to update job information, like state, etc...
    ##
    def updateMonitoringInformation(jobs):
    
        ###################################################
        ## Translates a saga job state to a ganga job state
        ##
        def sagatogangastatus(sagastatus):
                
            if sagastatus == saga.job.Done:
                return "completed"
            elif sagastatus == saga.job.Running:
                return "running"
        ##
        ##################################################
        
        for j in jobs:
            # Skip job status query in case the job is already in
            # 'completed' status. That should avoid a great amount of
            # overhead if there are many completed jobs in the list
            if j.status == 'completed':
                continue
        
            # create a new service object in order
            # to reconnect to the job
            try :
                js_url = saga.url(j.backend.rm_url)
                js = saga.job.service(js_url)
                saga_job = js.get_job(j.backend.saga_job_id)
            
                # query saga job state
                job_state = saga_job.get_state()
            
                # translate saga job state to ganga job state
                j.updateStatus(sagatogangastatus(job_state))
                
            except saga.exception, e:
                logger.error('exception caught while updating job: %s', str(e))
    
    ##########################################################################
    ## Make the monitoring function available to the update thread
    ##         
    updateMonitoringInformation = staticmethod(updateMonitoringInformation)

                        
            









    ##################################################
    ##
    #def resubmit(self):
    #  job = self.getJobObject()
    #  import shutil
    #
    #  try:
    #      shutil.rmtree(self.workdir)
    #  except OSError,x:
    #      import errno
    #      if x.errno != errno.ENOENT:
    #          logger.error('problem cleaning the workdir %s, %s',self.workdir,str(x))
    #          return 0
    #  try:
    #      os.mkdir(self.workdir)
    #  except Exception,x:
    #    logger.error('cannot make the workdir %s, %s',self.workdir,str(x))
    #    return 0
    #  return self.run(job.getInputWorkspace().getPath('__jobscript__'))
      
      

    ##################################################
    ##
    def peek( self, filename = "", command = "" ):
      """
      Allow viewing of output files in job's work directory
      (i.e. while job is in 'running' state)
                                                                                
      Arguments other than self:
      filename : name of file to be viewed
                => Path specified relative to work directory
      command  : command to be used for file viewing
                                                                                
      Return value: None
      """
      job = self.getJobObject()
      topdir = self.workdir.rstrip( os.sep )
      path = os.path.join( topdir, filename ).rstrip( os.sep )
      job.viewFile( path = path, command = command )
      return None
      

    ##################################################
    ##
    def preparejob(self,jobconfig,master_input_sandbox):

      job = self.getJobObject()

      subjob_input_sandbox = job.createPackedInputSandbox(jobconfig.getSandboxFiles())
      appscriptpath = [jobconfig.getExeString()]+jobconfig.getArgStrings()
      #if self.nice:
      #    appscriptpath = ['nice','-n %d'%self.nice] + appscriptpath
      #if self.nice < 0:
      #    logger.warning('increasing process priority is often not allowed, your job may fail due to this')
          
      sharedoutputpath=job.getOutputWorkspace().getPath()
      outputpatterns=jobconfig.outputbox
      environment=jobconfig.env

      from Ganga.Utility import tempfile
      workdir = tempfile.mkdtemp()
      
      script= """#!/usr/bin/env python

import os,os.path,shutil,tempfile
import sys,popen2,time

import sys

# FIXME: print as DEBUG: to __syslog__ file
#print sys.path
#print os.environ['PATH']
#print sys.version

# bugfix #13314 : make sure that the wrapper (spawned process) is detached from Ganga session
# the process will not receive Control-C signals
# using fork  and doing setsid() before  exec would probably  be a bit
# better (to avoid  slim chance that the signal  is propagated to this
# process before setsid is reached)
# this is only enabled if the first argument is 'subprocess' in order to enable
# running this script by hand from outside ganga (which is sometimes useful)
if len(sys.argv)>1 and sys.argv[1] == 'subprocess':
 os.setsid()

############################################################################################

###INLINEMODULES###

############################################################################################

input_sandbox = ###INPUT_SANDBOX###
sharedoutputpath= ###SHAREDOUTPUTPATH###
outputpatterns = ###OUTPUTPATTERNS###
appscriptpath = ###APPSCRIPTPATH###
environment = ###ENVIRONMENT###
workdir = ###WORKDIR###

statusfilename = os.path.join(sharedoutputpath,'__jobstatus__')

try:
  statusfile=file(statusfilename,'w')
except IOError,x:
  print 'ERROR: not able to write a status file: ', statusfilename
  print 'ERROR: ',x
  raise
  
line='START: '+ time.strftime('%a %b %d %H:%M:%S %Y',time.gmtime(time.time())) + os.linesep
statusfile.writelines(line)
statusfile.flush()

os.chdir(workdir)

import sys
sys.path.insert(0, ###GANGADIR###)
try:
    import subprocess
except ImportError,x:
    sys.path.insert(0,###SUBPROCESS_PYTHONPATH###)
    import subprocess
try:
    import tarfile
except ImportError,x:
    sys.path.insert(0,###TARFILE_PYTHONPATH###)
    import tarfile

for f in input_sandbox:
  getPackedInputSandbox(f)

for key,value in environment.iteritems():
    os.environ[key] = value

outfile=file('stdout','w')
errorfile=file('stderr','w')

sys.stdout=file('./__syslog__','w')
sys.stderr=sys.stdout

###MONITORING_SERVICE###
monitor = createMonitoringObject()
monitor.start()

import subprocess

try:
 child = subprocess.Popen(appscriptpath, shell=False, stdout=outfile, stderr=errorfile)
except OSError,x:
 file('tt','w').close()
 print >> statusfile, 'EXITCODE: %d'%-9999
 print >> statusfile, 'PROBLEM STARTING THE APPLICATION SCRIPT: %s %s'%(appscriptpath,str(x))
 statusfile.close()
 sys.exit()
 
print >> statusfile, 'PID: %d'%child.pid
statusfile.flush()

result = -1

try:
  while 1:
    result = child.poll()
    if result is not None:
        break
    outfile.flush()
    errorfile.flush()
    monitor.progress()
    time.sleep(0.3)
finally:
    monitor.progress()
    sys.stdout=sys.__stdout__
    sys.stderr=sys.__stderr__

monitor.stop(result)

outfile.flush()
errorfile.flush()

createOutputSandbox(outputpatterns,None,sharedoutputpath)

outfile.close()
errorfile.close()

from Ganga.Utility.files import recursive_copy

for fn in ['stdout','stderr','__syslog__']:
    try:
        recursive_copy(fn,sharedoutputpath)
    except Exception,x:
        print 'ERROR: (job'+###JOBID###+')',x

line="EXITCODE: " + repr(result) + os.linesep
line+='STOP: '+time.strftime('%a %b %d %H:%M:%S %Y',time.gmtime(time.time())) + os.linesep
statusfile.writelines(line)
sys.exit()

"""

      import inspect
      import Ganga.Core.Sandbox as Sandbox
      script = script.replace('###INLINEMODULES###',inspect.getsource(Sandbox.WNSandbox))

      script = script.replace('###APPLICATION_NAME###',repr(job.application._name))
      script = script.replace('###INPUT_SANDBOX###',repr(subjob_input_sandbox+master_input_sandbox))
      script = script.replace('###SHAREDOUTPUTPATH###',repr(sharedoutputpath))
      script = script.replace('###APPSCRIPTPATH###',repr(appscriptpath))
      script = script.replace('###OUTPUTPATTERNS###',repr(outputpatterns))
      script = script.replace('###JOBID###',repr(job.getFQID('.')))
      script = script.replace('###ENVIRONMENT###',repr(environment))
      script = script.replace('###WORKDIR###',repr(workdir))
      
      script = script.replace('###MONITORING_SERVICE###',job.getMonitoringService().getWrapperScriptConstructorText())
      
      self.workdir = workdir
      
      from Ganga.Utility.Config import getConfig

      script = script.replace('###GANGADIR###',repr(getConfig('System')['GANGA_PYTHONPATH']))

      import Ganga.PACKAGE
      script = script.replace('###SUBPROCESS_PYTHONPATH###',repr(Ganga.PACKAGE.setup.getPackagePath2('subprocess','syspath',force=True)))
      script = script.replace('###TARFILE_PYTHONPATH###',repr(Ganga.PACKAGE.setup.getPackagePath2('tarfile','syspath',force=True)))

      return job.getInputWorkspace().writefile(FileBuffer('__jobscript__',script),executable=1)

#    def kill(self):
#        import os,signal
#
#        job = self.getJobObject()
#        
#        ok = True
#        try:
#            # kill the wrapper script
#            # bugfix: #18178 - since wrapper script sets a new session and new group, we can use this to kill all processes in the group
#            os.kill(-self.wrapper_pid,signal.SIGKILL)
#        except OSError,x:
#            logger.warning('while killing wrapper script for job %d: pid=%d, %s',job.id,self.wrapper_pid,str(x))
#            ok = False
#
#        # waitpid to avoid zombies
#        try:
#            ws = os.waitpid(self.wrapper_pid,0)
#        except OSError,x:
#            logger.warning('problem while waitpid %d: %s',job.id,x)
#
#        from Ganga.Utility.files import recursive_copy
#
#        for fn in ['stdout','stderr','__syslog__']:
#            try:
#                recursive_copy(os.path.join(self.workdir,fn),job.getOutputWorkspace().getPath())
#            except Exception,x:
#                logger.info('problem retrieving %s:',fn,x)
#
#        self.remove_workdir()
#        return 1

    def remove_workdir(self):
        if config['remove_workdir']:
            import shutil
            try:
                shutil.rmtree(self.workdir)
            except OSError,x:
                logger.warning('problem removing the workdir %s: %s',str(self.id),str(x))        
    
#    def updateMonitoringInformation(jobs):
#
#      def get_exit_code(f):
#          import re
#          statusfile=file(f)
#          stat = statusfile.read()
#          m = re.compile(r'^EXITCODE: (?P<exitcode>-?\d*)',re.M).search(stat)
#
#          if m is None:
#              return None
#          else:
#              return int(m.group('exitcode'))
#
#      def get_pid(f):
#          import re
#          statusfile=file(f)
#          stat = statusfile.read()
#          m = re.compile(r'^PID: (?P<pid>\d*)',re.M).search(stat)
#
#          if m is None:
#              return None
#          else:
#              return int(m.group('pid'))          
#
#      logger.debug('local ping: %s',str(jobs))
#      
#      for j in jobs:
#          outw=j.getOutputWorkspace()
#
#          # try to get the application exit code from the status file
#          try:
#            statusfile = os.path.join(outw.getPath(),'__jobstatus__')
#            if j.status == 'submitted':
#                pid = get_pid(statusfile)
#                if pid:
#                    j.backend.id = pid
#                    #logger.info('Local job %d status changed to running, pid=%d',j.id,pid)
#                    j.updateStatus('running') # bugfix: 12194
#            exitcode = get_exit_code(statusfile)
#            logger.debug('status file: %s %s',statusfile,file(statusfile).read())
#          except IOError,x:
#            logger.debug('problem reading status file: %s (%s)',statusfile,str(x))
#            exitcode=None
#          except Exception,x:
#              logger.critical('problem during monitoring: %s',str(x))
#              import traceback
#              traceback.print_exc()
#              raise x
#
#          # check if the exit code of the wrapper script is available (non-blocking check)
#          # if the wrapper script exited with non zero this is an error
#          try:
#              ws = os.waitpid(j.backend.wrapper_pid,os.WNOHANG)
#              if not Ganga.Utility.logic.implies(ws[0]!=0,ws[1]==0):
#                  #FIXME: for some strange reason the logger DOES NOT LOG (checked in python 2.3 and 2.5)
#                  ##print 'logger problem', logger.name
#                  ##print 'logger',logger.getEffectiveLevel()
#                  logger.critical('wrapper script for job %s exit with code %d',str(j.id),ws[1])
#                  logger.critical('report this as a bug at http://savannah.cern.ch/bugs/?group=ganga')
#                  j.updateStatus('failed')
#          except OSError,x:
#              if x.errno != errno.ECHILD:
#                  logger.warning('cannot do waitpid for %d: %s',j.backend.wrapper_pid,str(x))#
#
#          # if the exit code was collected for the application get the exit code back
#          
#          if not exitcode is None:
#              # status file indicates that the application finished
#              j.backend.exitcode = exitcode#
#
#              if exitcode == 0:
#                  j.updateStatus('completed')
#              else:
#                  j.updateStatus('failed')
#              
#              #logger.info('Local job %d finished with exitcode %d',j.id,exitcode)
#
#              ##if j.outputdata:
#              ##    j.outputdata.fill()
#
#              j.backend.remove_workdir()
                  
                       
    
