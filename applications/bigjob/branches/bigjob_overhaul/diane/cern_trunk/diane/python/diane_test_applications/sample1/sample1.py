from diane import getLogger

logger = getLogger('sample1')

from diane import IApplicationWorker, SimpleApplicationManager
from diane.application import  ApplicationFailure

class Worker(IApplicationWorker):
    def initialize(self,x):
        logger.info('app.initialize(%s)',str(x))
        return None
    def finalize(self,x):
        # PENDING: define what gets called on do_work() error or when there is a system problem
        logger.info('app.finalize(%s)',str(x))
    def do_work(self,x):
        import time
        #time.sleep(0.5)
        logger.info('app.do_work(%s)',x)
        if x%2:
            #logger.info('failing task %s',x)
            #raise ApplicationFailure()
            pass
        return None

class SampleStaticApplicationManager(SimpleApplicationManager):
    def initialize(self,job_input):
        self.N = 10
        self.done_counter = 0
        return [self._task() for tid in range(1,self.N+1)]

    def tasks_done(self,tasks):
        self.done_counter += len(tasks)
        for t in tasks:
            if t.details.ignored:
                print "***ignored task", t.tid
            else:
                print "***completed task", t.tid
        
    def add_tasks(self):
        return []

    def make_input(self,tasks):
        for t in tasks:
            t.task_input = t.tid # FAKE DATA

    def has_more_work(self):
        return self.done_counter < self.N

