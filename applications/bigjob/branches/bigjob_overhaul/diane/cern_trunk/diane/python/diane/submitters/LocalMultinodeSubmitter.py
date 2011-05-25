#!/usr/bin/env ganga
#-*-python-*-

if __name__ == '__main__':

    from diane.submitters import Submitter
    prog = Submitter()
    prog.parser.add_option('--node-number',type="int",default=1,help="Use a number of worker nodes to run one worker agent on each.")

    sh_download_multinode_wrapper = '''#!/bin/sh

#wget %(DIANE_DOWNLOAD_URL)s/diane-install
#python ./diane-install --download-url=%(DIANE_DOWNLOAD_URL)s --prefix=$PWD/diane %(DIANE_VERSION)s
#BINDIR=$PWD/diane/install/%(DIANE_VERSION)s/bin

BINDIR=%(DIANE_TOP)s/bin

wids=''

for node in ###NODES###; 
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

    prog.parser.description = "Submit worker agents locally to mulitple nodes (possibly to different ones -> currently we use 'localhost' a number of times as a demponstration). You must have ssh keys setup correctly to be able to login without password. "+prog.parser.description
    prog.initialize()

    # let's expand our special template here
    prog.wrapper = prog.wrapper.replace('###NODES###','localhost '*prog.nnodes)
    

    for i in range(prog.options.N_WORKERS):
        j = Job()
        j.backend=Local()
        j.outputsanbox.apppend('*-stdouterr.txt') # make sure we get back all of the output of the workernodes
        prog.submit_worker(j)

