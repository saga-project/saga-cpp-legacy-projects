from crash import *
run2 = run

from diane import getLogger
logger = getLogger('FinalizeCrashApplication')

class FinalizeCrashApplicationManager(CrashApplicationManager):
    def finalize(self):
        N = 20
        import time
        logger.info('finalizing for %d seconds',N)
        time.sleep(N)
        CrashApplicationManager.finalize(self)
        logger.info('finalize finished')


def run(input,config):
    run2(input,config)
    input.manager = FinalizeCrashApplicationManager
