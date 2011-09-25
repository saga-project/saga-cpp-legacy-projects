#!/bin/bash

DATA_DIR=results
DATA_DIR_PROCESSED=results-processed

mkdir $DATA_DIR_PROCESSED

echo ""
echo "Analyze varying WU"

data_file_redis=data_wu_redis.txt
data_file_redis_local=data_wu_redis_local.txt
data_file_advert=data_wu_advert.txt
data_file_zmq=data_wu_zmq.txt

echo "# WU  #Avg #Stddev #n" > ${DATA_DIR_PROCESSED}/data_file_redis
echo "# WU  #Avg #Stddev #n" > ${DATA_DIR_PROCESSED}/data_file_redis_local
echo "# WU  #Avg #Stddev #n" > ${DATA_DIR_PROCESSED}/data_file_advert
echo "# WU  #Avg #Stddev #n" > ${DATA_DIR_PROCESSED}/data_file_zmq

for i in 64 128 256 512 1024 2048; do
	averages=`cat ${DATA_DIR}/* | grep redis://cyder | grep 8,8,${i} | awk -F, '{print $4-$5}' | awk -f analyze.awk`
	echo "${i}${averages}" >> ${DATA_DIR_PROCESSED}/data_file_redis
done

for i in 64 128 256 512 1024 2048; do
	averages=`cat ${DATA_DIR}/* | grep redis://i136 | grep 8,8,${i} | awk -F, '{print $4-$5}' | awk -f analyze.awk`
	echo "${i}${averages}" >> ${DATA_DIR_PROCESSED}/data_file_redis_local
done

for i in 64 128 256 512 1024 2048; do
	averages=`cat ${DATA_DIR}/* | grep advert:// | grep 8,8,${i} | awk -F, '{print $4-$5}' | awk -f analyze.awk`
	echo "${i}${averages}" >> ${DATA_DIR_PROCESSED}/data_file_advert
done

for i in 64 128 256 512 1024 2048; do
	averages=`cat ${DATA_DIR}/* | grep tcp://i136 | grep 8,8,${i} | awk -F, '{print $4-$5}' | awk -f analyze.awk`
	echo "${i}${averages}" >> ${DATA_DIR_PROCESSED}/data_file_zmq
done

######################################################################


# echo ""
echo "Analyze scaling"
#tail --lines +2 data.data | awk '{print $4}' | awk -f ./analyse.awk
data_file_cores_redis=data_cores_redis.txt
data_file_cores_redis_local=data_cores_redis_local.txt
data_file_cores_advert=data_cores_advert.txt
data_file_cores_zmq=data_cores_zmq.txt

echo "#Cores  #Avg #Stddev #n" > ${DATA_DIR_PROCESSED}/data_file_cores_redis
echo "#Cores  #Avg #Stddev #n" > ${DATA_DIR_PROCESSED}/data_file_cores_redis_local
echo "#Cores  #Avg #Stddev #n" > ${DATA_DIR_PROCESSED}/data_file_cores_advert
echo "#Cores  #Avg #Stddev #n" > ${DATA_DIR_PROCESSED}/data_file_cores_zmq

for i in 1 2 4 8 16; do
	averages=`cat results/* | grep advert:// | grep ${i},8,$[${i}*8*4] | awk -F, '{print $4-$5}' | awk -f analyze.awk`	
	echo $[${i}*8]${averages}>>${DATA_DIR_PROCESSED}/data_file_cores_advert
done

for i in 1 2 4 8 16; do
	averages=`cat results/* | grep redis://cyder | grep ${i},8,$[${i}*8*4] | awk -F, '{print $4-$5}' | awk -f analyze.awk`	
	echo $[${i}*8]${averages}>>${DATA_DIR_PROCESSED}/data_file_cores_redis
done

for i in 1 2 4 8 16; do
	averages=`cat results/* | grep redis://i136 | grep ${i},8,$[${i}*8*4] | awk -F, '{print $4-$5}' | awk -f analyze.awk`	
	echo $[${i}*8]${averages}>>${DATA_DIR_PROCESSED}/data_file_cores_redis_local
done

for i in 1 2 4 8 16; do
	averages=`cat results/* | grep tcp:// | grep ${i},8,$[${i}*8*4] | awk -F, '{print $4-$5}' | awk -f analyze.awk`	
	echo $[${i}*8]${averages}>>${DATA_DIR_PROCESSED}/data_file_cores_zmq
done

echo "Run Gnuplot"
gnuplot gnuplot.sh