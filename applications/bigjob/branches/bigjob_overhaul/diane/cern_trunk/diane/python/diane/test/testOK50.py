# this test should complete all tasks normally using 50 local workers

# ----------------------------------------------------------------
# run parameters. typically this content of a (separate) run file
from diane_test_applications import crash as application

def run(input,config):
    input.data.commands = ['time.sleep(0.5)']*500
    config.RunMaster.LOST_WORKER_TIMEOUT = 5
    config.WorkerAgent.HEARTBEAT_DELAY = 2
    
# ----------------------------------------------------------------
# validation function (takes application manager as argument)
def validate(mgr):
    assert mgr.N == 500
    assert mgr.done_counter == mgr.N
    assert mgr.completed_counter == mgr.N
    assert mgr.ignored_counter == 0

# ----------------------------------------------------------------
# connection to the testing framework

from diane.test.utils import make_timeout_test, run_test

# do the diane run and start 1 worker
# timeout for the run to finish: 10 seconds
# use the above validation function after the run has been finished
test = make_timeout_test(100,50,validate)

# a boilerplate code to make this file executable.
if __name__ == '__main__':
    run_test(test)
