from ITaskScheduler import ITaskScheduler

import time
import Queue

import diane
logger = diane.getLogger('SimpleTaskScheduler')

class SimplePolicy:
    """ Collection of parameters to control the SimpleTaskScheduler.
    """
    
    ## True => automatically discard init_input and init_output after worker is fully initialized
    ## by resetting the corresponding attributes of WorkerEntry to None.
    ## This gives a chance for garbage collection of unneeded data in memory
    INIT_DATA_CLEANUP = True

    ## True => automatically discard task.input_data after the task has been handed out by the task scheduler (completed or ignored).
    TASK_INPUT_CLEANUP = True
    
    ## True => automatically discard task.details.output_data after the task has been handed out by the task scheduler (completed or ignored).
    TASK_OUTPUT_CLEANUP = True

    ## A number of times task execution is attempted (while task is reported as failed).
    ## After reaching this number the policy STOP_IF_FAILED_TASKS is applied.
    ## For example: FAILED_TASK_MAX_ASSIGN == 2 => failed task will be retried only once.
    FAILED_TASK_MAX_ASSIGN = 3

    ## False => ignore failed tasks and continue running. True => stop run immediately after detecting the faulty tasks.
    ## This policy applies when failed tasks exceed the FAILED_TASK_MAX_ASSIGN number.
    STOP_IF_FAILED_TASKS = False
    
    ## Automatically remove the workers which produced failed tasks more or equal than specified number of times.
    ## 0 means that workers are never removed
    REMOVE_FAILED_WORKER_ATTEMPTS = 0

    ## A number of times task execution is attempted (while task is reported as lost).
    ## After reaching this number the policy STOP_IF_LOST_TASKS is applied.
    ## For example: LOST_TASK_MAX_ASSIGN == 1 => lost task will be not be retried
    LOST_TASK_MAX_ASSIGN = 3

    ## False => ignore lost tasks and continue running. True => stop run immediately after detecting the lost tasks.
    ## This policy applies when lost tasks exceed the LOST_TASK_MAX_ASSIGN number.
    STOP_IF_LOST_TASKS = False

    ## Do not assign tasks to a worker which exceeded this limit and remove the worker when it becomes waiting.
    ## The time is specified in seconds. 0 => no limit.
    WORKER_TIME_LIMIT = 0


class TodoTaskQueue:
    """Task queue which may use the worker object as a hint for
    prioritized scheduling.  The default implementation of
    TodoTaskQueue behaves like a standard non-blocking Queue.Queue and
    ignores the hint (FIFO).
    """
    def __init__(self):
        self.queue = Queue.Queue()
    
    def put(self,t):
        self.queue.put(t)

    def get(self,w):
        """Non-blocking get. The argument w (worker object) may be use as a hint for optimal task placement. 
        Should raise Queue.QueueEmpty exception if nothing in the queue [for this worker].
        """
        return self.queue.get(block=False)

    def reset_queue(self,task_list):
        try: # 2.4 use optimized collections 
            import collections
            self.queue.queue = collections.deque(task_list)
        except ImportError: # <2.4 (FIXME: the behaviour is not equivalent as the tasks are appended (queue is not reset))
            for t in task_list:
                self.queue.put(t)

    def __len__(self):
        return len(self.queue.queue)
        
        

