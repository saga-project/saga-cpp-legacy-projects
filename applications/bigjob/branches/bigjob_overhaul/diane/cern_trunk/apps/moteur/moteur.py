import os
import glob
import shutil
import time

import moteur as application

from diane import IApplicationWorker, SimpleApplicationManager, SimpleTaskScheduler,IApplicationManager
from diane.BaseThread import BaseThread

class MoteurTaskScheduler(SimpleTaskScheduler):
    def tasks_failed(self, tasks):
        for f in tasks:
            print "Task ",f.task_input.exe," failed: moving it to directory 'failed'"
            shutil.move(f.task_input.exe,'failed')
            #in case the task has been put in the running dir
            os.remove(os.path.join('running',os.path.basename(f.task_input.exe)))
            
def chmod_executable(path):
    "make a file executable"
    import stat,os
    os.chmod(path,stat.S_IXUSR|os.stat(path).st_mode)

class MoteurFailure(Exception):
    pass

class TaskInput:
    def __init__(self):
        self.exe = None
        self.return_code = None

class RunData:
    def __init__(self):
        self.basedir = '.'

class WorkerData:
    def __init__(self):
        self.basedir = None

class MoteurApplicationManager(SimpleApplicationManager, BaseThread):
    def __init__(self,name=None,auto_register=True):
        self.basedir="."
        BaseThread.__init__(self,name=name)
        SimpleApplicationManager.__init__(self)
    
    def list_files(self,pattern,dir=''):
        return [os.path.join(dir,os.path.basename(f)) for f in glob.glob(os.path.join(self.basedir,dir,pattern))]

    def initialize(self, run_data):
        print "Initializing Moteur Application Manager"
        w = WorkerData()
        w.basedir = self.basedir
        self.worker_init = w
        #create dirs and clean up garbage
        for d in ['submitted','scheduled','running','done','failed']:
            if not os.access(os.path.join(self.basedir,d),os.F_OK):
               print "Creating directory ",os.path.join(self.basedir,d)
               os.mkdir(os.path.join(self.basedir,d))
            if d!='submitted':
                old_jobs = self.list_files(d+'/*')
                for f in old_jobs:
                    print "Removing file ",f," from directory ",d
                    os.remove(os.path.join(self.basedir,d,f))
        return []
    
    def run(self):
         while self.scheduler.has_more_work():
             new_jobs = self.list_files('submitted/*')
             for f in new_jobs: 
                 print "Creating task for script ",f
                 shutil.move(os.path.join(self.basedir,'submitted',f),os.path.join(self.basedir,'scheduled'))
                 #creates the task
                 d=self._task()
                 d.task_input = TaskInput()
                 d.task_input.exe = 'scheduled/'+f
                 # inform scheduler that there are new tasks
                 self.scheduler.todo_tasks.put(d)
             time.sleep(2)
        
    def tasks_done(self, tasks):
        for f in tasks:
            print "Task ",f.task_input.exe," is done: moving it to directory 'done'"
            shutil.move(f.task_input.exe,'done')
            #in case the task has been put in the running dir
            os.remove(os.path.join('running',os.path.basename(f.task_input.exe)))

    def has_more_work(self):
         return 1
     
class MoteurWorker(IApplicationWorker):
    def initialize(self,worker_data):
        print "Initialize worker"
        self.worker_data = worker_data
        return None

    def do_work(self,task_input):
        # allow to overwrite the output files
        from diane.FileTransfer import FileTransferOptions
        self.file_transfer = self._agent.ftc
        self.allow_overwrite = FileTransferOptions(overwrite=True)

        execFile = os.path.join(self.worker_data.basedir,task_input.exe)
        print "Downloading ",execFile
        localExecName = os.path.basename(execFile)
        self.file_transfer.download(localExecName,execFile)

        #uploads executable to 'running' directory
        self.file_transfer.upload("./"+localExecName,"running/"+localExecName,opts=self.allow_overwrite)

        outFile = open ( 'std.out', 'w' )
        errFile = open ( 'std.err', 'w' )
        print "Executing ",localExecName
        chmod_executable('./'+localExecName)
        import subprocess
        process = subprocess.Popen(['./'+localExecName],stdout=outFile,stderr=errFile)
        return_code = process.poll()

        while return_code is None:
                self.file_transfer.upload("./std.out",localExecName+'.std.out',opts=self.allow_overwrite)
                self.file_transfer.upload("./std.err",localExecName+'.std.err',opts=self.allow_overwrite)
                self.file_transfer.download("std.in",localExecName+".std.in")
                return_code = process.poll()
                time.sleep(10)  
        
        print "Done. Exit code was ",return_code
        
        task_input.return_code=return_code

        if return_code!=0:
            raise MoteurFailure('Application exited with code '+str(return_code))
        
        return None

    def finalize(self,x):
        pass
    
def run(input,config):
    input.scheduler = MoteurTaskScheduler
    input.scheduler.policy.LOST_TASK_MAX_ASSIGN = 1
    input.scheduler.policy.FAILED_TASK_MAX_ASSIGN = 1
    input.manager = MoteurApplicationManager
    input.worker = MoteurWorker
    input.data = RunData()
    #config.WorkerAgent.inprocess_proxy = True
    config.WorkerAgent.HEARTBEAT_DELAY = 2
    config.WorkerAgent.HEARTBEAT_TIMEOUT = 30
    config.WorkerAgent.PULL_REQUEST_DELAY = 10
    config.RunMaster.LOST_WORKER_TIMEOUT = 1200
    config.RunMaster.IDLE_WORKER_TIMEOUT = 1200
    
