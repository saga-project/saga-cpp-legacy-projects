import diane.test.utils
from diane.test.utils import *

# in a single file we define the tests and the run configuration (run file)
this_file = os.path.join(os.path.dirname(diane.test.__file__),'test_timeouts.py')

class TestTimeouts(diane.test.utils.TestBase):
    def __init__(self,*args):
        diane.test.utils.TestBase.__init__(self,*args)

    def defaultRunFile(self):
        return this_file
    
    def testWorkerSuspend(self):
        logger.info('begin')
        pm = self.start_master_process()[0]
        time.sleep(1)

        pw = self.start_worker_process()[0]
        time.sleep(1)

        assert(pm.poll() is None)
        assert(pw.poll() is None)

        # make sure that master has one worker registered
        assert len(self.getmasterstatus().alive_workers)==1

        # suspend worker
        logger.info('suspending worker')
        os.kill(pw.pid,signal.SIGTSTP)

        time.sleep(5)

        logger.info('resuming worker')
        os.kill(pw.pid, signal.SIGCONT)

        time.sleep(10) # need to leave some time for the worker agent to finish if servlet sandbox process is used

        # make sure worker process terminated
        logger.info('checking if worker automatically terminated')
        assert(pw.poll() is not None)

        #print getmasterstatus().getStatusReport()
        
        # make sure that master has not alive workers
        assert len(self.getmasterstatus().alive_workers) == 0
        assert not self.getmasterstatus().get(1).alive

        logger.info('passed OK')

    def testMasterKill(self):
        logger.info('begin')
        pm = self.start_master_process()[0]
        time.sleep(1)

        pw = self.start_worker_process()[0]
        time.sleep(1)

        assert(pm.poll() is None)
        assert(pw.poll() is None)        

        # make sure that master has one worker registered
        assert len(self.getmasterstatus().alive_workers)==1

        logger.info('killing master')
        os.kill(pm.pid,signal.SIGINT)

        time.sleep(10)

        logger.info('checking if worker automatically terminated')
        assert(pw.poll() is not None)

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
### ----------------------------------------------------------------------------

test = unittest.TestLoader().loadTestsFromTestCase(TestTimeouts)

if __name__ == '__main__':
    run_test(test)
