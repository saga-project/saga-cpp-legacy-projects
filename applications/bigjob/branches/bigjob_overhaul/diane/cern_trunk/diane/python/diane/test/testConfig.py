# this is a classic test example
# this test should complete all tasks normally

##################################################################
# run parameters. typically this content of a (separate) run file

from diane_test_applications import test_config as application

def run(input,config):
    input.data.config_dict = { 'WorkerAgent' : {'HEARTBEAT_TIMEOUT' : 31415} }
    config.WorkerAgent.HEARTBEAT_TIMEOUT = 31415
    input.data.N = 3

##################################################################
# The code below is specific to the testing framework.

# ----------------------------------------------------------------
# connection to the testing framework

from diane.test.utils import make_timeout_test, run_test

# do the diane run and start 1 worker
# timeout for the run to finish: 30 seconds
# no validation
test = make_timeout_test(30,1,None)

# a boilerplate code to make this file executable.
if __name__ == '__main__':
    run_test(test)
