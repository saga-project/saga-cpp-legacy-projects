#!/usr/bin/env python

import unittest, os, sys
import glob
    
def suite():
    all = unittest.TestSuite()
    for fn in glob.glob('test*.py'):
        mod = __import__(os.path.splitext(fn)[0])
        all.addTest(mod.test)
    return all

def configs():
    return glob.glob('*.cfg')
    
suite = suite()

if __name__ == '__main__':
    from optparse import OptionParser
    parser = OptionParser(description="Run full DIANE test suite. \nBy default the test suite is run once for each *.cfg file in the test directory.")
    parser.add_option("-c","--config", dest="config",default=None,help="specify the configuration file",metavar="FILE.cfg")

    options,args = parser.parse_args()

    if options.config:
        configs = [options.config] # use a specified config file
    else:
        configs = configs() # use all *.cfg files
    
    import diane.test.utils

    import logging
    diane.test.utils.logger.setLevel(logging.CRITICAL)

    from diane import getLogger
    logger = getLogger('test.driver')


    if len(configs)>1:
        logger.user('selected test configurations: %s',configs)
        import subprocess
        for c in configs:
            # run each test configuration in a separate process (to avoid possible interference between runs)
            if subprocess.call(['python',sys.argv[0],'--config=%s'%c]) != 0:
                break
    else:
        c = configs[0]
        logger.user('running full test suite using %s',c)
        cf = os.path.abspath(c)
        if not os.path.exists(cf):
            logger.error('config file %s does not exist',c)
            sys.exit(1)
        os.environ['DIANE_CONFIG'] = os.path.abspath(c)
        logger.user('output directory: %s',diane.test.utils.get_output_dir())
        unittest.TextTestRunner(verbosity=2).run(suite)
    
