import os
import glob
import shutil
import time

def chmod_executable(path):
    "make a file executable"
    import stat,os
    os.chmod(path,stat.S_IXUSR|os.stat(path).st_mode)

from diane import IApplicationWorker, SimpleApplicationManager, SimpleTaskScheduler,IApplicationManager
from diane.BaseThread import BaseThread

from diane import getLogger
logger = getLogger('THISApplication')

#FIXME: hardcoded for the time being
EVENT_BUNCH  = 100000
TOTAL_EVENTS = 20000000

# basedir contains:
# input/* - all files to be copied to the WN
# output/* - output files received from WN
# rndm/* - random seeds (one file per task sequence)

class RunData:
    def __init__(self):
        self.basedir = None
        self.executable = 'this' # name of an executable file
        self.rundir = 'this/Thorax' # run directory on the worker node (containing executable and mac directory)
        self.args = ['mac/Thorax-segRe-6-RV.mac',
                     "/random/resetEngineFrom rndm/status%(SEED)s.rndm",
                     "/this/geometry/DS/save output/Thorax-segRe-6-RV/output-%(SEED)s.hdr",
                     "/this/geometry/TSS/save output/Thorax-segRe-6-RV/output-%(SEED)s-stat.txt",
                     "/run/initialize",
                     "/control/execute mac/beam.mac",
                     "/run/beamOn 2000000"]
        
class WorkerData:
    def __init__(self):
        self.input_files = None
        self.executable = None
        
class TaskInput:
    def __init__(self):
        self.seed_file = None
        self.rundir = None
        self.args = None
        
class TaskOutput:
    def __init__(self):
        pass



class THISApplicationManager(SimpleApplicationManager, BaseThread):
    def __init__(self,name=None,auto_register=True):
       BaseThread.__init__(self,name=name)
       SimpleApplicationManager.__init__(self)
    
    def list_files(self,pattern,dir=''):
        return [os.path.join(dir,os.path.basename(f)) for f in glob.glob(os.path.join(self.basedir,dir,pattern))]

    def initialize(self, run_data):
        self.basedir = run_data.basedir

        logger.info('THIS: run basedir: %s',run_data.basedir)

        import glob
        w = WorkerData()
        w.input_files = self.list_files('*',dir='input')
        w.executable = run_data.executable
        w.rundir = run_data.rundir
        w.args = run_data.args
        self.worker_init = w
        self.seed_files = self.list_files('rndm/*')
        logger.info('THIS:scan list of random seeds: %d found'%len(self.seed_files))

        tasks = []

        for f in self.seed_files:
            t = self._task()
            t.task_input = TaskInput()
            t.task_input.seed_file = f
            tasks.append(t)

        # keeping track many tasks have been done
        #stop condition on the nb of events, not the tasks
        #self.N = len(tasks)
        self.N=TOTAL_EVENTS
        self.done_counter = 0

        return tasks

    def run(self):
        while self.scheduler.has_more_work():
                new_seed_files = self.list_files('new_rndm/*')
                for f in new_seed_files: # just the newly added files
     #print "***********************************the new seedfiles are "
     #os.remove(f)
                   # code similar to initialize() method
     os.system('cp '+os.path.join(self.basedir,'new_rndm',f)+' rndm/'+f)
     os.system('rm '+os.path.join(self.basedir,'new_rndm',f))
                   t = self._task()
                   t.task_input = TaskInput()
                   t.task_input.seed_file = f
                   # inform scheduler that there are new tasks
                   self.scheduler.todo_tasks.put(t)
                   # update my local task counter
                   # self.N += 1
  time.sleep(5)
         
  #counting the nb of events done
  ev=0
  import diane.workspace
         for f in glob.glob(os.path.join('./','*.cnt')):
          try:
    ev=ev+eval(open(f,"rb").read())
   except:
           print 'IOerror when reading cnt file ', f
    time.sleep(1)
    try:
                                 ev=ev+eval(open(f,"rb").read())
                         except:
                                 print '2nd IOerror when reading cnt file ', f

  open(os.path.join('./',"sum.txt"), "wb").write(str(ev))
         self.done_counter = ev
        
    def tasks_done(self, tasks):
        #stop condition on the nb of events, not the tasks
        #self.done_counter += len(tasks)
 #not really used 
        s=0
 import diane.workspace

 #open(os.path.join('./',"sum1.txt"), "wb").write(str(s))
        #self.done_counter = s

    def has_more_work(self):
 #stop condition on the nb of events
        #return self.done_counter < self.N
        return self.done_counter < TOTAL_EVENTS

