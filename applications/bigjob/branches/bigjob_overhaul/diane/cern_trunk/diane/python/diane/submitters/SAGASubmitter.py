#!/usr/bin/env ganga
#-*-python-*-
#
# This script may be placed in ~/diane/submitters (it will then take precedence over any submitters with the same name contained in the release tree).
#
# usage:
# diane-submitter SAGA [options]
#
# print all available options: diane-submitter SAGA -h
#

import sys

from diane import getLogger
logger=getLogger('SAGASubmitter')

from diane.submitters import Submitter
prog = Submitter()

prog.download=False
prog.parser.description="Submit worker agents using SAGA. "+prog.parser.description

# These parameters are required for the SAGA backend
prog.parser.add_option("--jobservice-url",type="string",default="",help="the remote job service url (e.g. gram://qb1.loni.org/jobmanager-pbs)")
prog.parser.add_option("--filesystem-url",type="string",default="",help="the remote filesystem root (e.g. gsiftp://qb1.loni.org/work/oweidner/diane-worker/)")
prog.parser.add_option("--allocation",type="string",default="",help="the allocation to be used for job accounting")
prog.parser.add_option("--delay",dest="delay",type="int",default=0,help="delay in seconds in between the worker submission (to avoid spikes in the worker agent registration)")


# this wrapper will start a number of worker agents using ssh hosts specified by $PBS_NODEFILE
# it assumes, however, that all the nodes are connected by a shared file system 
sh_download_multinode_wrapper = '''#!/bin/sh

wget %(DIANE_DOWNLOAD_URL)s/diane-install
python ./diane-install --download-url=%(DIANE_DOWNLOAD_URL)s --prefix=$PWD/diane %(DIANE_VERSION)s
BINDIR=$PWD/diane/install/%(DIANE_VERSION)s/bin

# a case of a shared filesystem between the worker nodes the submitter host 
#BINDIR=%(DIANE_TOP)s/bin

wids=''

for node in `cat $PBS_NODEFILE`; 
do 

pwd

# create a workdir
wdir=`mktemp -d ${PWD}/tmp.workdir.XXXXXXX`

echo 'spawning worker to ' $node ' workdir ' $wdir

# copy the required input files to the subdirectory first
# TODO: having hardcoded file names here is suboptimal, in the future this list should be created automatically
cp MasterOID ganga_job_uuid diane_worker_config-* $wdir

echo "ls -l $wdir"
ls -l $wdir

# start a worker agent in the background, internally switching to the workdir
nohup ssh $node $BINDIR/diane-worker-start --workdir $wdir $* > $wdir-stdouterr.txt 2>&1 &

wids=$wids' '$!

done

# let's wait for all ssh processes in the 'wids' list to finish

mustwait=1 

while [ $mustwait -eq 1 ];
do

willwait=0

sleep 5

echo "checking $wids"
for wpid in $wids; 
do
echo "kill -0 $wpid;"
if kill -0 $wpid;
then
echo "$wpid still alive"
willwait=1
fi;
done;

echo

mustwait=$willwait

done;

echo "quitting the multinode wrapper script"

'''

prog.wrapper = sh_download_multinode_wrapper

prog.initialize()

# Parameter checking
if len(prog.options.jobservice_url) < 1 :
    sys.exit("ERROR: the --jobservice-url paramter is mandatory.")

if len(prog.options.filesystem_url) < 1 :
    sys.exit("ERROR: the --filesystem-url paramter is mandatory.")


import time
for i in range(prog.options.N_WORKERS):
    #Configure GANGA job
    j = Job()
    j.backend=SAGA()

    # fix more multinode submitter
    j.outputsandbox.append('*-stdouterr.txt') # make sure we get back all of the output of the workernodes

    j.backend.jobservice_url = prog.options.jobservice_url 
    j.backend.filesystem_url = prog.options.filesystem_url

    if len(prog.options.allocation) > 1 :
    	j.backend.allocation = prog.options.allocation

    prog.submit_worker(j)
    if prog.options.delay:
        print "now sleeping for %d seconds "%prog.options.delay
        time.sleep(prog.options.delay)
    