class SimpleTaskScheduler(ITaskScheduler):
    ''' Self-scheduler for independent tasks. Tasks are assigned to workers on "first-come, first-served" basis.
    The scheduler is controlled by SimplePolicy parameters.
    '''
    policy = SimplePolicy()
    
    def __init__(self,job_master,appmgr):
        ITaskScheduler.__init__(self,job_master,appmgr)
        self.todo_tasks = TodoTaskQueue()
        self.completed_tasks = Queue.Queue()
        self.worker_attempts = {}
        self.processing = False
        
    def initialize(self,job_input):
        self.todo_tasks.reset_queue(self.appmgr.initialize(job_input))

    def run(self):
        # make sure that has_more_work() returns a correct value since any appmgr thread which may be started
        # by the ITaskScheduler.run() below may use it
        self.processing = True

        # must call base class implementation first
        ITaskScheduler.run(self)
        
        from diane.util.periodic_call import PeriodicCall
        
        msg = PeriodicCall(1,logger.info)

        while self.appmgr.has_more_work() and not self.should_stop():

            #print 'waiting workers:',self.job_master.worker_registry.waiting_workers
            for w in self.job_master.worker_registry.waiting_workers.values():

                #logger.debug('***** checking policy.WORKER_TIME_LIMIT %s %s',self.policy, dir(self.policy))
                if self.policy.WORKER_TIME_LIMIT:
                    if time.time()-w.registration_time > self.policy.WORKER_TIME_LIMIT:
                        logger.info('removing worker %s due to exceeded WORKER_TIME_LIMIT',w.wid)
                        self.job_master.remove_worker(w)
                        continue
                
                tasks = []
                try:
                    for i in range(w.capacity()):
                        tasks.append(self.todo_tasks.get(w))
                except Queue.Empty:
                    pass

                if tasks:
                    logger.info('scheduling tasks tid=%s to worker wid=%d',[t.tid for t in tasks],w.wid)
                    self.appmgr.make_input(tasks,w)
                    self.worker_attempts[w.wid] += len(tasks)
                    self.job_master.schedule(w,tasks)
                else:
                    break

            try:
                while True:
                    t = self.completed_tasks.get(False)
                    r = t.details.task_output
                    logger.info('consuming task tid=%d from worker wid=%d',t.tid,t.details.assigned_wid)
                    logger.debug('task tid=%d -> result=%s',t.tid,r)
                    self.appmgr.tasks_done([t])
                    # clean input and output associated with the task
                    if self.policy.TASK_INPUT_CLEANUP:
                        del t.task_input
                    if self.policy.TASK_OUTPUT_CLEANUP:
                        del t.details.task_output
            except Queue.Empty:
                pass

            #msg("todo %s, completed %s",[t.tid for t in self.todo_tasks.queue], [t.tid for t in self.completed_tasks.queue])
            time.sleep(0.1)

        def dump_state(f):
            print >>f, 'completed tasks:', len(self.completed_tasks.queue)
            print >>f, 'todo tasks:', len(self.todo_tasks)

        self.appmgr.finalize()
        
        logger.info('dumping processing state to file: tasks.txt')
        dump_state(file('tasks.txt','w'))
        logger.info('terminating task manager...')

        # we end processing AFTER appmgr.finalize() has completed (bugfix:http://savannah.cern.ch/bugs/?73322)
        self.processing = False
        
    def has_more_work(self):
        return self.processing

    def worker_added(self,w_entry):
        #FIXME: w_entry.init_data not implemented yet...
        logger.info('TaskScheduler: worker_added(%d)',w_entry.wid)
        w_entry.init_input = self.appmgr.make_init(w_entry)
        self.worker_attempts[w_entry.wid] = 0

    def worker_removed(self,w_entry):
        logger.info('TaskScheduler: worker_removed(%d)',w_entry.wid)

    def worker_initialized(self,w_entry):
        """This method applies the self.policy.INIT_DATA_CLEANUP """
        if self.policy.INIT_DATA_CLEANUP:
            # reset to None for garbage potential collection
            w_entry.init_output = None  
            w_entry.init_input = None

    def tasks_completed(self, tasks):
        logger.info('TaskScheduler: tasks_completed(%s)',[t.tid for t in tasks])
        
        for t in tasks:
            self.completed_tasks.put(t)
        
    def tasks_failed(self, tasks):
        logger.info('TaskScheduler: tasks_failed(%s)',[t.tid for t in tasks])
        for t in tasks:
            if len(t.history) < self.policy.FAILED_TASK_MAX_ASSIGN:
                self.todo_tasks.put(t)
            else:
                if self.policy.STOP_IF_FAILED_TASKS:
                    logger.info('stopping scheduler: tid=%d -> STOP_IF_FAILED_TASKS is True and FAILED_TASK_MAX_ASSIGN<%d',t.tid,len(t.history))
                    self.stop()
                else:
                    self.job_master.ignore_tasks([t])
                    self.appmgr.tasks_done([t])
            
            wid = t.details.assigned_wid
            if self.policy.REMOVE_FAILED_WORKER_ATTEMPTS and self.worker_attempts[wid] >= self.policy.REMOVE_FAILED_WORKER_ATTEMPTS:
                logger.info('Maximum number of failed attempts per worker reached (%d).Worker wid %s is going to be removed.',
                            self.policy.REMOVE_FAILED_WORKER_ATTEMPTS,wid)
                self.job_master.remove_worker(wid)

    def tasks_lost(self,tasks):
        logger.info('TaskScheduler: tasks_lost(%s)',[t.tid for t in tasks])
        for t in tasks:
            if len(t.history) < self.policy.LOST_TASK_MAX_ASSIGN:
                self.todo_tasks.put(t)
            else:
                if self.policy.STOP_IF_LOST_TASKS:
                    logger.info('stopping scheduler: tid=%d -> STOP_IF_LOST_TASKS is True and LOST_TASK_MAX_ASSIGN<%d',t.tid,len(t.history))
                    self.stop()
                else:
                    self.job_master.ignore_tasks([t])
                    self.appmgr.tasks_done([t])


    def tasks_unscheduled(self,tasks):
        logger.info('TaskScheduler: tasks_lost(%s)',[t.tid for t in tasks])
        for t in tasks:
            self.todo_tasks.put(t)

        
    
