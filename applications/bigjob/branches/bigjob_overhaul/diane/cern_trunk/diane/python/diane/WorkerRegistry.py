
# will be set by importing module (RunMaster.py)
logger = None
config = None

import sys
import time

import Queue as std_Queue

class Queue(std_Queue.Queue):
    def clear(self):
        try:
            self.queue.clear()
        except AttributeError: # python < 2.5
            self.queue[:] = []
        

import diane.util

import threading

class WorkerEntry:
    """ Representation of a worker agent. Contains worker information and task queues.
    """
    def __init__(self,wid,worker_uuid,max_capacity):
        self.worker_uuid = worker_uuid
        self.wid = wid
        self.registration_time = time.time()
        self.last_contact_time = self.registration_time
        self.idle_time = 0 # timestamp of the moment when the worker became idle
        self.max_capacity = max_capacity # how many tasks may be assigned to the worker at any given time (default 1), submasters may be superworkers with capacity>1, FIXME: for the moment support only static capacity (defined upfront and set to 1 ;-)

        self.initialized = False # worker may not be processing until it is initialized
        self.init_input = None # input for application initialization, optionally assigned by Task Manager
        self.init_output = None # data produced by worker initialization, optionally consumed by Task Manager
        
        #self.processing_task_num = 0 # number of processing tasks
        self.processing_tasks = {}
        self.scheduled_tasks = Queue() # currently waiting tasks to be picked by the worker

        self.alive = True # worker which has been unregistered is not alive
        self.alive_lock = threading.RLock() # this lock protects against declaring worker as not alive in concurrent threads
        
    def capacity(self):
        return self.max_capacity-len(self.processing_tasks)-len(self.scheduled_tasks.queue)

    def lost(self):
        return time.time()-self.last_contact_time>config.LOST_WORKER_TIMEOUT

    def idle_too_long(self):
        return self.idle_time and config.IDLE_WORKER_TIMEOUT and time.time()-self.idle_time>config.IDLE_WORKER_TIMEOUT
            
    def getStatusReport(self):
        return "wid=%d alive = %d registered %s last_contact %s (%f seconds ago)\n"% \
                   (self.wid,self.alive,diane.util.ctime(self.registration_time), \
                   diane.util.ctime(self.last_contact_time), \
                   time.time()-self.last_contact_time)

    def __getstate__(self):
        state = self.__dict__.copy()
        #PENDING: pickle underlying queue attribute
        #can't pickle Queue objects directly
        state['scheduled_tasks']=state['scheduled_tasks'].queue
        state['alive_lock']=None
        return state

    def __setstate__(self,state):
        #PENDING: pickle underlying queue attribute
        #unpickle queue objects...
        self.__dict__ = state.copy()
        self.__dict__['scheduled_tasks'] = Queue()
        self.__dict__['alive_lock']=threading.RLock()


class WorkerRegistryStatusSummary:
    def __init__(self):
        self.workers_number = 0
        self.alive_workers_number = 0       

class WorkerRegistry:
    def __init__(self):
        self.__all_workers = {}
        self.__all_worker_uuids = {}
        self.last_wid = 0
        self.lock = threading.RLock()

        # internal efficiency cache
        self.__waiting_workers = {} # all workers which have free scheduling slots 
        self.__alive_workers = {} # all workers which are still alive

    def schedule_tasks(self,wid,task_info_list, task_input_list):
        w = self.get(wid)
        assert(w.initialized)
        for i in range(len(task_info_list)):
            tinfo = task_info_list[i]
            if task_input_list[i] is not None:
                tinfo.task_input = task_input_list[i]
            w.scheduled_tasks.put(tinfo)
        self.update_cache(w)

    # cache accessors (for task manager)
    waiting_workers = property(lambda self: self.__waiting_workers.copy(), None, None, "get a snapshot of waiting workers cache")
    alive_workers = property(lambda self: self.__alive_workers.copy(), None, None, "get a snapshot of all alive workers")
    
    def update_cache(self,w):
        if not w.initialized:
            return
        
        if w.capacity() > 0 and w.alive:
            self.__waiting_workers[w.wid] = w
        else:
            try:
                del self.__waiting_workers[w.wid]
            except KeyError:
                pass
            
    def add(self,worker_uuid,max_capacity):
        self.lock.acquire()
        try:
            if self.__all_worker_uuids.has_key(worker_uuid):
                wid = self.__all_worker_uuids[worker_uuid]
                logger.warning('worker already registered: wid=%d, worker_uuid=%s',wid,worker_uuid)
                return None
            else:
                self.last_wid+=1
                newid = self.last_wid
                w = WorkerEntry(newid,worker_uuid,max_capacity)
                self.__all_workers[newid] = w
                self.__alive_workers[newid] = w
                self.__all_worker_uuids[worker_uuid] = newid
                logger.info('new worker registered: wid=%d, worker_uuid=%s',newid,worker_uuid)
                return w
        finally:
            self.lock.release()

    def remove(self,wid):
        try:
            self.lock.acquire()
            try:
                worker_uuid = self.__all_workers[wid].worker_uuid
            except KeyError:
                logger.warning('attempt to remove not registered worker: wid=%d',wid)
            else:
                logger.info('worker removed: wid=%d'%wid)
                self.__all_workers[wid].alive = False
                del self.__alive_workers[wid]
        finally:
            self.lock.release()

    def __iter__(self):
        # iteration may be done concurrently with the add() and remove() methods as we iterate over a *copy* of the worker dictionary
        return self.__all_workers.copy().__iter__()
    
    def get(self,wid):
        return self.__all_workers[wid]

    def get_by_uuid(self,worker_uuid):
        return self.get(self.__all_worker_uuids[worker_uuid])

    def __getstate__(self):
        state = self.__dict__.copy()
        del state['lock'] #can't pickle lock objects
        return state

    def __setstate__(self,state):
        self.__dict__ = state.copy()
        self.__dict__['lock'] = threading.RLock()

    def getStatusPickle(self):
        try:
            self.lock.acquire()
            import cPickle
            return cPickle.dumps(self)
        finally:
            self.lock.release()
            
    def getStatusReport(self):
        try:
            self.lock.acquire()
            summary = WorkerRegistryStatusSummary()
            summary.workers_number = len(self.__all_workers)
            summary.alive_workers_number = len(self.alive_workers)
            return summary
        finally:
            self.lock.release()
