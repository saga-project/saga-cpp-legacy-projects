
# this modules contains functions which interface to the project release structure and environment

######################################################################################
# description of external packages

# in case of 64 bit architectures the extension modules are placed in lib64 and not in lib...
# in case of 32 bit architectures the extension modules are all placed in lib - the setup below is slightly redundant in this case

_externalPackages = {
    'omniORB' : { 'version' : '4.1.5',
                  'vars': {
                     'PYTHONPATH' : 'lib64/python%(PYVER)s/site-packages:lib/python%(PYVER)s/site-packages',
                     'PATH' : 'bin',
                     'LD_LIBRARY_PATH' : 'lib' }},

    'subprocess':{'version' : '2.4.2',
                  'vars' : {
                      'PYTHONPATH' : 'lib/python2.2/site-packages'},
                 'maxHexVersion' : '0x20400f0',
                 'noarch':True},

    'stomputil': {'version' : '2.4',
                  'vars' : {
                      'PYTHONPATH' : 'python' },
                  'noarch':True}
                      
    }

# PLATFORM STRINGS:
#
# See function _detectPlatform() below for a list of known platform strings
#
# DEFAULT is a special platform string which indicates a best-guess default binary
# NOPLATF is a special platform string which indicates that binary dependencies are not needed
# 

# Python version control:
# %(PYVER)s is a token which gets replaced by "major.minor" version string as defined by sys.version[:3]
# the python version should be defined by the operating system type in the pyver_table

######################################################################################
# Setup the environment for _externalPackages and restart the currect process
#

# This is disabled if DIANE_NO_INTERNAL_SETUP variable is set. Some debuggers
# (e.g. WingIDE) cannot work correctly when the process is restarted
# therefore internal setup should be disabled and instead the environment
# should be set in the system shell (see below)
#
# If print_env option is True then print the bash environment commands to
# stdout without restarting the process. In bash this output may be sourced
# using backquotes or $() syntax. This is used to implement diane-env command.

import os,sys

PROG = os.path.basename(sys.argv[0])

def standardSetup(print_env=False):
    minimalSetup(print_env)
    fullSetup(print_env)

def minimalSetup(print_env=False):
    import diane.PACKAGE
    # path to diane commands
    diane.PACKAGE.prependPath('PATH',os.path.join(diane.PACKAGE.getReleaseDir(), 'bin'))
    # ganga environment
    diane.PACKAGE.prependPath('GANGA_CONFIG_PATH',os.path.join(diane.PACKAGE.getReleaseDir(), 'etc', 'ganga.ini' ))
    try:
        diane.PACKAGE.prependPath('PATH',os.path.join(diane.PACKAGE.getSiteRootDir(),'ganga','install',file(os.path.join(diane.PACKAGE.getSiteRootDir(),'packages','GANGA_VERSION')).read().strip(),'bin'))
    except IOError,x:
        print >>sys.stderr, "%s: PATH to ganga not set (%s)"%(PROG,x)

    # set DIANE_USER_WORKSPACE if not defined
    import diane.workspace
    os.environ.setdefault('DIANE_USER_WORKSPACE',diane.workspace.getPath(''))

    # print minimal user environment
    if print_env:
        for v in ['PATH','GANGA_CONFIG_PATH','PATH', 'DIANE_USER_WORKSPACE']:
            print 'export %s=%s'%(v,os.environ[v]),

