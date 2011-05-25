"""
Simple test application allows to trigger application errors in different places and to verify how the framework reacts to them.
"""

###############################################################################################################
# Here we define the data structures with parameters for the run, tasks and the task result.

class CrashRunData:
    ''' Crash run is defined as a sequence of python commands - one command per task.
       The commands are to be executed by the worker agents. Supported
       commands are: "fail()", "abort()" or "time.sleep(N)". These commands
       are always specified as strings.
    '''
    def __init__(self):
        ## A list of commands to be executed as individual tasks.
        self.commands = []
        ## Worker-specific commands. The key of this dictionary is the worker id. The value is
        ## a dictionary which specifies a command to be executed at different points of the worker agent life:
        ## 'initialize,'before_do_work','after_do_work','finalize'
        self.workers = {}
        
class CrashTask:
    """ Crash task input parameters - sent from the master to the workers.
    """
    def __init__(self,command=None,tid=None):
        ## Command (string) to be executed.
        self.command = command
        ## Task identifier.
        self.tid = tid

class CrashResult:
    """ Crash task result - sent from the workers to the master when a task is completed.
    """
    def __init__(self,execution_time):
        ## Worker just tells how long it executed a task.
        self.execution_time = execution_time


###############################################################################################################
# Here we define the functionality of crash application. The application is able to:
#  - sleep n seconds
#  - raise CrashFailure exception
#  - abort
# Wow!

from diane import getLogger
logger = getLogger('CrashApplication')

class CrashFailure(Exception):
    pass

def fail(*x):
    raise CrashFailure(*x)

def nameError(*x):
    print _some_undefined_name_1234_
    
from os import abort
import time

def exec_command(x,_locals=None):
    if x is not None:
        logger.info("*** executing command: %s",repr(x))
        if _locals is None:
            _locals = locals()
        _locals.update({'fail':fail, 'abort':abort, 'time':time, 'nameError':nameError})
        exec x in _locals


###############################################################################################################
# Here we define the application components which will enable the framework to use the application's functionality.

from diane import IApplicationWorker, SimpleApplicationManager, SimpleTaskScheduler

class CrashWorker(IApplicationWorker):
    """ Execute commands.
    """
    def initialize(self,init):
        """
        This method is called only once per worker agent.
        The init parameters are sent by the master (and they result of calling make_init() method of the application manager).
        """
        logger.info('*** initialize: worker id=%d',self._agent.wid)
        self.commands = {'initialize':None, 'before_do_work':None, 'after_do_work':None, 'finalize':None}
        self.commands.update(init.get(self._agent.wid,{}))
        exec_command(self.commands['initialize'])
                         
    def finalize(self,x):
        """ This method is called only once per worker agent.
        """
        logger.info('*** finalize: worker id=%d',self._agent.wid)
        exec_command(self.commands['finalize'])
        
    def do_work(self,crash_task):
        """ This method is called to execute a task. If execution succeeds it returns the CrashResult object. This object is
        automatically sent back to the master. How many tasks are executed by a given worker agent depends
        on the scheduler and other runtime conditions.
        """
        import time
        t0=time.time()
        logger.info('*** do_work: worker id=%d tid=%d',self._agent.wid,crash_task.tid)
        _locals = {'tid':crash_task.tid}
        exec_command(self.commands['before_do_work'],_locals)
        exec_command(crash_task.command,_locals)
        exec_command(self.commands['after_do_work'],_locals)
        return CrashResult(time.time()-t0)
        
class CrashApplicationManager(SimpleApplicationManager):
    """ Simply count the completed and ignored tasks and stop when N is reached.
    All tasks are created statically at the begining.
    """
    def initialize(self, job_input):
        self.N = len(job_input.commands)
        self.done_counter = 0
        self.completed_counter = 0
        self.ignored_counter = 0
        self.worker_init = job_input.workers # this value will be sent to the worker agent when it initializes, see IApplicationWorker.initialize()
        self.total_execution_time = 0
        tasks = []
        for command in job_input.commands:
            t = self._task()
            t.task_input = CrashTask(command,t.tid)
            tasks.append(t)
        logger.info('number of commands (tasks) to be executed: %d',self.N)
        return tasks

    def tasks_done(self,tasks):
        """ This method is called when the scheduler decides that the given tasks will no longer be scheduled.
        With the simple scheduler this may happen in two cases: tasks are sucessfully completed or a retry count is hit.
        In the latter case the tasks are marked by the scheduler as ignored so it must be handled.
        """
        self.done_counter += len(tasks)
        for t in tasks:
            if t.status == t.IGNORED:
                self.ignored_counter += 1
                logger.info("*** ignored task %d", t.tid)
            else:
                self.completed_counter += 1
                # use the task output produced by the worker
                self.total_execution_time += t.task_output.execution_time
                logger.info("*** completed task %d, total execution time: %f", t.tid,self.total_execution_time)

    def has_more_work(self):
        """ Return False when the run should stop.
        """
        return self.done_counter < self.N

    def finalize(self):
        """ This function is called once the master is finishing the run. Dump task counters to a pickled file.
        """
        import pickle
        logger.info('*** crash task summary:')
        logger.info('*** N: %d',self.N)
        logger.info('*** done_counter: %d',self.done_counter)
        logger.info('*** completed_counter: %d',self.completed_counter)
        logger.info('*** ignored_counter: %d',self.ignored_counter)
        pickle.dump(self,file('crash.pp','w'))

###############################################################################################################
# Here we finally connect our application to the framework.  When a run is
# started this rundef() function is executed first: it sets the default run
# behaviour for this application.  Then a similar rundef() function from the
# run file provided by the user is executed and may customize the paeremetrs.

def run(input,config):
     input.scheduler = SimpleTaskScheduler
     input.manager = CrashApplicationManager
     input.worker = CrashWorker
     input.data = CrashRunData()

     
