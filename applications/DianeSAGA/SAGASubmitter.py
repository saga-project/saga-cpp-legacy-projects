#!/usr/bin/env ganga
#-*-python-*-

from diane.submitters import Submitter

import tempfile
import os
import diane
import saga


class SAGASubmitter(Submitter):

    def submit_worker(self,j):
        import Ganga.GPI
        """This function uses Ganga.GPI to submit the job created by the submitter script."""
        j.application = Ganga.GPI.Executable() 
        wf = tempfile.NamedTemporaryFile(mode="w",prefix='diane_worker_wrapper-')
        
        # if diane dir is not explicitly given, default to 
        # the master wrapper
        if(self.options.dianedir == ''):
            wf.write(self.wrapper)

        # if it is given, use a modified wrapper script
        else:
            sh_run_wrapper = '''#!/bin/sh
                                $('''+self.options.dianedir+'''/bin/diane-env)
                                diane-worker-start $*
                             '''
 
            wf.write(sh_run_wrapper)

        wf.flush()
        uuid_path = tempfile.mkdtemp()
        uuid = open(os.path.join(uuid_path, 'ganga_job_uuid'), 'w')
        uuid.write(str(j.info.uuid))
        uuid.flush()
        uuid.close()
        diane.util.chmod_executable(wf.name)
        j.application.exe = "/bin/sh" #Ganga.GPI.File(wf.name)
        j.application.args+=[Ganga.GPI.File(wf.name)]
        j.application.args+=['--ior-file=MasterOID']
        if self.enable_GSI:
            j.application.args+=['--enable-GSI']

        j.application.args+=['--omniorb-config-file=%s'%os.path.basename(self.config_file.name)]

        # Point the worker agent to the working directory
        workdir = saga.url(j.backend.filesystem_url).path+"/"
        j.application.args += ['--workdir=%s'%workdir]


        from Ganga.Utility.util import hostname

        ganga_config = Ganga.GPI.config
        
        # works only for local repository type
        j.application.env = {'GANGA_JOB_ID': "%s@%s:%s:%d"%(ganga_config.Configuration.user,ganga_config.System.GANGA_HOSTNAME, ganga_config.Configuration.gangadir, j.id),
                             'GANGA_JOB_UUID': str(j.info.uuid)}
        
        j.inputsandbox = j.inputsandbox + [Ganga.GPI.File(self.master_oid_filename),
                                           Ganga.GPI.File(self.config_file.name),
                                           Ganga.GPI.File(uuid.name)]
        j.name = "SAGA/DIANEWorkerAgent"

        if self.options.ENABLE_DOCTOR:
            j.outputsandbox = j.outputsandbox + ['*DUMMY_PLATFORM_STRING*']

        #try:
            self.runfile.call('worker_submit',j)
        #except DianeException:
        #    pass
        
        j.submit()


prog = SAGASubmitter()

prog.parser.description = "Submit worker using the GangaSAGA backend. "+prog.parser.description
prog.parser.add_option("--dianedir",       dest="dianedir",        type="string", default='',help="Diane root directory on the worker machine")
prog.parser.add_option("--jobservice_url", dest="saga_job_service",type="string", default='',help="SAGA Job Service Endpoint URL")
prog.parser.add_option("--filesystem_url", dest="saga_filesystem", type="string", default='',help="SAGA File System Endpoint URL")
prog.parser.add_option("--queue",          dest="saga_queue",      type="string", default='',help="Queue name that should be used for submission")
prog.parser.add_option("--allocation",     dest="saga_allocation", type="string", default='',help="Allocation that should be used for accounting")

prog.initialize()


for i in range(prog.options.N_WORKERS):
 
    j = Job()
    j.backend=SAGA()

    if prog.options.saga_job_service != '':
        j.backend.jobservice_url = prog.options.saga_job_service

    if prog.options.saga_filesystem != '':
        j.backend.filesystem_url = prog.options.saga_filesystem  

    if prog.options.saga_queue != '':
        j.backend.queue = prog.options.saga_queue 

    if prog.options.saga_allocation != '':
        j.backend.allocation = prog.options.saga_allocation 

    prog.submit_worker(j)