def fullSetup(print_env=False):
    # switch off the warnings from incompatible extension modules (if any)
    import warnings
    warnings.simplefilter('ignore', RuntimeWarning)

    # keep all var names which have been modified so we can print their values later
    modified_vars = {}
    
    # setup the environment only if the process has not been already rexecuted
    if not os.environ.has_key('DIANE_NO_INTERNAL_SETUP'):

        # the first element in the sys.path is the diane package directory (defined in the executable boiler plate)
        prependPath('PYTHONPATH',sys.path[0])

        # put diane workspace apps directory into the python path
        import diane.workspace
        prependPath('PYTHONPATH',diane.workspace.getPath('apps'))
        prependPath('PYTHONPATH',diane.workspace.getPath('submitters'))

        os.environ['DIANE_RELEASE_DIR'] = getReleaseDir()
        
        for name in _externalPackages:
            for v in _externalPackages[name]['vars']:
                pp = getPackagePath(name,v)
                if pp:
                    modified_vars[v] = 1
                    prependPath(v,pp)

        if print_env:
            for v in modified_vars:
                print 'export %s=%s'%(v,os.environ[v]),
            print #add new line to flush stdout
        else:
            from diane import getLogger
            logger = getLogger('setup')
            logger.debug('re-executing setup process, sys.executable=%s, sys.argv=%s',sys.executable,sys.argv)
            # restart current process (for LD_LIBRARY_PATH to take effect)
            os.environ['DIANE_NO_INTERNAL_SETUP'] = '1'
            _reExecThisProcess()

    from diane import getLogger
    logger = getLogger('setup')
    logger.info('This is DIANE version "%s"',getVersion())
    logger.debug('current process: sys.executable=%s, sys.argv=%s',sys.executable,sys.argv)

def initializeRuntime():
    # initialize DIANE's own configuration (read the config file etc)
    from diane.config import initialize, ConfigError
    try:
        initialize()
    except ConfigError,x:
        from diane import getLogger
        logger = getLogger('setup')
        x.warning(logger)


######################################################################################
# platform detection code - to be updated if new platforms are introduced

def _detectPlatform():
    """ Try to guess the platform string according to the operating
    system, current environment and python interpreter.  DIANE adopts
    the Ganga strategy which provides precompiled external packages on
    a limited set of _default platforms_ as explained in:
    https://twiki.cern.ch/twiki/bin/view/ArdaGrid/GangaSupportedPlatforms
    This function is set only to detect the well-known platform
    strings as defined by the LCG SPI project and is not meant to be a
    generic platform detection utility. If the platform cannot be
    guessed a default one is returned. This may or may not work on
    other systems. In this case you should resolve the external binary
    dependencies yourself.

    The python version should be set in the pyver_table.

    [This function also detects few extra platforms which happen to be
    our development platforms but do not exist in LCG SPI.]

    Comments about current implementations:

    We ignore IA64 architecture (Opteron) as not frequently used.

    """

    import sys

    # True if 64 bit architecture
    bits64 = bool(sys.maxint >> 32)
    #print '64 bits',bits64
    arch = {True: "x86_64", False: "i686"}

    try:
        import platform
        p=platform.platform()

        # guess system paramaters first

        # python version id, e.g. python2.6 -> py26
        pyver = 'py'+''.join(platform.python_version_tuple()[:-1])

        # let's try to guess the compiler id by taking first two words:
        # the first word should be the compiler name (GCC)
        # the second word should be the version in format x.y.z
        
        c = platform.python_compiler().split()[:2]
        compver = c[0].lower()+''.join(c[1].split('.')[:2])

        # DISABLED UNTIL NEEDED AGAIN
        ## unusual encoding of Unicode
        #if  sys.maxunicode==(1<<16)-1:
        #    UCS = "UCS2-"
        #else:
        #    UCS = ""


        ##########################
        # handle special cases

        # loni platform with some parameters hardcoded
        if 'redhat' in p and 'loni' in p:
            return '%s-rhloni4-gcc43-%s'%(arch[bits64],pyver)

        dist = platform.dist()

        # no need binaries on ubuntu
        if dist[0].lower() == 'ubuntu':
            return "NOPLATF"

        ##########################
        # other platforms

        # let's assume SLC for all redhat-based platforms

        distid = dist[0]

        if distid == 'redhat':
            distid = 'slc'

        distver = distid+dist[1].split('.')[0] # major version of the operating system


        return "%s-%s-%s-%s"%(arch[bits64],distver,compver,pyver)

    except ImportError:
        pass

    _warnUnknownPlatform()
    return "DEFAULT"

def _warnUnknownPlatform(s=''):
    global _platformUnknown
    _platformUnknown=True
    print >> sys.stderr, "WARNING: unknown platform (%s), assuming DEFAULT"%repr(sys.version),s

#default python version for each type of the operating system
pyver_table = {'DEFAULT':'2.4'}


