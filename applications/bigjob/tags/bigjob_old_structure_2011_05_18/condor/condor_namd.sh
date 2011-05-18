#!/bin/bash -l

/bin/date

# Am I a leading process?
if [[ $_CONDOR_PROCNO -ne 0 ]] ; then
	exit 0
fi

# If HOME is not set, then set HOME and execute this script again.
# (Bash configuration files use HOME to set environment correctly).
if [[ -z $HOME ]] ; then
	export HOME=/home/`/usr/bin/id -un`
	exec $_CONDOR_SCRATCH_DIR/condor_exec.exe $@
fi


PWD=`/bin/pwd`
MACHINES=`mktemp -p $PWD`
ABS_PATH=`/usr/bin/which namd2`

HOSTS=`grep AllRemoteHosts $_CONDOR_JOB_AD`
HOSTS=${HOSTS##AllRemoteHosts = \"}
HOSTS=${HOSTS%\"}
HOSTS=`echo $HOSTS | tr , "\n" | sed "s/^[^@]*@//"`
for H in $HOSTS; do
	echo $H >> $MACHINES
	echo $H >> $MACHINES
	echo $H >> $MACHINES
	echo $H >> $MACHINES
done

let N_NODES=4*$_CONDOR_NPROCS

/bin/date
mpirun -machinefile $MACHINES -np $N_NODES $ABS_PATH $@
rm -f $MACHINES
/bin/date
