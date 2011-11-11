#!/bin/bash

# Cleaning old temp & output files
rm /N/u/pmantha/data/reduce*
rm /N/u/pmantha/data/*txt*-*
rm /N/u/pmantha/output/*

# Create working directory ( Need to create manually for 1.5.3 version of saga
# Fixed in 1.6 version
mkdir /N/u/pmantha/pmantha/MapReduce_Python/source/agent

#Executing MapReduce application with all the inputs.
# use python MapReduceApp.py --help for help
# if you use -p option then make sure you have password less login enabled to the target location.

echo " Starting MapReduce"
k=`expr 64 \* 1024 \* 1024`
echo $k

python MRnoBJApp.py -i "file://localhost//N/u/pmantha/data" -o  "file://localhost/N/u/pmantha/output" -t "file://localhost/N/u/pmantha/data" -b 8 -c $k -m "/N/u/pmantha/pmantha/MapReduce_Python/source/applications/wordcount/wordcount_map_partition.py" -r "/N/u/pmantha/pmantha/MapReduce_Python/source/applications/wordcount/wordcount_reduce.py" -u "ssh://localhost" -w "/N/u/pmantha/pmantha/MapReduce_Python/source/agent" -k 1
