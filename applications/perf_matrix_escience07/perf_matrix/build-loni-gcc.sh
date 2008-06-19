#!/bin/bash

export  SAGA_LOCATION=/home/oweidner/scratch/packages/saga-latest/
export BOOST_LOCATION=/home/oweidner/scratch/packages/boost-1.33.1

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$SAGA_LOCATION/lib:$BOOST_LOCATION/lib:$SOCI_LOCATION/lib

g++ -o perf_matrix *.cpp -I$BOOST_LOCATION/include   \
                         -I$SAGA_LOCATION/include    \
                         -L$SAGA_LOCATION/lib        \
                         -L$BOOST_LOCATION/lib       \
                         -lsaga_engine               \
                         -lsaga_package_file         \
                         -lsaga_package_job          \
                         -lsaga_package_advert       \
                         -lboost_program_options-gcc \
                         -Wl,-G -Wl,-brtl
