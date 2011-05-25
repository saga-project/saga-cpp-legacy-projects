# this test should carry on to the end
# task #5 should cause failure and subsequently there is a failure in worker finalization

from diane_test_applications import crash as application
def run(input,config):
    input.data.commands = ["time.sleep(1)"]*10
    input.data.commands[5] = "fail()"
    input.data.workers = {1 : {'finalize' : 'fail()'}, 2 : {'finalize' : 'fail()'}}
    config.RunMaster.LOST_WORKER_TIMEOUT = 2
    config.WorkerAgent.HEARTBEAT_DELAY = 0.5
    input.scheduler.policy.STOP_IF_LOST_TASKS = False
    input.scheduler.policy.LOST_TASK_MAX_ASSIGN = 5
    input.scheduler.policy.REMOVE_FAILED_WORKER_ATTEMPTS = 0
    
def validate(mgr):
    assert mgr.N == 10
    assert mgr.done_counter == 10
    assert mgr.completed_counter == 9
    assert mgr.ignored_counter == 1

from diane.test.utils import make_timeout_test, run_test

test = make_timeout_test(20,2,validate)

if __name__ == '__main__':
    run_test(test)
    
