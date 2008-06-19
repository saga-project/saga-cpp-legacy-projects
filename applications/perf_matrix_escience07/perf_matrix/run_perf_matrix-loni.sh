#!/bin/bash

# This scripts sets the required environment variables
# to run a SAGA application - in particular perf_matrix
# on gg101.cct.lsu.edu and gg201.cct.lsu.edu.
# All command line parameters are passed directly to 
# the perf_matrix application.
 
declare -x BOOST_LOCATION="/scratch/local/oweidner/packages/boost-1.33.1-xlc/"
declare -x POSTGRESQL_LOCATION="/scratch/local/oweidner/packages/postgresql-8.2.4/"
declare -x SAGA_LOCATION="/scratch/local/oweidner/packages/saga-latest-xlc/"
declare -x SQLITE3_LOCATION="/scratch/local/oweidner/packages/sqlite-3.4.0/"
declare -x XMLRPC_LOCATION="/scratch/local/oweidner/packages/xmlrpc++0.7/"

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$BOOST_LOCATION/lib 
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$POSTGRESQL_LOCATION/lib 
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$SAGA_LOCATION/lib 
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$SQLITE3_LOCATION/lib 


declare -x GLOBUS_FLAVOUR="vendorcc32dbgpthr"
declare -x GLOBUS_LOCATION="/usr/local/packages/globus/globus-4.0.3/"

. $GLOBUS_LOCATION/etc/globus-user-env.sh

./perf_matrix $*


