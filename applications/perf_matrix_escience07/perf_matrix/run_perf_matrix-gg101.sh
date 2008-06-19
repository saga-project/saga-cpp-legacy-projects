#!/bin/bash

# This scripts sets the required environment variables
# to run a SAGA application - in particular perf_matrix
# on gg101.cct.lsu.edu and gg201.cct.lsu.edu.
# All command line parameters are passed directly to 
# the perf_matrix application.
 
export GLOBUS_LOCATION=/usr/local/globus/globus-4.0.3
. $GLOBUS_LOCATION/etc/globus-user-env.sh

export SAGA_LOCATION=/usr/local/packages/saga
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$SAGA_LOCATION/lib

export BOOST_LOCATION=/usr/local/packages/boost-1.3.1
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$BOOST_LOCATION/lib

export SOCI_LOCATION=/home/packages/soci-2.2.0/
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$SOCI_LOCATION/lib

export SQLITE_LOCATION=/home/packages/sqlite-3.3.13/
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$SQLITE_LOCATION/lib

export PG_LOCATION=/home/packages/postgresql-8.2.3/
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$PG_LOCATION/lib

export JAVA_HOME=/usr/java/jdk1.5.0_10
export PATH=$JAVA_HOME/bin:$PATH

./perf_matrix $*


