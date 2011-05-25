
#Worker class must be available in this file (either defined here or imported from a sub-module)

from sample1 import SampleStaticApplicationManager, Worker

def run(input,config):
    from diane import SimpleTaskScheduler

    input.scheduler = SimpleTaskScheduler
    input.manager = SampleStaticApplicationManager
    input.worker = Worker
    input.scheduler.policy.REMOVE_FAILED_WORKER_ATTEMPTS = 3
    config.WorkerAgent.HEARTBEAT_DELAY = 2
    config.WorkerAgent.HEARTBEAT_TIMEOUT = 2
    config.WorkerAgent.BOOTSTRAP_CONTACT_TIMEOUT = 2
    config.WorkerAgent.PULL_REQUEST_DELAY = 0.2
