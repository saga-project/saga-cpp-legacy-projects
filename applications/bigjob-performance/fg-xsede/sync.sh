#!/bin/bash

DIR=`date "+%Y%m%d-%H%M"`
mkdir $DIR

#HOST= "kraken"
#echo "Sync ${HOST}"
#mkdir $HOST
#/usr/local/globus-5.2.0/bin/gsiscp $HOST:/lustre/scratch/aluckow/bigjob/ $DIR/$HOST

#HOST="hotel.futuregrid.org"
#echo "Sync ${HOST}"
#mkdir ${DIR}/$HOST
#scp -r ${HOST}:/scratch/gpfs/luckow/bigjob/ ${DIR}/${HOST}

HOST="queenbee.loni.org"
echo "Sync ${HOST}"
mkdir ${DIR}/$HOST
#rsync -av --rsh=ssh $HOST:/work/luckow/bigjob/ $DIR/$HOST
scp -r  ${HOST}:/work/luckow/bigjob/ ${DIR}/${HOST}

for i in `ls ${DIR}`; do

RESULT=`find $i | xargs grep real`

done
