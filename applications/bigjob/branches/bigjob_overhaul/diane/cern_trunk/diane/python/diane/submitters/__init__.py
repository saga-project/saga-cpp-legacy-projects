
# this is the interface used by ganga-based submitter scripts
# standard submitter scripts are defined in this package
# other submitter scripts should be put in PYTHONPATH

import sys
import os
import os.path

from diane.diane_exceptions import DianeException
import diane.util

from diane.logger import getLogger,logging
logger = getLogger('Submitter')

sh_download_wrapper = '''#!/bin/sh

wget %(DIANE_DOWNLOAD_URL)s/diane-install
python ./diane-install --download-url=%(DIANE_DOWNLOAD_URL)s --prefix=$PWD/diane %(DIANE_VERSION)s
$($PWD/diane/install/%(DIANE_VERSION)s/bin/diane-env)
%(DIANE_WORKER_START)s
'''

sh_shared_setup_wrapper = '''#!/bin/sh
$(%(DIANE_TOP)s/bin/diane-env)
%(DIANE_WORKER_START)s
'''

sh_shared_platform_doctor_wrapper = """#!/bin/sh

python %(DIANE_TOP)s/etc/platform-doctor.py --force=DUMMY_PLATFORM_STRING

# force using the newly compiled platform
export DIANE_PLATFORM=DUMMY_PLATFORM_STRING
$(%(DIANE_TOP)s/bin/diane-env)
%(DIANE_WORKER_START)s

"""

sh_download_platform_doctor_wrapper = """#!/bin/sh

wget %(DIANE_DOWNLOAD_URL)s/diane-install
python ./diane-install --download-url=%(DIANE_DOWNLOAD_URL)s --prefix=$PWD/diane %(DIANE_VERSION)s
$($PWD/diane/install/%(DIANE_VERSION)s/bin/diane-env)


python $PWD/diane/install/%(DIANE_VERSION)s/etc/platform-doctor.py --force=DUMMY_PLATFORM_STRING

# force using the newly compiled platform
export DIANE_PLATFORM=DUMMY_PLATFORM_STRING
cp $PWD/diane/packages/*${DIANE_PLATFORM}* .

%(DIANE_WORKER_START)s

"""



import tempfile

class Submitter:
    def __init__(self):
        from optparse import OptionParser
        self.parser = OptionParser(description="DIANE Worker Agent Submitter starts N worker agents. The worker agents are configured according to the specification in a run file and connect the specified run master or a directory service.")
        self.parser.add_option("--diane-run-file",dest="RUNFILE",default=None, help="optional file containing the run description and submission customization parameters (see diane-run command)")
        self.parser.add_option("--diane-master",dest="MASTERLOC",default='workspace:auto', help="the run master identifier. With MASTERLOC=workspace:auto (default) the workers will connect to the last master (run) defined in the local DIANE_USER_WORKSPACE. Use arbitrary file: MASTERLOC=file:path/to/MasterOID (the file: prefix may be skipped). Use the given run number: MASTERLOC=workspace:N")
        self.parser.add_option("--diane-worker-number",dest="N_WORKERS",type="int",default=1,help="number of worker agents to be submitted (default 1)")

        self.parser.add_option("--diane-platform-doctor",dest="ENABLE_DOCTOR",default=False,help="automatically compile external packages if needed and provide assistance with solving problems related to omniORB binary compatibility")

        self.parser.add_option("--diane-download-url",dest="DOWNLOAD_URL", default="http://cern.ch/diane/packages", help="download URL for DIANE itself")
        # the download parameter decides whether or not the diane installation should be downloaded locally on the worker agent node
        # this parameter should be set by the constructor of the derived class
        self.download = False
        # the wrapper shell text, this parameter may be set from the run file callback hooks
        self.wrapper = None
        
    def initialize(self):
        logger.user('*'*20 + ' DIANE Worker Agent Submitter '+'*'*20)
        options,args = self.parser.parse_args()
        self.options = options
        self.args = args
        self.master_oid_filename = None
        
        def require(opt):
            if getattr(options,opt) is None:
                self.parser.print_help()
                logger.error('ERROR: %s not specified'%opt)
                sys.exit(-1)

        #for opt in ['MASTERLOC','N_WORKERS']:
        #    require(opt)

        import urlparse
        scheme, netloc, path = urlparse.urlparse(self.options.MASTERLOC)[:3]

        # netloc not supported
        if netloc:
            logger.error('ERROR: wrong format of master location specification: %s - network location (%s) not supported'%(self.options.MASTERLOC,netloc))
            sys.exit(-1)

        # defaut scheme
        if not scheme:
            scheme = 'file'

        if not scheme in ['workspace','file']: #TODO: 'corbaloc','IOR'
            logger.error('ERROR: unknown scheme (%s) of master location specification'%scheme)
            sys.exit(-1)

        import diane.workspace # this updates the sys.path to DIANE_USER_WORKSPACE/apps

        if scheme == 'workspace':
            if path == 'auto' or not path:
                runid = None
            else:
                runid = int(path)
            self.master_oid_filename = diane.workspace.getRundir(path='MasterOID',runid=runid)

        if scheme == 'file':
            if not path:
                logger.error('ERROR: wrong format of master location: filename not specified ')
                sys.exit(-1)
            self.master_oid_filename = path

