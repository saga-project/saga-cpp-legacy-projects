# this test should carry on to the end
# worker wid=1 should be removed as it fails to initialize correctly

from diane_test_applications import crash as application
def run(input,config):
    input.data.commands = ["time.sleep(1)"]*10
    input.data.workers = {1 : {'initialize' : 'fail()'}}
    config.RunMaster.LOST_WORKER_TIMEOUT = 2
    config.WorkerAgent.HEARTBEAT_DELAY = 0.5
    
def validate(mgr):
    assert mgr.N == 10
    assert mgr.done_counter == 10
    assert mgr.completed_counter == 10
    assert mgr.ignored_counter == 0

from diane.test.utils import make_timeout_test, run_test

test = make_timeout_test(30,3,validate)

if __name__ == '__main__':
    run_test(test)
    
