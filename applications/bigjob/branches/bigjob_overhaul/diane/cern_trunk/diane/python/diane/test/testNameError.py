# this is a classic test example

##################################################################
# run parameters. typically this content of a (separate) run file

from diane_test_applications import crash as application

def run(input,config):
    input.data.commands = ['time.sleep(0.3)']*10
    input.data.commands[2] = 'nameError()'
    input.scheduler.policy.STOP_IF_FAILED_TASKS = False
    input.scheduler.policy.FAILED_TASK_MAX_ASSIGN = 1

##################################################################
# The code below is specific to the testing framework.

# ----------------------------------------------------------------
# validation function (takes application manager as argument)
def validate(mgr):
    assert mgr.N == 10
    assert mgr.done_counter == mgr.N
    assert mgr.completed_counter == mgr.N-1
    assert mgr.ignored_counter == 1

# ----------------------------------------------------------------
# connection to the testing framework

from diane.test.utils import make_timeout_test, run_test

# do the diane run and start 1 worker
# timeout for the run to finish: 30 seconds
# use the above validation function after the run has been finished
test = make_timeout_test(30,1,validate,false_positives=['NameError'])

# a boilerplate code to make this file executable.
if __name__ == '__main__':
    run_test(test)
