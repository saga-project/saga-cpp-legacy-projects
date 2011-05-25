#####################################################################
#
#
# APPLICATION ADAPTER FOR GEANT4 PRODUCTION:
#
# This is the main entry point for DIANE G4Production application
#
# Modified 01/2007
#
#####################################################################

import os

def chmod_executable(path):
    "make a file executable"
    import stat,os
    os.chmod(path,stat.S_IXUSR|os.stat(path).st_mode)

from diane import IApplicationWorker, SimpleApplicationManager, SimpleTaskScheduler

from diane import getLogger
logger = getLogger('CrashApplication')

class G4ProdApplicationManager(SimpleApplicationManager):
    def initialize(self, job_data):

        ###################
        # Creating Plan:
        #  Input:
        #   job_data: Includes dictionary 'JobInitData' specified in .job file, with 
        #             the keys {'exedir','tarname'}, where:
        #
        #    exedir:   directory containing the following files:
        #                - geant4_executables*.py -- executable files
        #                (one file corresponds to one task)
        #                the parent directory must contain:
        #                - geant4_production.sh -- the driver (script running the executable)
        #    tarname:  tar-ball containing current Geant4 candidate version
        #
        #
        #
        # Variable description: 
        #  exedir:  directory containing the scripts geant4_executableXXX..XX.py,
        #          where the parent directory must contain driver geant4_production.sh
        #  tarname: name of tar-ball containing Geant4 candidate version
        #  exlist:  list of all geant4_executableXXX..XX.py
        #  taskNo:  number of found scripts geant4_executableXXX..XX.py
        #  driver:  content of geant4_production.sh (driver script)
        #
        #
        #  IN LOOP:
        #  exf:     content of current script geant4_executableXXX..XX.py in loop
        #  exe:     name of current geant4_executableXXX..XX.py in loop
        # 
        #  plan:    list containing dictionaries, with each dictionary containing
        #           the keys {'exe','exename','outputname'}, where: 
        #             exe:        content of particular geant4_executableXXX..XX.py
        #             exename:    name of particular geant4_executableXXX..XX.py
        #             outputname: name of tar-ball, containing simulation output
        #
        ###################
        
        exedir,tarname,tarloc = job_data['exedir'],job_data['candname'],job_data['candurl']
        driverpath = '%(driverdir)s/%(drivername)s'%job_data
        
        ### Retrieving list of executables
        import glob
        exlist = glob.glob('%(exedir)s/geant4_executable*.py'%job_data)

        taskNo = len(exlist)
        
        logger.info('G4-Appl-Adapter: Found %d executable files in %s',taskNo,exedir)
        logger.info('G4-Appl-Adapter: Using G4 candidate version contained in %s',tarname)
        
        if taskNo == 0:
            raise ValueError("G4-Appl-Adapter: ERROR: No executables found")

        ### Reading in driver script
        if os.path.exists(driverpath):
            logger.info('G4-Appl-Adapter: Reading in driver %s',driverpath)
            driver = file(driverpath).read()
        else:
            raise ValueError("G4-Appl-Adapter: ERROR: Driver not found: %s"%driverpath)

        ### Reading in other required files
        reqfiles = {}
        for myfname in job_data['filelist']:
            myfpath = '%s/%s'%(job_data['filedir'],os.path.basename(myfname))
            if os.path.exists(myfpath):
                logger.info('G4-Appl-Adapter: Reading in file %s',myfpath)
                reqfiles[myfname] = file(myfpath).read()
            else:
                raise ValueError("G4-Appl-Adapter: ERROR: File not found: %s"%myfpath)

        ### Reading in executables and filling up plan list
        tasks = []
        import re
        outpat = re.compile(r'^#OUTPUT_FILE (?P<outputname>\S+)',re.M)
        for exe in exlist:
            exf = file(exe).read()

            m = outpat.search(exf)

            if not m:
                raise ValueError('G4-Appl-Adapter: ERROR: cannot find the #OUTPUT_FILE tag in '+exe)              

            t = self._task()
            t.task_input = {'exe':exf,'exename':os.path.basename(exe),'outputname':m.group('outputname')}
            tasks.append(t)
            logger.info('task %s %s',t.task_input['exename'],t.task_input['outputname'])

        self.worker_init = {'driver':driver,'files':reqfiles,'tarname':os.path.basename(tarname),'tarloc':tarloc}

        # keeping track many tasks have been done
        self.N = len(tasks)
        self.done_counter = 0
        
        return tasks
        
    def tasks_done(self, tasks):

        ###################
        # Integration of task output.
        #  Input:
        #   output_data: Dictionary 'out' returned by "Worker.env_performWork"
        #
        #  Steps:
        #    1) Writing output tar-ball stream to file in designated directory
        #    2) Writing geant4_production log stream to file in designated directory
        #
        ###################

        for t in tasks:
            if t.status == t.IGNORED:
                logger.info('** ignored task: %d',t.tid)
            else:
                output_data = t.task_output
                logger.info('G4-Appl-Adapter: Saving output tar-ball %s ', output_data['name'])
                file(output_data['name'],'w').write(output_data['file'])
        
                logger.info('G4-Appl-Adapter: Saving log-file %s.log',output_data['name'])
                file(output_data['name']+'.log','w').write(output_data['outputlog'])

        self.done_counter += len(tasks)

    def has_more_work(self):
        return self.done_counter < self.N