class THISFailure(Exception):
    pass

def untar(fn):
    if os.system('tar xfz %s'%fn):
        raise THISFailure('cannot untar %s'%fn)

def tar(tf,fn):
    if os.system('tar cfz %s %s'%(tf,fn)):
        raise THISFailure('cannot tar %s %s'%(tf,fn))

class THISWorker(IApplicationWorker):
    def initialize(self, worker_data):

        # allow to overwrite the output files
        from diane.FileTransfer import FileTransferOptions
        self.file_transfer = self._agent.ftc
        self.allow_overwrite = FileTransferOptions(overwrite=True)
        
        #telechargement de this
        for fn in worker_data.input_files:
            self.file_transfer.download(os.path.basename(fn),fn)

        self.worker_data = worker_data

        for fn in glob.glob('*.tar.gz'):
            untar(fn)

        os.environ['G4LEDATA'] = os.path.join(os.getcwd(),'G4EMLOW3.0')

        for fn in glob.glob('*.mac'):
            logger.info('cp %s %s',fn,os.path.join(self.worker_data.rundir,'mac'))
            shutil.copy(fn,os.path.join(self.worker_data.rundir,'mac'))

        shutil.copy('stopOnScript.sh',self.worker_data.rundir)

        chmod_executable(os.path.join(self.worker_data.rundir,self.worker_data.executable))

        os.chdir(self.worker_data.rundir)
 

    def do_work(self, task_input):
 
 #sorina logging time
 start_time=time.time()
 logger.info('starting time is %f ',start_time)
        
 self.file_transfer.download(task_input.seed_file,os.path.join('rndm',task_input.seed_file))

        import re
        r = re.compile('status(\d+)[.]rndm').match(task_input.seed_file)

        if not r:
            raise THISFailure('wrong format of seed file: %s'%task_input.seed_file)

        seed = r.group(1)
        SEED_pattern = {'SEED':'%0.4d'%int(seed)}
        # start execution

        args = ['./'+self.worker_data.executable]+[a%SEED_pattern for a in self.worker_data.args]

        print '**** cwd=',os.getcwd()
        print '**** command=',args
        
        import subprocess
        
        this_process = subprocess.Popen(args)

        return_code = this_process.poll()

        event_counter = 0
        
        # while the simulation runs, wait for the updates of the result files
        while return_code is None:
            if os.path.exists('OUTPUT_READY'):
                tf = 'output-%(SEED)s.tgz'%SEED_pattern
                tar(tf,'output')
                self.file_transfer.upload(tf,opts=self.allow_overwrite)
                event_counter += EVENT_BUNCH
                cf = file('event-%(SEED)s.cnt'%SEED_pattern,'w')
                cf.write(str(event_counter))
                cf.close()
                self.file_transfer.upload(cf.name,opts=self.allow_overwrite)
                os.remove('OUTPUT_READY')
            return_code = this_process.poll()
            time.sleep(1)

        # the simulation finished
        # FIXME: can we rely on the return code to signal application error?
  
 #logging time
        end_time=time.time()
 logger.info('starting time is %f ',start_time)
        logger.info('ending time is %f ',end_time)
 execution_time=time.time()       
        logger.info('execution time is %f ',execution_time)
 
    def finalize(self,x):
        pass

def run(input,config):
     input.scheduler = SimpleTaskScheduler
     input.scheduler.policy.LOST_TASK_MAX_ASSIGN = 1
     input.scheduler.policy.FAILED_TASK_MAX_ASSIGN = 1
     input.manager = THISApplicationManager
     input.worker = THISWorker
     input.data = RunData()
     #config.WorkerAgent.inprocess_proxy = True
     config.WorkerAgent.HEARTBEAT_DELAY = 900
     config.WorkerAgent.HEARTBEAT_TIMEOUT = 30
     config.WorkerAgent.PULL_REQUEST_DELAY = 10
     config.RunMaster.LOST_WORKER_TIMEOUT = 1200
     config.RunMaster.IDLE_WORKER_TIMEOUT = 1200
