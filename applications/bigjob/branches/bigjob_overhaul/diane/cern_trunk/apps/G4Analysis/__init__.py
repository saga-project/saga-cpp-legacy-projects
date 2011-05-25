from G4Analysis import Worker, ApplicationManager

# uncomment this line if you want to enable the shared library mode
#from G4AnalysisSharedLibWorker import SharedLibWorker, setup_application

from Data import *

from diane import SimpleTaskScheduler
import diane
logger = diane.getLogger('G4Analysis')


def run(input,config):    
    input.worker = Worker
    input.manager = ApplicationManager
    input.scheduler = SimpleTaskScheduler

    ####################
    #shared library mode
    #config.WorkerAgent.APPLICATION_SHELL='sh' # run via a separate shell process
    #input.worker = SharedLibWorker
    ####################

# clean and tar up the application directory
def run_post(input,config):
    import os    
    appname = input.data.workerInit.G4ApplicationName

    logger.info('simulation module: %s',appname)
    
    try:
        appmod = __import__(appname)
    except ImportError,x:
        raise diane.diane_exceptions.DianeException('cannot find the Geant4 application package %s (ImportError: %s)'%(appname,x))

    appdir = os.path.dirname(appmod.__file__)

    logger.info('cleaning up the application directory %s',appdir)
    logger.info('the output stored in %s',os.path.abspath('clean.out'))
    
    cmd = '(cd %s && source setup.sh && gmake clean) 2>&1 > clean.out' %appdir
    
    r=os.system(cmd)
    if r != 0:
        raise diane.diane_exceptions.DianeException('failed to run command (exit code = %s): %s'%(r,cmd))

    logger.info('tarring the application directory %s',appdir)
    import tarfile
    tf = tarfile.TarFile('_%s.tar'%appname,'w')
    tf.add(appdir,appname,recursive=True)
    tf.close()
    
    
## ---- OLD STUFF ----
## import os

## CERN_AFS_FIX = True

## class BOOT:
##     def preconfig(self, backend,spec):
##         print '*' *30, 'backend=', backend
##         print '*' *30, 'spec=',spec
##         import pickle
##         data = pickle.loads(spec.JobInitData)
##         text = ''
##         if backend.lower() == 'cluster':
##             from  DIANE.util import importModule
##             cmod = importModule(data.workerInit.G4ApplicationName)
##             return """
##             pushd %s
##             eval `diane.app.setup --eval=csh`
##             popd
##             """ % (os.path.dirname(cmod.__file__),)
##         if not CERN_AFS_FIX:
##             return ""
##         else:
##             return """setenv DIANE_USER_WORKSPACE /afs/cern.ch/sw/arda/install/DIANE/Geant4/G4DIANEworkspace
## which python
## ls -l `dirname python`
## """

##     def postconfig(self,backend,spec):
##         if not CERN_AFS_FIX:
##             return ""
##         else:
##             return """

## # FIXME:
## setenv VO_GEANT4_SW_DIR /afs/cern.ch/project/gd/apps/geant4/dirInstallations

## echo before sh

## bash <<\\EOF
## echo this is bash

## pushd ${VO_GEANT4_SW_DIR}
## source setup.sh
## popd

## # setup script broken
## export G4LEDATA=${G4INSTALL}/data/G4EMLOW
## export G4RADIOACTIVEDATA=$G4INSTALL/data/RadioactiveDecay
## """

##     def postworker(self,backend,spec):
##         if not CERN_AFS_FIX:
##             return ""
##         else:
##             return '\\EOF\n'
        
## boot =BOOT()