#============================================================================
#============================================================================
#           JUNK                     JUNK                    JUNK
#============================================================================
#============================================================================

## class TaskRegistry:
##     """Helper class to keep track of tasks and quickly access them according to their status.
##     Same tasks may be in more than one state at the same time.
##     """
##     def __init__(self,task_list):
##         self.all = {}
##         for s in TaskStatus.all():
##             self.all[s] = {}
            
##         self.add_tasks(task_list)

##     def add_tasks(self,task_list):
##         for t in task_list:
##             self.all[t.status][t.tid] = t

##     def update_status(self,new_status,task_list,old_status=None):
##         snap = self.all.copy()
##         try:
##             assert(TaskStatus.is_status(new_status))
##             if old_status is not None:
##                 assert(TaskStatus.is_status(old_status))

##             for t in task_list:

##                 if old_status is None:
##                     old_stat = t.status
##                 else:
##                     old_stat = old_status

##                 del self.all[old_stat][t.tid]
##                 self.all[new_status][t.tid] = t
##         except KeyError,x:
##             logger.exception('update_status(new_status=%d,task_list(tid,status)=%s,old_status=%s'%(new_status,repr([(t.tid,t.status) for t in task_list]),repr(old_status)))
##             self.print_snapshot(snap)
            
##     def get_tasks(self,status):
##         return self.all[status]

##     def print_snapshot(self,snap=None,stream=None):
##         if stream is None:
##             import sys
##             stream = sys.stderr
##         if snap is None:
##             snap = self.all
##         for stat in snap:
##             print >>stream, 'status=',stat,':',
##             for t in snap[stat]:
##                 print >>stream, snap[stat][t].tid,
##             print >>stream
    
## class CheckpointableObject:
##     def checkpoint(self,f):
##         raise NotImplementedError

##     def restore(self,f):
##         raise NotImplementedError

##     restore = staticmethod(restore)
        
    
## class ComplicatedTaskScheduler(TaskScheduler):
##     def __init__(self,job_master):
##         TaskScheduler.__init__(self,job_master)
##         self.tasks = None
##         self.completed_tasks = Queue.Queue()
        
