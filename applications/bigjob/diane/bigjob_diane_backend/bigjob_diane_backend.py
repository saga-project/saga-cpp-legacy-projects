from diane import IApplicationWorker, SimpleTaskScheduler, \
        SimpleApplicationManager, IApplicationManager
from diane.BaseThread import BaseThread
from diane.FileTransfer import FileTransferOptions
import time
import os
import glob
import shutil
import subprocess
import stat

def chmod_executable(path):
    "make a file executable"
    os.chmod(path,stat.S_IXUSR|os.stat(path).st_mode)


class TaskInput:
    def __init__(self):
        self.exe = None
        self.return_code = None

class WorkerData:
    def __init__(self):
        self.basedir = None


class BigJobDianeManager(SimpleApplicationManager, BaseThread):
    def __init__(self,name=None,auto_register=True):
        print "Initializing Manager"
        self.basedir = '.'
        BaseThread.__init__(self,name=name)
        SimpleApplicationManager.__init__(self)

    def list_files(self,pattern,dir=''):
        return [os.path.join(dir,os.path.basename(f)) for f in
                glob.glob(os.path.join(self.basedir,dir,pattern))]

    
    def initialize(self, run_data):
        print "Initializing Diane Application Manager"
        w = WorkerData()
        w.basedir = self.basedir
        self.worker_init = w

        # create dirs and clean up garbage
        for d in ['submitted','scheduled','running','done','failed','exit_codes','cancelled','cancel-request','err','out','in']:
            if not os.access(os.path.join(self.basedir,d),os.F_OK):
                print "Creating directory ",os.path.join(self.basedir,d)
                os.mkdir(os.path.join(self.basedir,d))

#            if d!='submitted':
#                old_jobs = self.list_files(d+'/*')
#                for f in old_jobs:
#                    print "Removing file ",f," from directory ",d
#                    os.remove(os.path.join(self.basedir,d,f))
        return []


    def run(self):
        while self.scheduler.has_more_work():
            print "Application loop ..."
            new_jobs = self.list_files('submitted/*')
            for f in new_jobs:
                print "Found new task, scheduling it"
                shutil.move(os.path.join(self.basedir,'submitted',f),os.path.join(self.basedir,'scheduled'))

                d=self._task()
                d.task_input = TaskInput()
                d.task_input.exe = 'scheduled/'+f
                # inform scheduler that there are new tasks
                self.scheduler.todo_tasks.put(d)

            time.sleep(10)

    def tasks_done(self, tasks):
        for f in tasks:
            print "Task ",f.task_input.exe," is done: moving it to \
                    directory 'done'"
            shutil.move(f.task_input.exe,'done')
            #in case the task has been put in the running dir
            os.remove(os.path.join('running',os.path.basename(f.task_input.exe)))

        
    def has_more_work(self):
             return 1

class BigJobDianeData:
    def __init__(self):
        print "Initializing Data"
        self.basedir = '.'


class BigJobDianeScheduler(SimpleTaskScheduler):
    def tasks_failed(self, tasks):
        for f in tasks:
            print "Task ",f.task_input.exe," failed: moving it to \
                    directory 'failed'"
            shutil.move(f.task_input.exe,'failed')
            #in case the task has been put in the running dir
            os.remove(os.path.join('running',
                    os.path.basename(f.task_input.exe)))


class BigJobDianeWorker(IApplicationWorker):
    def initialize(self,worker_data):
        print "Initializing worker"
        self.worker_data = worker_data
        return None

    def do_work(self, task_input):
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
        process = subprocess.Popen(['./'+localExecName],stdout=outFile,
                stderr=errFile)
        return_code = process.poll()

        while return_code is None:
            self.file_transfer.upload("./std.out",'out/'+localExecName+'.std.out',
                    opts=self.allow_overwrite)
            self.file_transfer.upload("./std.err",'err/'+localExecName+'.std.err',
                    opts=self.allow_overwrite)
            #self.file_transfer.download("std.in",localExecName+".std.in")
            return_code = process.poll()
            time.sleep(10)

        print "Done. Exit code was ",return_code

        task_input.return_code=return_code

        if return_code!=0:
            raise Exception('Application exited with code '+str(return_code))

        return None


# the run function is called when the master is started
# input.data stands for run parameters
def run(input,config):
    print "Diane master starting ..."

    input.scheduler = BigJobDianeScheduler
    input.manager = BigJobDianeManager
    input.worker = BigJobDianeWorker
    input.data = BigJobDianeData()
