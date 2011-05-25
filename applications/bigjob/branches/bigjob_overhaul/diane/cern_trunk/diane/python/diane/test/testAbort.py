from diane.test.utils import make_timeout_test, run_test

# FIXME: the exact behabiour of this test case depends if in-process or out-of-process worker proxy is used
# FIXME: for the moment LOST policies are not implemented by SimpleTaskScheduler

from diane_test_applications import crash as application
def run(input,config):
    input.data.commands = ["time.sleep(1)"]*10
    input.data.commands[5] = "abort()"
    input.scheduler.policy.STOP_IF_LOST_TASKS = False
    input.scheduler.policy.LOST_TASK_MAX_ASSIGN = 1
    config.RunMaster.LOST_WORKER_TIMEOUT = 2
    config.WorkerAgent.HEARTBEAT_DELAY = 0.5
    
# validation function (accepts application manager)
def validate(mgr):
    assert mgr.N == 10
    assert mgr.done_counter == 10
    assert mgr.completed_counter == 9
    assert mgr.ignored_counter == 1

test = make_timeout_test(30, 2, validate)

if __name__ == '__main__':
    run_test(test)
