#1/bin/bash

export SAGA_LOCATION=/usr/local/packages/saga
export BOOST_LOCATION=/usr/local/packages/boost-1.3.1
export SOCI_LOCATION=/home/packages/soci-2.2.0/

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$SAGA_LOCATION/lib:$BOOST_LOCATION/lib:$SOCI_LOCATION/lib

g++ -o perf_matrix *.cpp -I$BOOST_LOCATION/include   \
                         -I$SAGA_LOCATION/include    \
                         -L$SAGA_LOCATION/lib        \
                         -L$BOOST_LOCATION/lib       \
                         -lsaga_engine               \
                         -lsaga_package_file         \
                         -lsaga_package_job          \
                         -lsaga_package_advert       \
                         -lboost_program_options-gcc
