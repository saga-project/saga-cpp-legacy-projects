#!/bin/sh
date && 
/bin/echo -n "Running on host: " &&
/bin/hostname && 

/bin/echo -n "Contents of input file 1: "
/bin/cat $2

/bin/echo -n "Contents of input file 2: "
/bin/cat $3
 
echo "Sleeping for $1 seconds..." && 
sleep $1 && 
date
