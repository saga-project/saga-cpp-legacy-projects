""" This is a dummy application to check if the configuration options are passed correctly to the worker.
"""

import diane
logger = diane.getLogger('test_config')

import diane.config

class TCWorker:
    def check(self):
        for name in self.config_dict:
            d = self.config_dict[name]
            c = diane.config.getConfig(name)
            for opt in d:
                try:
                    val = getattr(c,opt)
                    if val != d[opt]:
                        logger.error("problem with option %s.%s : requested value %s, actual configuration value: %s",name,opt,d[opt],val)
                        return
                except AttributeError:
                    logger.error("problem with option %s.%s : not found in the actual configuration")
                    return
            
    def initialize(self,config_dict):
        self.config_dict = config_dict
        self.check()
    def finalize(self,x):
        self.check()
    def do_work(self,x):
        self.check()

from diane import SimpleApplicationManager

class TCApplicationManager(SimpleApplicationManager):
    def initialize(self,run_input):
        self.run_input = run_input
        self.cnt = 0
        self.worker_init = self.run_input.config_dict
        return [self._task() for i in range(run_input.N)]

    def tasks_done(self,tasks):
        self.cnt += len(tasks)
        
    def has_more_work(self):
        return self.cnt < self.run_input.N

class Data:
    def __init__(self):
        self.config_dict = {}
        self.N = 0
        
def run(input,config):
     from diane.SimpleTaskScheduler import SimpleTaskScheduler
     input.scheduler = SimpleTaskScheduler
     input.manager = TCApplicationManager
     input.worker = TCWorker
     input.data = Data()
     # this test will only work with the inprocess worker proxy because the config options are NOT propagated to the out-of-process proxy (worker servlet)
     config.WorkerAgent.APPLICATION_SHELL=None
     
