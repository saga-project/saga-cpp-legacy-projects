#!/bin/bash

# Cleaning old temp & output files
rm /N/u/pmantha/output/*
rm /N/u/pmantha/temp/*

# Create working directory
mkdir /N/u/pmantha/class_sc11/MapReduce/applications/wordcount/agent

#Executing MapReduce application with all the inputs.
# use python wordcountApp.py --help for help
# if you use -p option then make sure you have password less login enabled to the target location.

echo " Starting MapReduce"
k=`expr 64 \* 1024 \* 1024`
echo $k

python wordcountApp.py -i "file://localhost//N/u/pmantha/data" -o  "file://localhost/N/u/pmantha/output" -t "file://localhost/N/u/pmantha/temp" -b 8 -c $k -m "/N/u/pmantha/class_sc11/MapReduce/applications/wordcount/wordcount_map_partition.py" -r "/N/u/pmantha/class_sc11/MapReduce/applications/wordcount/wordcount_reduce.py" -a "advert://advert.cct.lsu.edu:8080" -u "pbs-ssh://localhost" -n 2 -w 40 -s 8 -d "/N/u/pmantha/class_sc11/MapReduce/applications/wordcount/agent" -q None -x None -y None -e 1