######################################################################################
# ganga packaging and system information query functions

def getVersion():
    import os.path
    return os.path.basename(_getTreeDir(3))

def getExternalDir():
    "Return the external dir (relative to the location of diane python packages)"
    global _externalHome
    if not _externalHome:
        _externalHome = os.path.join(getSiteRootDir(),'external')
    return _externalHome

def getPlatformString():
    "detect platform automatically and return the platform string, if platform not recognized return None"
    global _platformString
    if not _platformString:
        if os.environ.has_key('DIANE_PLATFORM'):
            _platformString = os.environ['DIANE_PLATFORM']
            print >> sys.stderr, "WARNING: platform string overriden by environment variable DIANE_PLATFORM (%s)"%_platformString
        else:
            _platformString = _detectPlatform()
    return _platformString

def getSiteRootDir():
    "get the site installation root directory (the one containing: external, install, www,...)"
    return _getTreeDir(5)

def getReleaseDir():
    "get path to the release directory (the one containing: bin,etc,python...) "
    return _getTreeDir(3)

# utility used by the installation and packaging system
def getPackageInstallationPaths(name):
    """ For NOPLATF platforms return an empty string.
    """
    p = _externalPackages[name]
    if p.has_key('noarch') and p['noarch']:
        platf = 'noarch'
    else:
        platf = getPlatformString()

    if platf == 'NOPLATF':
        return ('','')

    return (os.path.join(name,p['version'],platf),
            '-'.join([name,p['version'],platf,'diane.tar.gz']))

######################################################################################
# auxiliary functions and classes

def prependPath(var,value):
    if value:
        try:
            suffix = ':'+os.environ[var]
        except KeyError:
            suffix = ''
        os.environ[var] = value+suffix

def _reExecThisProcess():
    # FIXME: /usr/bin/env: python: Too many levels of symbolic links
    # this problem is caused by loops in symbolic links in LD_LIBRARY_PATH
    os.execv(sys.executable,[sys.executable]+sys.argv)

def getPackagePath(name,var=None,check=True,force=False):
    """Return the top of the external package (including version and platform components of the path).
    If var is specified the return the full path to the subdirectory defined by var.
    Note that var may define multiple subdirectories separated by colon.
    If check is True then print warning messages if the paths do not exist.
    If force is True then disable conditional packages (such as specified with maxHexVersion).
    If platform is NOPLATF then return an empty string.
    """
    p = _externalPackages[name]

    # if checking enabled and current python interpreter version is high enough
    # then return empty string (package is not required)
    if not force and p.has_key('maxHexVersion'):
        if sys.hexversion >= int(p['maxHexVersion'],16):
            return ""

    if p.has_key('noarch') and p['noarch']:
        platf = 'noarch'
    else:
        platf = getPlatformString()
        
    if platf == 'NOPLATF':
        return ''

    prefix_path = [getExternalDir(),name,p['version'],platf]
    
    def transform(s):
        # get the distribution id
        try:
            distver = platf.split('-')[1]
        except IndexError:
            distver = platf

        # end look up the fixed python version in the pyver table if needed
        try:
            pyver = pyver_table[distver]
        except KeyError:
            pyver = sys.version[:3]
        return s%{"PYVER":pyver}

    if var:
        paths = [transform(os.path.join(*(prefix_path+[p]))) for p in p['vars'][var].split(':') if p]
    else:
        paths = [os.path.join(*prefix_path)]

    for p in paths:
        if not os.path.exists(p):
            from diane import getLogger
            logger = getLogger('setup')
            logger.debug('path %s does not exist (setting %s for %s)',p,var,name)

    return ':'.join(paths)

   
def _getTreeDir(levels):
    "return the path to diane python directory up specified number of parent levels"
    import diane, os.path
    def dirname(path,n):
        for i in range(n):
            path = os.path.dirname(path)
        return path
    return os.path.abspath(dirname(diane.__file__,levels))    
    

######################################################################################
# internal variables (initialized by standardSetup)

# location of external packages
_externalHome = None

# platform string
_platformString = None

# this is set to True if platform cannot be detected and a default is guessed
_platformUnknown = False