## TODO:
##         if scheme in ['IOR','corbaloc']:
##             print scheme,path
##             import tempfile
##             self.master_oid_file = tempfile.NamedTemporaryFile(prefix='MasterOID')
##             self.master_oid_file.write(path)
##             self.master_oid_file.flush()
##             self.master_oid_filename = self.master_oid_file.name()

        logger.user('the workers will connect to the master specified by %s'%self.master_oid_filename)
        
        import diane.rundef        
        self.runfile = diane.rundef.Runfile(options.RUNFILE)

        from diane.util.omniorb_setup import make_omniorb_config
        #self.config_file = file('worker_config.cfg','w')
        self.config_file = tempfile.NamedTemporaryFile(mode="w",prefix='diane_worker_config-')
        self.enable_GSI=make_omniorb_config(self.runfile,'worker',self.config_file)
        self.config_file.flush()
        
        try:
            self.runfile.call('initialize_submitter',self)
        except DianeException:
            pass
        
        if self.wrapper is None:
            if self.download:
                if self.options.ENABLE_DOCTOR:
                    self.wrapper = sh_download_platform_doctor_wrapper
                else:
                    self.wrapper = sh_download_wrapper
            else:
                if self.options.ENABLE_DOCTOR:
                    self.wrapper = sh_shared_platform_doctor_wrapper
                else:
                    self.wrapper = sh_shared_setup_wrapper

        self.expandable_wrapper_variables = {'DIANE_TOP': diane.PACKAGE.getReleaseDir(),
                                       'DIANE_VERSION': diane.PACKAGE.getVersion(),
                                       'DIANE_WORKER_START': 'diane-worker-start $*',
                                       'DIANE_DOWNLOAD_URL': self.options.DOWNLOAD_URL}

        import diane.PACKAGE
        self.wrapper = self.wrapper % self.expandable_wrapper_variables

        # make sure that Executable does not have "Hello World" as default arguments
        # this will not be necessary if  DIANEWorkerAgent application handler is introduced
        # FIXME: bug in ganga 5-0-0-beta9!
        #import Ganga.GPI
        #Ganga.GPI.config.defaults_Executable.args = []
        # workaround:
        import Ganga.Utility.Config
        c = Ganga.Utility.Config.getConfig('defaults_Executable')
        c.options['args'].session_value = []

        #FIXME: bug in ganga-5-0-0
        #workaround: override _checkset_status method which causes an error
        import Ganga
        Ganga.GPIDev.Lib.Job.Job._checkset_status = lambda x, y : None


    def submit_worker(self,j):
        import Ganga.GPI
        """This function uses Ganga.GPI to submit the job created by the submitter script."""
        j.application = Ganga.GPI.Executable() #PENDING: FIXME: make DIANEWorkerAgent application handler
        wf = tempfile.NamedTemporaryFile(mode="w",prefix='diane_worker_wrapper-')
        wf.write(self.wrapper)
        wf.flush()
        uuid_path = tempfile.mkdtemp()
        uuid = open(os.path.join(uuid_path, 'ganga_job_uuid'), 'w')
        uuid.write(str(j.info.uuid))
        uuid.flush()
        uuid.close()
        diane.util.chmod_executable(wf.name)
        j.application.exe = Ganga.GPI.File(wf.name)
        j.application.args+=['--ior-file=MasterOID']
        if self.enable_GSI:
            j.application.args+=['--enable-GSI']

        j.application.args+=['--omniorb-config-file=%s'%os.path.basename(self.config_file.name)]

        from Ganga.Utility.util import hostname

        ganga_config = Ganga.GPI.config
        
        # works only for local repository type
        j.application.env = {'GANGA_JOB_ID': "%s@%s:%s:%d"%(ganga_config.Configuration.user,ganga_config.System.GANGA_HOSTNAME, ganga_config.Configuration.gangadir, j.id),
                             'GANGA_JOB_UUID': str(j.info.uuid) }
        
        j.inputsandbox = j.inputsandbox + [Ganga.GPI.File(self.master_oid_filename),
                                           Ganga.GPI.File(self.config_file.name),
                                           Ganga.GPI.File(uuid.name)]
        j.name = "DIANEWorkerAgent"

        j.outputsandbox = j.outputsandbox + ['worker.log']

        if self.options.ENABLE_DOCTOR:
            j.outputsandbox = j.outputsandbox + ['*DUMMY_PLATFORM_STRING*']

        try:
            self.runfile.call('worker_submit',j)
        except DianeException:
            pass
        
        logger.user('Submitting DIANE worker agent to the %s backend, Ganga job id=%s ',repr(Ganga.GPI.typename(j.backend)),j.id)
        j.submit()
        
        # cleanup
        #os.removedirs(uuid_path)
