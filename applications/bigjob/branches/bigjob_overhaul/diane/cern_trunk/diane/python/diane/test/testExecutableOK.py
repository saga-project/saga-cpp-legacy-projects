# this is a classic test example
# this test should complete all tasks normally

##################################################################
# run parameters. typically this content of a (separate) run file

from diane_test_applications import ExecutableApplication as application

import tempfile,os.path
import diane.util

def run(input,config):
    d = input.data.task_defaults # this is just a convenience shortcut

    f,fn = tempfile.mkstemp()

    f = open(fn,'w')
    print >>f, """#!/usr/bin/env bash
rm -f message.out
echo hello $* > message.out
echo "I said hello $* and saved it in message.out"
"""
    f.close()

    diane.util.chmod_executable(fn)

    # all tasks will share the default parameters (unless set otherwise in individual task)
    d.input_files = [fn]
    d.output_files = ['message.out']
    d.executable = os.path.basename(fn)

    # here are tasks differing by arguments to the executable
    for i in range(20):
        t = input.data.newTask()
        t.args = [str(i)]

##################################################################
# The code below is specific to the testing framework.

# ----------------------------------------------------------------
# connection to the testing framework

from diane.test.utils import make_timeout_test, run_test

# do the diane run and start 1 worker
# timeout for the run to finish: 30 seconds
# use the above validation function after the run has been finished
test = make_timeout_test(30,3)

# a boilerplate code to make this file executable.
if __name__ == '__main__':
    run_test(test)
