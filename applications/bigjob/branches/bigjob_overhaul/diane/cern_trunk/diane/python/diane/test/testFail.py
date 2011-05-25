# this test should stop on the first failed task,
# since we are running only one worker the run will be only partially completed

from diane_test_applications import crash as application
def run(input,config):
    input.data.commands = [None]*10
    input.data.commands[5] = "fail()"
    input.scheduler.policy.STOP_IF_FAILED_TASKS = True
    input.scheduler.policy.FAILED_TASK_MAX_ASSIGN = 1
    
def validate(mgr):
    assert mgr.N == 10
    assert mgr.done_counter == 5
    assert mgr.completed_counter == 5
    assert mgr.ignored_counter == 0

from diane.test.utils import make_timeout_test, run_test

test = make_timeout_test(10,1,validate)

if __name__ == '__main__':
    run_test(test)
