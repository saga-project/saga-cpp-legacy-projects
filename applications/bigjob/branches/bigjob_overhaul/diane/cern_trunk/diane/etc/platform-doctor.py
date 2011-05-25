#!/usr/bin/env python
# -*- python -*-

#  ==========================================
#  DIANE - Distributed Analysis Environment 
#  http://cern.ch/diane
#  ------------------------------------------                                          
#  This program is under GPL licence.
#  See README file for details.
#  ==========================================

#$Id: platform-doctor.py,v 1.1 2009/04/29 18:32:29 moscicki Exp $

import sys,os


def help():
    log('compile external libraries for DIANE for a new platform if platform is not supported')
    log('--fix option compiles and installes libraries in the DIANE releaseDir if needed')
    log('--test option prints the information about the platform and does not modify anything')
    log('--force=platf options compiles and installs libraries in the DIANE releaseDir unconditionally')
    sys.exit()


# we assume that we are in releaseDir/etc
releaseDir = os.path.abspath(os.path.normpath(os.path.dirname(os.path.dirname(sys.argv[0]))))
installDir = os.path.dirname(os.path.dirname(releaseDir))

pythonDir = os.path.join(releaseDir,'python')
sys.path.insert(0,pythonDir)

def log(*args):
    s = ' '.join([str(x) for x in args])
    print >> sys.stderr, s

def header(*args):
    log()
    print >> sys.stderr, '-'*80
    log(*args)
    print >> sys.stderr, '-'*80

def item(*args):
    log(' * ',*args)

force_platf = None

try:
 if sys.argv[1]=='--fix':
     fix = True
 elif sys.argv[1]=='--test':
     fix = False
 elif sys.argv[1].find('--force') == 0:
     fix = True
     force_platf = sys.argv[1][len('--force')+1:]
     if not force_platf:
         log('missing argument: --force=platf')
         sys.exit()
 else:
     help()
except IndexError:
    help()

header('This is DIANE platform doctor...')

log('releaseDir:',releaseDir)

header('checking DIANE detects the platform...')

try:
    import diane.PACKAGE
    platf = diane.PACKAGE.getPlatformString()
    if diane.PACKAGE._platformUnknown:
        log('diane.PACKAGE does not know about this platform')
        log('%s platform is assumed'%platf)
    else:
        log('this platform %s is detected OK by diane.PACKAGE'%platf)
        log('platf:',platf)
except ImportError,x:
    log("cannot 'import diane.PACKAGE':",repr(x))
    sys.exit(-1)    

header('platform identification...')

log('sys.version:',repr(sys.version))
log('sys.version_info:',repr(sys.version_info))

try:
    import platform
    log('platform.uname():',repr(platform.uname()))
    log('platform.platform():',repr(platform.platform()))
except ImportError,x:
    log("cannot 'import platform':",repr(x))

try:
    issue = file('/etc/issue').read()
    log('/etc/issue:',repr(issue))
except IOError,x:
    log('/etc/issue unreadible:',repr(x))


diane_environment = False
if os.environ.has_key('DIANE_RELEASE_DIR'):
    header('WARNING: it seems that DIANE environment has been already set up')
    diane_environment = True

header('checking if omniORB is in the system...')
try:
    import omniORB
    log('imported omniORB from the system:',omniORB.__file__)
    omniORB_system = True
except ImportError,x:
    log("cannot 'import omniORB':",repr(x))
    omniORB_system = False


header('checking omniORB distributed with DIANE (diane-env)')

omniORB_diane=True
cmd = '%s/bin/diane-env -d python -c "import omniORB; import sys; sys.exit(0)" '%releaseDir
log('running command:',cmd)
if os.system(cmd):
    omniORB_diane = False
    log('failed to import omniORB from DIANE distribution')
else:
    log('imported omniORB from DIANE distribution')

header('SUMMARY')

if diane.PACKAGE._platformUnknown:
    item('DIANE does not know this platform and assumes:',platf)
else:
    item('DIANE detects this platform as',platf)

if diane_environment:
    item('cannot check if omniORB is available of the system because DIANE environment (diane-env) has been set prior to running this script, rerun in clean shell')
else:
    item('omniORB is %savailable in the system'%{True:'',False:'NOT '}[omniORB_system])

item('omniORB distributed with DIANE %s'%{True:'imports OK',False:'DOES NOT import correctly'}[omniORB_diane])


header('ACTIONS')

def action(cmd):
    item('doing:',repr(cmd))
    if fix:
        if cmd:
            rc = os.system(cmd)
            if rc:
                log('error executing',repr(cmd))
                return 0
    return 1

def recompile_omniORB(platf=None):
    P = ""
    if platf:
        P = '--platf='+platf
    if action('python %s/etc/omniorb-build.py --auto %s > ./build.log 2>&1'%(releaseDir,P)):
        action('python %s/etc/diane-install --silent --prefix=%s --make-tarballs %s %s'%(releaseDir,installDir,P,diane.PACKAGE.getVersion()))

def doctor():

        if omniORB_diane:
            item('Nothing to be done')
            return

        if not omniORB_diane:
            if omniORB_system:
                item("Nothing to be done now but recompile omniORB if your system libraries do not work correctly.")
                return

        if not diane.PACKAGE._platformUnknown:
            recompile_omniORB()
            item('This should not happen and needs investigation. Get help on http://cern.ch/diane.')
            return
        else:
            if not fix:
                item('Run platform-doctor.py --fix and it will do these actions:')
            recompile_omniORB(platf='DUMMY_PLATFORM_STRING')
            if fix:
                item('Now set: export DIANE_PLATFORM=DUMMY_PLATFORM_STRING')

            return

if force_platf:
    item('I am forced to compile libraries for %s'%force_platf)
    recompile_omniORB(platf=force_platf)
else:
    doctor()
    
item('Documentation: http://twiki.cern.ch/twiki/bin/view/ArdaGrid/DIANEPlatformDoctor')
