# this is a classic test example
# this test should complete all tasks normally

##################################################################
# run parameters. typically this content of a (separate) run file

from diane_test_applications import crash as application

def run(input,config):
    config.WorkerAgent.HEARTBEAT_DELAY = 100
    input.data.commands = ['time.sleep(0.3)']*10

##################################################################
# The code below is specific to the testing framework.

# ----------------------------------------------------------------
# validation function (takes application manager as argument)
def validate(mgr):
    assert mgr.N == 10
    assert mgr.done_counter == mgr.N
    assert mgr.completed_counter == mgr.N
    assert mgr.ignored_counter == 0

# ----------------------------------------------------------------
# connection to the testing framework

from diane.test.utils import make_timeout_test, run_test

# do the diane run and start 1 worker
# timeout for the run to finish: 30 seconds
# use the above validation function after the run has been finished
test = make_timeout_test(30,1,validate)

# a boilerplate code to make this file executable.
if __name__ == '__main__':
    run_test(test)
