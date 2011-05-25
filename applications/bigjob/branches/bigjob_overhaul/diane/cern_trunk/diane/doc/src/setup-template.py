#!/usr/bin/env python
# ===============================================
# SETUP FULL DIANE ENVIRONMENT
# Note: this is copy/paste logic for executable scripts.
# Note: minimal diane environment must be already set up (i.e. diane-env in PATH)
# Note: this code does not work in interactive python sessions or with -c python option.
import os.path,sys
setup_done = False
for p in os.environ['PATH'].split(os.pathsep):
    if os.path.exists(os.path.join(p,'diane-env')):
        sys.path.insert(0, os.path.join(os.path.dirname(p),'python'))
        import diane.PACKAGE
        diane.PACKAGE.standardSetup()
        diane.PACKAGE.initializeRuntime()
        setup_done = True
if not setup_done:
    print >> sys.stderr, 'ERROR: cannot set up DIANE environment (diane-env not in PATH)'
    sys.exit(-1)
# ===============================================    

# Here is an example:
import diane
logger = diane.getLogger('mylogger')
logger.info('Hey, I am using DIANE logger')