##     def initialize(self,job_input):
##         self.N = 100
##         self.tasks = [TaskInfo(tid) for tid in range(1,self.N+1)]
##         self.reg = TaskRegistry(self.tasks)

##     def make_input(self,tasks):
##         for t in tasks:
##             t.task_input = t.tid # FAKE DATA
    
##     def run(self):
##         while self.has_more_work() and not self.should_stop():

##             for w in self.job_master.worker_registry.waiting_workers.values():
##                 n = w.capacity()
##                 tasks = []
##                 for status in [TaskStatus.FAILED, TaskStatus.LOST,TaskStatus.NEW]:
##                     tasks += self.reg.get_tasks(status).values()[:n-len(tasks)]
##                 self.make_input(tasks)
##                 logger.info('scheduling tasks tid=%s to worker wid=%d',[t.tid for t in tasks],w.wid)
##                 self.reg.update_status(TaskStatus.PROCESSING,tasks)
##                 self.job_master.schedule(w,tasks)
##             try:
##                 while True:
##                     t = self.completed_tasks.get(False)
##                     r = t.details.task_output
##                     logger.info('consuming task tid=%d from worker wid=%d',t.tid,w.wid)
##                     logger.debug('task tid=%d -> result=%s',t.tid,r)
##                     # clean input and output associated with the task (FIXME: configurable)
##                     del t.task_input
##                     del t.details.task_output
##             except Queue.Empty:
##                 pass

##             time.sleep(0.1)

##         def dump_state(f):
##             for stat in self.reg.all:
##                 print >>f, 'status:',stat
##                 for tid in self.reg.all[stat]:
##                     print >>f, tid,
##                     t = self.reg.all[stat][tid]
##                     for a in t.details.__dict__:
##                         print >>f, '%s:%s '%(a,repr(getattr(t.details,a))),
##                     print >>f

##         logger.info('dumping processing state to file: tasks.txt')
##         dump_state(file('tasks.txt','w'))
                
##         logger.info('terminating task scheduler...')
        
##     def has_more_work(self):
##         return len(self.reg.get_tasks(TaskStatus.COMPLETED)) + len(self.reg.get_tasks(TaskStatus.IGNORED)) != self.N

##     def worker_added(self,w_entry):
##         logger.info('task manager: worker_added(%d)',w_entry.wid)

##     def worker_removed(self,w_entry):
##         logger.info('task manager: worker_removed(%d)',w_entry.wid)
        
##     def tasks_completed(self, tasks):
##         logger.info('task manager: tasks_completed(%s)',[t.tid for t in tasks])
        
##         for t in tasks:
##             t.update(TaskStatus.COMPLETED)
##             self.completed_tasks.put(t)
##             print "completed task", t.tid
            
##         self.reg.update_status(TaskStatus.COMPLETED,tasks,TaskStatus.PROCESSING)
        
##     def tasks_failed(self, task):
##         logger.info('task manager: tasks_failed(%s)',[t.tid for t in tasks])
##         for t in tasks:
##             if len(t.history) >= MAX_ASSIGN:
##                 t.ignore()        
##                 self.reg.update_status(TaskStatus.IGNORED,[t],TaskStatus.PROCESSING)
##                 print 'ignoring task',t
##             else:
##                 self.reg.update_status(TaskStatus.FAILED,[t],TaskStatus.PROCESSING)

##     def tasks_lost(self,tasks):
##         logger.info('task manager: tasks_lost(%s)',[t.tid for t in tasks])
##         for t in tasks:
##             t.update(TaskStatus.LOST)
##         self.reg.update_status(TaskStatus.LOST,tasks,TaskStatus.PROCESSING)

##     def tasks_unscheduled(self,tasks):
##         logger.info('task manager: tasks_lost(%s)',[t.tid for t in tasks])
##         for t in tasks:
##             t.update(TaskStatus.UNSCHEDULED)
##         self.reg.update_status(TaskStatus.LOST,tasks,TaskStatus.PROCESSING)

        
    
        
    
