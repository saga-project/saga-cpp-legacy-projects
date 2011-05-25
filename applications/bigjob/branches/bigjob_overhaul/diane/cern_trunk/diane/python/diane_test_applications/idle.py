import diane
logger = diane.getLogger('idle')

# this is an application adapter which does nothing (useful for testing purposes)

class Worker:
    def initialize(self,x):
        logger.debug('idle initialize')
    def finalize(self,x):
        logger.debug('idle finalize')
    def do_work(self,x):
        logger.debug('idle do work')        

from diane import SimpleApplicationManager
from diane import SimpleTaskScheduler

class IdleApplicationManager(SimpleApplicationManager):
    def has_more_work(self):
        return True

# define application symbol to point to this module if we are not being imported
# this allows to use this file as a run file
if __name__ != 'diane_test_applications.idle':
    import diane_test_applications.idle as application

def run(input,config):
     input.scheduler = SimpleTaskScheduler
     input.manager = IdleApplicationManager
     input.data = None
     input.worker = Worker

