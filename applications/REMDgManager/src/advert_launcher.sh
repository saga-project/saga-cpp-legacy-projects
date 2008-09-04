#!/bin/bash

. ~/.bashrc

echo "load modules"
module unload mvapich2
module unload mvapich
module swap pgi intel
module load mvapich
module load python/2.5.2

echo "run advert launcher"
python `dirname $0`/advert_launcher.py $*
