
from diane_test_applications import crash as application

def run(input,config):
    config.MSGMonitoring.MSG_MONITORING_ENABLED = False
    input.data.commands = ['time.clock()']*2048
