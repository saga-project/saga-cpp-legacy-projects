#!/usr/bin/env python

# $Id: omniorb-build.py,v 1.9 2009/06/04 13:25:01 moscicki Exp $

__version__ = "0.2"
__author__  = "Adrian.Muraru@cern.ch"

import sys,os

# ******************************************************************************
# CONFIGURATION TABLE
#
# ******************************************************************************
#
# external/omniORB/4.1.5:
#    OMNIORB_VERSION = 4.1.5
#    OMNIORBPY_VERSION = 3.5
#    OPENSSL_VERSION = 0.9.8h
#    OMNIORB_GSI_PATCH_TARBALL = omniORB-4.1.0-GSI.tar.gz
#
# external/omniORB/4.1.2:
#    OMNIORB_VERSION = 4.1.2
#    OMNIORBPY_VERSION = 3.2
#    OPENSSL_VERSION = 0.9.8h
#    OMNIORB_GSI_PATCH_TARBALL = omniORB-4.1.0-GSI.tar.gz
#
# external/omniORB/4.1.0: 
#    OMNIORB_VERSION = 4.1.0
#    OMNIORBPY_VERSION = 3.0
#    OPENSSL_VERSION = 0.9.8e
#    OMNIORB_GSI_PATCH_TARBALL = omniORB-4.1.0-GSI.tar.gz
#
# ******************************************************************************
# KEEP THIS TABLE UP-TO-DATE IF YOU MODIFY CURRENT CONFIGURATION
#
#
# HERE IS THE CURRENT CONFIGURATION
CONFIG={}

CONFIG['DIANE_PACKAGES_URL'] = 'http://cern.ch/diane/packages/'

CONFIG['SF_MIRROR'] = CONFIG['DIANE_PACKAGES_URL']
CONFIG['OMNIORB_VERSION']='4.1.5'
CONFIG['OMNIORBPY_VERSION']='3.5'

##Build openssl (0.9.7l is the minimum version required for OMNIORB + GSI)

CONFIG['OPENSSL_URL'] = CONFIG['DIANE_PACKAGES_URL']
CONFIG['OPENSSL_VERSION'] =  '0.9.8h' #'0.9.7m'

CONFIG['OMNIORB_GSI_PATCH_TARBALL'] = "omniORB-4.1.0-GSI.tar.gz"

CONFIG['CRT_DIR']=os.path.abspath(os.getcwd())
CONFIG['BUILD_DIR']='%(CRT_DIR)s'%CONFIG
CONFIG['SRC_DIR']='%(CRT_DIR)s/OMNIORB_SRC' % CONFIG


# do not use the external mirrors as they may occasionally change
#CONFIG['SF_MIRROR']='http://switch.dl.sourceforge.net/sourceforge/omniorb/'
#CONFIG['OPENSSL_URL'] = 'http://www.openssl.org/source/'

# ******************************************************************************

import sys

def usage():
    print >> sys.stdout, """Compile omniorb from sources in specified directory.
Usage: %s [--pwd | --auto] [--platf=PLATF]

If --pwd is specified then compile in the current working directory. If --auto is specified then guess the platform and
the location of external directory (requires diane-env --devel). Optional PLATF may be used to override the platform string.
"""%sys.argv[0]
    sys.exit(-1)
    
PLATF = None
DETECT = None

try:
    opt = sys.argv[1]

    if opt == '--pwd':
        DETECT = False
    else:
        if opt == '--auto':
            DETECT = True
        else:
            usage()
except IndexError:
    usage()

try:
    opt = sys.argv[2]
    if '--platf' in opt:
        PLATF=opt.split('=')[-1]
    else:
        usage()
except IndexError:
    pass

if DETECT:
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
            #diane.PACKAGE.initializeRuntime() # we don't need runtime
            setup_done = True
    if not setup_done:
        print >> sys.stderr, 'ERROR: cannot set up DIANE environment (diane-env not in PATH)'
        sys.exit(-1)
    # ===============================================    
    import diane.PACKAGE
    if PLATF is None:
        PLATF = diane.PACKAGE.getPlatformString()
    CONFIG['BUILD_DIR'] = os.path.join(diane.PACKAGE.getExternalDir(),'omniORB',CONFIG['OMNIORB_VERSION'],PLATF)
    
print "Using configuration:"
for c in CONFIG:
    print "%s = %s" % (c,CONFIG[c])


def ccmd( command , fail_msg='' ):
    print 'doing command:',command
    if os.system(command)!=0:
        print 'command failed:',command
        print fail_msg
        sys.exit()

