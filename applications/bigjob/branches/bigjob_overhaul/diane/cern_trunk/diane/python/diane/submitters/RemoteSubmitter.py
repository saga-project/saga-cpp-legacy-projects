#!/usr/bin/env ganga
#-*-python-*-
# author: andrea.dotti@cern.ch
# This Script implements a RemoteSubmitter using
# ganga's remote backend
# Please consult ganga's manual on usage of remote backend
# usage:
# diane-env -d ganga RemoteSubmitter.py --host="somehost.cern.ch" --username="my_name" [--prescript=any_additional_cmd] \
#           [--ganga_cmd="ganga_executable_full_path_on_remote_host"] [--ganga_dir="ganga_directory_for_remote_backend]

#from Ganga.GPIDev.Base import *
#from Ganga.GPIDev.Schema import *

from diane import getLogger
logger=getLogger('RemoteSubmitter')

from diane.submitters import Submitter
prog = Submitter()

prog.download=False
prog.parser.description="Submit worker agents to a remote host "+prog.parser.description

prog.parser.add_option("--host",dest="host",type="string",default='localhost',help="Set remote host name")
prog.parser.add_option("--username",dest="username",type="string",default='',help="Remote user name")
prog.parser.add_option("--prescript",dest="prescript",default=[],help="Additional script to be executed on remote host before applicaiton. Can be used several times",action="append")
prog.parser.add_option("--ganga_cmd",dest="gangacmd",help="Ganga command (with full path) on remote host")
prog.parser.add_option("--ganga_dir",dest="gangadir",help="Ganga remote directory for jobs")
prog.parser.add_option("--delay",dest="delay",type="int",default=0,help="delay in seconds in between the worker submission (to avoid spikes in the worker agent registration)")
prog.parser.add_option("--config_file",dest="config_file",help="Configuration file for defaults value of some parameters, if not specified use $DIANE_USER_WORKSAPCE/remotesubmitters_config.py")
prog.parser.add_option("--ssh_key",dest="ssh_key",help="SSH Key file. Needed to avoid ganga asking password for remote host. Refer to Ganga Remote backend for details.")
prog.parser.add_option("--key_type",dest="key_type",help="SSH Key type. Needed if ssh_key is specified. Refer to Ganga Remote backend for details.")
prog.parser.add_option("--nice",dest="nice",type="int",help="Nice level for job")

prog.initialize()

import time
for i in range(prog.options.N_WORKERS):
    #Parse configuration file
    remoteconfig={}
    if prog.options.config_file:
        execfile(prog.options.config_file,remoteconfig)
    else:
        try:
            from os import getenv,path
            basepath=getenv('DIANE_USER_WORKSPACE')
            fn=path.join(basepath,'remotesubmitters_config.py')
            execfile(fn,remoteconfig)
            logger.debug(str(remoteconfig))
        except Exception,e:
            logger.warning('Cannot find configuration file, assuming all parameters are passed via command line')
            logger.warning(str(e))
            pass
    #Configure GANGA job
    j = Job()
    j.backend=Remote()

    #Get username, first via commandline, then via conf file, finally use same as login user
    if prog.options.username:
        j.backend.username=prog.options.username
    elif remoteconfig.has_key('username'):
        j.backend.username=remoteconfig['username']
    else:
        from getpass import getuser
        j.backend.username=getuser()

    #Host where to run
    if prog.options.host:
        j.backend.host=prog.options.host
    else:
        try:
            j.backend.host=remoteconfig['host']
        except:
            logger.critical('Host specified neither in conf file nor via command line')
            raise

    #These are the commands to be executed on the remote host before starting the job
    try:
        j.backend.pre_script=remoteconfig['pre_script']
    except:
        j.backend.pre_script=[]
        
    j.backend.pre_script.extend(prog.options.prescript)

    #Check configuration for passwordless ganga
    if prog.options.ssh_key:
        j.backend.ssh_key=prog.options.ssh_key
    else:
        try:
            j.backend.ssh_key=remoteconfig['ssh_key']
        except:
            pass
    if prog.options.key_type:
        j.backend.key_type=prog.options.key_type
    else:
        try:
            j.backend.key_type=remoteconfig['key_type']
        except:
            pass

    #Remote GANGA configuration 
    if prog.options.gangacmd:
        j.backend.ganga_cmd=prog.options.gangacmd
    else:
        try:
            j.backend.ganga_cmd=remoteconfig['ganga_cmd']
        except:
            logger.critical('Configuration parameter ganga_cmd is mandatory')
            raise
    if prog.options.gangadir:
        j.backend.ganga_dir=prog.options.gangadir
    else:
        try:
            j.backend.ganga_dir=remoteconfig['ganga_dir']
        except:
            logger.critical('Configuration parameter ganga_dir is mandatory')
            raise
        
    #Remote backend is Local
    j.backend.remote_backend= Local()
    if prog.options.nice:
        j.backend.remote_backend.nice=prog.options.nice
    else:
        try:
            j.backend.remote_backend.nice=remoteconfig['nice']
        except:
            pass

    prog.submit_worker(j)
    if prog.options.delay:
        print "now sleeping for %d seconds "%prog.options.delay
        time.sleep(prog.options.delay)
    

