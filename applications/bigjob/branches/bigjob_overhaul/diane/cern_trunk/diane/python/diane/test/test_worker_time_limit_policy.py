import diane.test.utils
from diane.test.utils import *

# in a single file we define the tests and the run configuration (run file)
this_file = os.path.join(os.path.dirname(diane.test.__file__),'test_worker_time_limit_policy.py')

class TestWorkerTimeLimitPolicy(diane.test.utils.TestBase):
    def __init__(self,*args):
        diane.test.utils.TestBase.__init__(self,*args)

    def defaultRunFile(self):
        return this_file
    
    def testWorkerTimeLimitPolicy(self):
        logger.info('begin')
        pm = self.start_master_process()[0]
        time.sleep(1)

        pw = self.start_worker_process()[0]
        time.sleep(1)

        assert(pm.poll() is None)
        assert(pw.poll() is None)

        # make sure that master has one worker registered
        assert len(self.getmasterstatus().alive_workers)==1

        # wait a bit longer than needed for WORKER_TIME_LIMIT policy to take effect
        time.sleep(15)

        # make sure worker process terminated
        logger.info('checking if worker automatically terminated')
        assert(pw.poll() is not None)
        
        # make sure that master has not alive workers
        assert len(self.getmasterstatus().alive_workers) == 0
        assert not self.getmasterstatus().get(1).alive

        logger.info('passed OK')

        
### ----------------------------------------------------------------------------
### application and system configuration for the tests
import diane_test_applications.idle
application = diane_test_applications.idle        
def run(input,config):
    config.WorkerAgent.HEARTBEAT_DELAY = 1
    config.WorkerAgent.HEARTBEAT_TIMEOUT = 1
    config.WorkerAgent.BOOTSTRAP_CONTACT_TIMEOUT = 1
    config.RunMaster.LOST_WORKER_TIMEOUT = 3
    config.RunMaster.CONTROL_DELAY = 1
    config.RunMaster.IDLE_WORKER_TIMEOUT = 0 #never
    input.scheduler.policy.WORKER_TIME_LIMIT = 10 #kill workers after 10 seconds
    logger.debug("%s %s",input,dir(input))
    logger.debug("%s %s",input.scheduler.policy, dir(input.scheduler.policy))
### ----------------------------------------------------------------------------

test = unittest.TestLoader().loadTestsFromTestCase(TestWorkerTimeLimitPolicy)

if __name__ == '__main__':
    run_test(test)