class G4ProdFailure(Exception):
    pass

class G4ProdWorker(IApplicationWorker):
    def initialize(self, init_data):

        ###################
        # Initializing procedure for worker node (once per worker node).
        #  Input:
        #   init_data: First tuple entry (which is a dictionary) returned by "Planner.env_createPlan"
        #
        #  Steps:
        #    1) Writing geant4_production.sh stream to file
        #    2) Changing permissions of geant4_production.sh for execution
        #    3) Fetching tar-ball containing Geant4 candidate version with wget
        #    4) Extracting candidate version
        #
        ###################

        self.exename = ''
        
        logger.info('G4-Appl-Adapter: Writing driver script to file geant4_production.sh')
        file('geant4_production.sh','w').write(init_data['driver'])
        chmod_executable('geant4_production.sh')

        myfiles = init_data['files']
        for myfile in myfiles.keys():
            logger.info('G4-Appl-Adapter: Writing file %s',myfile)
            file(myfile,'w').write(myfiles[myfile])

        g4candtar = init_data['tarname']
        g4tarloc  = init_data['tarloc']
        logger.info('G4-Appl-Adapter: Fetching tarball %s containing G4 candidate',g4candtar)
        rc_wget = os.system('wget  %s/%s'%(g4tarloc,g4candtar))

        logger.info('G4-Appl-Adapter: Extracting %s',g4candtar)
        rc_tar  = os.system('tar xfzv %s'%g4candtar)
        
        if rc_wget == 0 and rc_tar == 0: 
            logger.info('G4-Appl-Adapter: Initialization of worker successful')
            return 1
        else:
            logger.error('G4-Appl-Adapter: Initialization of worker failed')
            return 0



    def do_work(self, input_data):

        ###################
        # Single task on worder node.
        #  Input:
        #   input_data: One entry (which is a dictionary) of list 'plan' created in "Planner.env_createPlan",
        #               where 'plan' is the second tuple entry returned by "Planner.env_createPlan"
        #
        #  Steps:
        #   1) Writing current geant4_executableXXX..XX.py stream to file  
        #   2) Removing old file g4output.text containing messages of geant4_production.sh
        #   3) Invoking script geant4_production.sh (creates tar-ball with simulation output)
        #   4) Creating dictionary 'out' containing tar-ball name, tar-ball content and
        #      content of g4output.text 
        # 
        #   Important: If return value of geant4_production.sh is non-zero, env_performWork
        #              returns no output and induces the termination of the worker!
        #              Conversely, if geant4_production.sh returns 0 (and rm -f g4output.text was
        #              also successful), the dictionary 'out' is returned and subsequently integrated
        #              (provided that the expected output tarball is found).
        #
        #
        ###################
        
        file(input_data['exename'],'w').write(input_data['exe'])

        # geant4_production.sh requires following arguments: 
        #          <geant4_executableXXX..XX.py>
        #          <output tar-ball> 
        # ATTENTION: Flags work only with new version of geant4_production.sh
        g4outfile = 'g4output.text'
        g4exe,g4tar = input_data['exename'],input_data['outputname']
        
        g4cmd = 'rm -f %s && ./geant4_production.sh %s %s > g4output.text 2>&1' %(g4outfile,g4exe,g4tar)
        rc = os.system(g4cmd)

        if rc == 0: 
            logger.info('G4-Appl-Adapter: Task successful')
        else:
            logger.error('G4-Appl-Adapter: Task failed')

        out = {'name':g4tar, 'file':None, 'outputlog' : file(g4outfile,'r').read() }

        if os.path.exists(g4exe):
            os.remove(g4exe)

        if os.path.exists(g4outfile):
            os.remove(g4outfile)            
       
        if rc == 0:
            if os.path.exists(g4tar):
                out['file'] = file(g4tar).read()
                os.remove(g4tar)
                return out
            else:
                msg = 'output file not produced despite exit code == 0'
                logger.error(msg)
                raise G4ProdFailure(msg)
        else:
            if os.path.exists(g4tar):
                os.remove(g4tar)
            msg = 'exit code: %d'%rc
            logger.error(msg)
            return G4ProdFailure(msg)
        
    def finalize(self,x):
        pass
  
def run(input,config):
     input.scheduler = SimpleTaskScheduler
     input.manager = G4ProdApplicationManager
     input.worker = G4ProdWorker
