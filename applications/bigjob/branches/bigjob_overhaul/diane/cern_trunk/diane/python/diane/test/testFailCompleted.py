# this test should carry on to the end (leaving task #5 ignored)
# both worker should complete the run, ignoring task #5 after a few retries

from diane_test_applications import crash as application
def run(input,config):
    input.data.commands = [None]*10
    input.data.commands[5] = "fail()"
    input.scheduler.policy.STOP_IF_FAILED_TASKS = False
    input.scheduler.policy.FAILED_TASK_MAX_ASSIGN = 4
    
# validation function (accepts application manager)
def validate(mgr):
    assert mgr.N == 10
    assert mgr.done_counter == 10
    assert mgr.completed_counter == 9
    assert mgr.ignored_counter == 1

from diane.test.utils import make_timeout_test, run_test

test = make_timeout_test(15,2,validate)

if __name__ == '__main__':
    run_test(test)
