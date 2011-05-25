import diane

logger = diane.getLogger('AtlasPilotJobs')

import os.path, shutil, glob

from diane_test_applications import ExecutableApplication 


class AtlasPilotRunData(ExecutableApplication.ExecutableRunData):
    def __init__(self):
        ExecutableApplication.ExecutableRunData.__init__(self)

    def newTask(self):
        d = AtlasPilotTaskData()
        self.tasks.append(d)
        return d


AtlasPilotWorkerData = ExecutableApplication.ExecutableWorkerData

class AtlasPilotTaskData(ExecutableApplication.ExecutableTaskData):
    def __init__(self):
        ExecutableApplication.ExecutableTaskData.__init__(self)
        self.requirements = PilotTaskRequirements()

AtlasPilotTaskResult = ExecutableApplication.ExecutableTaskResult
        
PilotWorkerCapabilities = frozenset
PilotTaskRequirements = frozenset

#class PilotWorkerCapabilites:
#    pass

############################################################################################

class AtlasPilotWorker(ExecutableApplication.ExecutableWorker):
    def initialize(self,worker_data):
        self.worker_data = worker_data
        return PilotWorkerCapabilities(['a','b','c'])

############################################################################################

import threading, Queue

class PilotTodoTaskQueue:
    def __init__(self):
        self._mutex = threading.RLock()
        self.tasks = []

    def get(self,w):
        try:
            self._mutex.acquire()
            
            for i,t in enumerate(self.tasks):
                logger.info('trying to match worker %d with capabilities(%s) to task %d with requirements(%s)',w.wid,repr(w.init_output),t.tid,repr(t.task_input.requirements))                

                if t.task_input.requirements.issubset(w.init_output): #w.init_output are pilot capabilities
                    del self.tasks[i]
                    return t
            raise Queue.Empty()
        finally:
            self._mutex.release()

    def put(self,t):
        try:
            self._mutex.acquire()
            self.tasks.append(t)
        finally:
            self._mutex.release()

    def reset_queue(self,task_list):
        try:
            self._mutex.acquire()
            self.tasks = list(task_list)
        finally:
            self._mutex.release()

    def __len__(self):
        return len(self.tasks)
        
class AtlasPilotScheduler(diane.SimpleTaskScheduler):
    def __init__(self,job_master,appmgr):
        diane.SimpleTaskScheduler.__init__(self,job_master,appmgr)
        self.todo_tasks = PilotTodoTaskQueue()

# make sure that worker capabilites structure is never removed by SimpleTaskScheduler
AtlasPilotScheduler.policy.INIT_DATA_CLEANUP = False


AtlasPilotManager = ExecutableApplication.ExecutableApplicationManager
#TODO: derive mgr class from a Thread to add tasks dynamically
#TODO: it should never stop by itself