try:
    if not os.path.exists("%(SRC_DIR)s" % CONFIG):
        os.makedirs("%(SRC_DIR)s" % CONFIG)

    ###(Step 1) openssl ###
    os.chdir("%(SRC_DIR)s" % CONFIG)
    ccmd('wget %(OPENSSL_URL)s/openssl-%(OPENSSL_VERSION)s.tar.gz'  % CONFIG)
    ccmd('tar xzvf openssl-%(OPENSSL_VERSION)s.tar.gz'  % CONFIG)
    os.chdir('openssl-%(OPENSSL_VERSION)s' % CONFIG)
    cfg_cmd = "./config -fPIC  -D_REENTRANT -DDSO_DLFCN -DHAVE_DLFCN_H -DKRB5_MIT  -I/usr/kerberos/include "\
              "-DL_ENDIAN -DTERMIO -Wall -O2 -pipe -Wa,--noexecstack -DSHA1_ASM -DMD5_ASM -DRMD160_ASM zlib-dynamic threads shared "\
              "--prefix=%(BUILD_DIR)s" % CONFIG
    print "Doing %s " % cfg_cmd
    ccmd(cfg_cmd,'Fail to configure openssl-%(OPENSSL_VERSION)s' % CONFIG )
    print 'Building openssl-%(OPENSSL_VERSION)s' % CONFIG
    ccmd('make && make install')
    #delete BUILD/ssl dir since we don't need it and it takes a lot of space 
    #this is only needed in local installtion of openssl
    ccmd('rm -rf %(BUILD_DIR)s/ssl' % CONFIG)

    #! set PKG_CONFIG_PATH to point to openssl .pc files (this is used by omniorb to locate correct openssl)
    os.environ['PKG_CONFIG_PATH']='%(BUILD_DIR)s/lib/pkgconfig/' % CONFIG

    ### (Step 2)omniORB ###
    os.chdir("%(SRC_DIR)s" % CONFIG)

    ccmd('wget %(SF_MIRROR)s/omniORB-%(OMNIORB_VERSION)s.tar.gz -O omniORB-%(OMNIORB_VERSION)s.tar.gz'  % CONFIG)

    ccmd('tar xzvf omniORB-%(OMNIORB_VERSION)s.tar.gz'  % CONFIG)

    os.chdir('omniORB-%(OMNIORB_VERSION)s' % CONFIG) 
    
    print 'Patching OMNIORB with GSI support'
    ccmd('wget http://cern.ch/diane/packages/%(OMNIORB_GSI_PATCH_TARBALL)s'%CONFIG)
    ccmd('tar xzvf %(OMNIORB_GSI_PATCH_TARBALL)s'%CONFIG)

    cfg_cmd = './configure --prefix=%(BUILD_DIR)s --with-openssl=%(BUILD_DIR)s --disable-static --disable-ipv6' % CONFIG
    print "Configuring omniORB-%(OMNIORB_VERSION)s" % CONFIG
    ccmd(cfg_cmd,'Fail to configure omniORB-%(OMNIORB_VERSION)s' % CONFIG )

    print 'Building omniORB-%(OMNIORB_VERSION)s' % CONFIG
    ccmd('make && make install')
    
    ### (Step 3) omniORBpy ###
    os.chdir("%(SRC_DIR)s" % CONFIG)

    ccmd('wget %(SF_MIRROR)s/omniORBpy-%(OMNIORBPY_VERSION)s.tar.gz -O omniORBpy-%(OMNIORBPY_VERSION)s.tar.gz'  % CONFIG)

    ccmd('tar xzvf omniORBpy-%(OMNIORBPY_VERSION)s.tar.gz'  % CONFIG)

    os.chdir('omniORBpy-%(OMNIORBPY_VERSION)s' % CONFIG) 

    cfg_cmd = './configure --prefix=%(BUILD_DIR)s --with-omniorb=%(BUILD_DIR)s --with-openssl=%(BUILD_DIR)s' % CONFIG   
    print "Configuring omniORBpy-%(OMNIORBPY_VERSION)s" % CONFIG
    ccmd(cfg_cmd,'Fail to configure omniORBpy-%(OMNIORBPY_VERSION)s' % CONFIG )

    print 'Building omniORBpy-%(OMNIORBPY_VERSION)s' % CONFIG
    ccmd('make && make install')

except OSError,e:
    print "Got an exception while trying to dw the source tarballs:\n\t",e

