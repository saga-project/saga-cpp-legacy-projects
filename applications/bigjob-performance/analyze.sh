#!/bin/bash

DATA_DIR=results
DATA_DIR_PROCESSED=results-processed

mkdir $DATA_DIR_PROCESSED

echo ""
echo "Analyze varying WU"


for host in s1; do

local_node="alamo"
	
data_file_redis=data_wu_redis_${host}.txt
data_file_redis_local=data_wu_redis_local_${host}.txt
data_file_advert=data_wu_advert_${host}.txt
data_file_zmq=data_wu_zmq_${host}.txt
data_file_diane=data_wu_diane_${host}.txt

echo "# WU  #Avg #Stddev #n" > ${DATA_DIR_PROCESSED}/$data_file_redis
echo "# WU  #Avg #Stddev #n" > ${DATA_DIR_PROCESSED}/$data_file_redis_local
echo "# WU  #Avg #Stddev #n" > ${DATA_DIR_PROCESSED}/$data_file_advert
echo "# WU  #Avg #Stddev #n" > ${DATA_DIR_PROCESSED}/$data_file_zmq
echo "# WU  #Avg #Stddev #n" > ${DATA_DIR_PROCESSED}/$data_file_diane

for i in 64 128 256 512 1024 2048; do
	echo "process ${data_file_redis}"
	averages=`cat ${DATA_DIR}-${host}/res* | grep redis://cyder | grep 8,8,${i} | awk -F, '{print $4-$5}' | awk -f analyze.awk`
	echo "${i}${averages}" >> ${DATA_DIR_PROCESSED}/$data_file_redis
done

for i in 64 128 256 512 1024 2048; do
	echo "process ${data_file_redis_local}"
	averages=`cat ${DATA_DIR}-${host}/res* | grep redis:///${local_node} | grep 8,8,${i} | awk -F, '{print $4-$5}' | awk -f analyze.awk`
	echo "${i}${averages}" >> ${DATA_DIR_PROCESSED}/$data_file_redis_local
done

for i in 64 128 256 512 1024 2048; do
	echo "process ${data_file_advert}"
	averages=`cat ${DATA_DIR}-${host}/res* | grep advert:// | grep 8,8,${i} | awk -F, '{print $4-$5}' | awk -f analyze.awk`
	echo "${i}${averages}" >> ${DATA_DIR_PROCESSED}/$data_file_advert
done

for i in 64 128 256 512 1024 2048; do
	echo "process ${data_file_zmq}"
	averages=`cat ${DATA_DIR}-${host}/res* | grep tcp:// | grep 8,8,${i} | awk -F, '{print $4-$5}' | awk -f analyze.awk`
	echo "${i}${averages}" >> ${DATA_DIR_PROCESSED}/$data_file_zmq
done

for i in 64 128 256 512 1024 2048; do
	echo "process ${data_file_diane}"
	averages=`cat ${DATA_DIR}-${host}/diane/res* | grep diane:// | grep 8,8,${i} | awk -F, '{print $4-$5}' | awk -f analyze.awk`
	echo "${i}${averages}" >> ${DATA_DIR_PROCESSED}/$data_file_diane
done


######################################################################
echo "Analyze scaling"
#tail --lines +2 data.data | awk '{print $4}' | awk -f ./analyse.awk
data_file_cores_redis=data_cores_redis_${host}.txt
data_file_cores_redis_local=data_cores_redis_local_${host}.txt
data_file_cores_advert=data_cores_advert_${host}.txt
data_file_cores_zmq=data_cores_zmq_${host}.txt
data_file_cores_diane=data_cores_diane_${host}.txt

echo "#Cores  #Avg #Stddev #n" > ${DATA_DIR_PROCESSED}/$data_file_cores_redis
echo "#Cores  #Avg #Stddev #n" > ${DATA_DIR_PROCESSED}/$data_file_cores_redis_local
echo "#Cores  #Avg #Stddev #n" > ${DATA_DIR_PROCESSED}/$data_file_cores_advert
echo "#Cores  #Avg #Stddev #n" > ${DATA_DIR_PROCESSED}/$data_file_cores_zmq
echo "#Cores  #Avg #Stddev #n" > ${DATA_DIR_PROCESSED}/$data_file_cores_diane


for i in 1 2 4 8 16; do
	echo "process ${data_file_cores_advert}"
	averages=`cat ${DATA_DIR}-${host}/res* | grep advert:// | grep ${i},8,$[${i}*8*4] | awk -F, '{print $4-$5}' | awk -f analyze.awk`	
	echo $[${i}*8]${averages}>>${DATA_DIR_PROCESSED}/$data_file_cores_advert
done

for i in 1 2 4 8 16; do
	echo "process ${data_file_cores_redis}"
	averages=`cat ${DATA_DIR}-${host}/res* | grep redis://cyder | grep ${i},8,$[${i}*8*4] | awk -F, '{print $4-$5}' | awk -f analyze.awk`	
	echo $[${i}*8]${averages}>>${DATA_DIR_PROCESSED}/$data_file_cores_redis
done

for i in 1 2 4 8 16; do
	echo "process ${data_file_cores_redis_local}"
	averages=`cat ${DATA_DIR}-${host}/res* | grep redis://${local_node} | grep ${i},8,$[${i}*8*4] | awk -F, '{print $4-$5}' | awk -f analyze.awk`	
	echo $[${i}*8]${averages}>>${DATA_DIR_PROCESSED}/$data_file_cores_redis_local
done

for i in 1 2 4 8 16; do
	echo "process ${data_file_cores_zmq}"
	averages=`cat ${DATA_DIR}-${host}/res* | grep tcp:// | grep ${i},8,$[${i}*8*4] | awk -F, '{print $4-$5}' | awk -f analyze.awk`	
	echo $[${i}*8]${averages}>>${DATA_DIR_PROCESSED}/$data_file_cores_zmq
done

for i in 1 2 4 8 16 32; do
	echo "process ${data_file_cores_diane}"
	averages=`cat ${DATA_DIR}-${host}/diane/res* | grep diane:// | grep ${i},8,$[${i}*8*4] | awk -F, '{print $4-$5}' | awk -f analyze.awk`	
	echo $[${i}*8]${averages}>>${DATA_DIR_PROCESSED}/$data_file_cores_diane
done

done
echo "Run Gnuplot"
gnuplot gnuplot.sh