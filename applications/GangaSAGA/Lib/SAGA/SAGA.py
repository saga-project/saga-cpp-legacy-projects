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
import uuid

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
                                    
        ## Public SAGA specific attributes - REQUIRED                        
        'remote_job_service' : SimpleItem(defvalue='', doc='Resource manager URL that will be passed to SAGA'),
        'remote_filesystem' : SimpleItem(defvalue='', doc='Resource manager URL that will be passed to SAGA'),
        
        ## Public SAGA specific attributes - OPTIONAL
        'workdir' : SimpleItem(defvalue='',protected=1,copyable=0,doc='JSDL Attribute: Working Directory'),
        'queue' : SimpleItem(defvalue='',protected=1,copyable=0,doc='JSDL Attribute: Queue.'),
        'saga_job_id' : SimpleItem(defvalue='',protected=1,copyable=0,doc='SAGA-internal Process id.'),
        
        ## Hidden SAGA specific attributes
        'saga_job_out' : SimpleItem(defvalue='',protected=1,copyable=0,doc='SAGA-internal Process id.'),
        'saga_job_err' : SimpleItem(defvalue='',protected=1,copyable=0,doc='SAGA-internal Process id.')
        
    })
    
    _GUIPrefs = [ { 'attribute' : 'remote_job_service', 'widget' : 'String' },
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
    ## Tries to create the local working directories properly
    ##
    def setupworkdir(self, path):
        import shutil, errno
        
        job = self.getJobObject()
        # As a side effect, these functions create the
        # workspace directories if they don't exist
        input_wd_path = job.getInputWorkspace().getPath()
        output_wd_path = job.getOutputWorkspace().getPath()
        
        logger.info('Local workspace - input dir: %s', input_wd_path)
        logger.info('Local workspace - output dir: %s', output_wd_path)
                

    ##########################################################################
    ## Tries to submit a ganga job through saga
    ##
    def submit(self, jobconfig, master_input_sandbox):
    
        # Make sure that all REQUIRED attributes are set
        if len(self.remote_job_service) == 0 :
            logger.error('remote_job_service attribute needs to be set')
            return 0
        if len(self.remote_filesystem) == 0 :
            logger.error('remote_filesystem attribute needs to be set')
            return 0
    
        self.setupworkdir(self.workdir)
        
        try: 
            jd = saga.job.description()
            jd = self.makesagajobdesc(self.getJobObject())
                  
            self.run(self.remote_job_service, jd)
                
        except saga.exception, e:
            logger.error('exception caught while submitting job: %s', 
                         e.get_full_message())
            return 0
      
        return 1 # sets job to 'submitted'
        

    ##########################################################################
    ## Tries to resubmit an existing ganga job through saga
    ##
    def resubmit(self):                
        try: 
            jd = saga.job.description()
            jd = self.makesagajobdesc(self.getJobObject())
                  
            self.run(self.remote_job_service, jd)
                
        except saga.exception, e:
            logger.error('exception caught while submitting job: %s', 
                         e.get_full_message())
            return 0
      
        return 1 # sets job to 'submitted'

           
      
    ##########################################################################
    ## Run a SAGA job
    ##
    def run(self, js_contact, jd):
        # actualCE is the same as the resource manager (for now)
        self.actualCE = self.remote_job_service;
        
        try:
            logger.info("Trying to run job on %s", self.remote_job_service);
            
            js_url = saga.url(self.remote_job_service)
            js =  saga.job.service(js_url)
            saga_job = js.create_job(jd)
      
            # execute the job
            saga_job.run()
            logger.info("Job state after submission: %s", saga_job.get_state()); 
            self.saga_job_id = saga_job.get_job_id()  
                  
        except saga.exception, e:
            logger.error('exception caught while submitting job: %s', 
                         e.get_full_message())
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
            js_url = saga.url(job.backend.remote_job_service)
            js = saga.job.service(js_url)
            saga_job = js.get_job(job.backend.saga_job_id)
            
            # KILL KILL KILL
            saga_job.cancel()
                            
        except saga.exception, e:
            logger.error('exception caught while killing job: %s', 
                         e.get_full_message())
            return 0
            
        return 1 # sets job to 'killed'
        

    ##########################################################################
    ## Creates and returns a saga.job.description from a given
    ## job object
    ##
    def makesagajobdesc(self, job):
        jd = saga.job.description()
        logger.info("Ganga Attributes -> SAGA JSDL translation:")
        
        # executable
        jd.executable = job.application.exe
        logger.info("  * application.exe -> saga.executable: %s", jd.executable)
        
        # arguments
        argList = []
        for arg in job.application.args:
            argList.append( arg ) #"\\'%s\\'" % arg ) 
        if len(argList) != 0:
            jd.arguments = argList
            logger.info("  * application.args -> saga.arguments: %s", jd.arguments)

        # environment
        envList = []
        for env in job.application.env:
            envList.append( arg ) #"\\'%s\\'" % arg ) 
        if len(envList) != 0:
            jd.environment = envList
            logger.info("  * application.env -> saga.environment: %s", jd.environment)

        # workdir
        if len(job.backend.queue) != 0:        
            jd.workdir = job.backend.workdir
            logger.info("  * backend.workdir -> saga.workdir: %s", jd.workdir)
       
        # queue
        if len(job.backend.queue) != 0:
            jd.queue = job.backend.queue
            logger.info("  * backend.queue -> saga.queue: %s", jd.workdir)

            
        ## We have to create special filenames for stdout/stderr redirection
        ## To avoid name clashes, we append a UUID to the filename.
        self.saga_job_out = 'ganga-'+str(uuid.uuid4())+'.out'
        jd.output = self.saga_job_out 
        logger.info("STDOUT should be available at %s",self.remote_filesystem+jd.output)

        self.saga_job_err = 'ganga-'+str(uuid.uuid4())+'.err'
        jd.error = self.saga_job_err
        logger.info("STDERR should be available at %s",self.remote_filesystem+jd.error)
        
        return jd
                

    ##########################################################################
    ## Method gets triggered by a ganga monitoring thread periodically 
    ## in order to update job information, like state, etc...
    ##
    def updateMonitoringInformation(jobs):
    
        ##########################################################################
        ## post-stage output files
        ##        
        def poststage(job):
                
            try:
                stdout = saga.file.file(saga.url(job.backend.remote_filesystem+job.backend.saga_job_out));
                stdout.copy("file://localhost/"+job.getOutputWorkspace().getPath()+"stdout");
    
                stderr = saga.file.file(saga.url(job.backend.remote_filesystem+job.backend.saga_job_err));
                stderr.copy("file://localhost/"+job.getOutputWorkspace().getPath()+"stderr");
        
            except saga.exception, e:
                logger.error('exception caught while poststaging: %s', 
                             e.get_full_message())
            return 0
        ##
        ##########################################################################

    
        ###################################################
        ## Translates a saga job state to a ganga job state
        ##
        def sagatogangastatus(sagastatus, job):
                
            if sagastatus == saga.job.Done:
                job.updateStatus("completing")
                poststage(job)
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
                js_url = saga.url(j.backend.remote_job_service)
                js = saga.job.service(js_url)
                saga_job = js.get_job(j.backend.saga_job_id)
            
                # query saga job state
                job_state = saga_job.get_state()
            
                # translate saga job state to ganga job state
                j.updateStatus(sagatogangastatus(job_state, j))
                
            except saga.exception, e:
                logger.error('exception caught while updating job: %s', 
                             e.get_full_message())
    
    ##########################################################################
    ## Make the monitoring function available to the update thread
    ##         
    updateMonitoringInformation = staticmethod(updateMonitoringInformation)

                        
            








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
      #job = self.getJobObject()
      #topdir = self.workdir.rstrip( os.sep )
      #path = os.path.join( topdir, filename ).rstrip( os.sep )
      #job.viewFile( path = path, command = command )
      return None
      

#    def remove_workdir(self):
#        if config['remove_workdir']:
#            import shutil
#            try:
#                shutil.rmtree(self.workdir)
#            except OSError,x:
#                logger.warning('problem removing the workdir %s: %s',str(self.id),str(x))                        
    
