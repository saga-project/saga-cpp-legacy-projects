# this is a classic test example
# this test should complete all tasks normally

##################################################################
# run parameters. typically this content of a (separate) run file

from diane_test_applications import crash as application

default_master = """
endPoint = giop:tcp::22000
"""

def run(input,config):
    config.MSGMonitoring.MSG_MONITORING_ENABLED = False
    input.data.commands = ['time.clock()']*64

