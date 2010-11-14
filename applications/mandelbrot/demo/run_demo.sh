#!/bin/bash

function die()
{
  echo $*;
  exit -1; 
}

while [ ! -f $SAGA_DEMO_ROOT/stop_demo ]
do

  export SAGA_LOCATION=$HOME/install
  export SAGA_DEMO_ROOT=$HOME/www/demo
  export SAGA_DEMO_TIME=`/bin/date "+%H:%M"`
  export SAGA_DEMO_DATE=`/bin/date "+%Y-%m-%d"`
  export SAGA_DEMO_HOME=$SAGA_DEMO_ROOT/$SAGA_DEMO_DATE/$SAGA_DEMO_TIME
  export SAGA_DEMO_STDOUT=$SAGA_DEMO_HOME/stdout
  export SAGA_DEMO_STDERR=$SAGA_DEMO_HOME/stderr
  export SAGA_DEMO_OUTPUT=$SAGA_DEMO_HOME/

  export SAGA_MANDELBROT_INI=$SAGA_DEMO_ROOT/mandelbrot.ini

  echo "===============================" >> $SAGA_DEMO_ROOT/index
  echo "running demo in $SAGA_DEMO_HOME" >> $SAGA_DEMO_ROOT/index
  echo "using $SAGA_MANDELBROT_INI"      >> $SAGA_DEMO_ROOT/index
  
  mkdir -p $SAGA_DEMO_HOME || die "cannot create demo home $SAGA_DEMO_HOME"
  cd       $SAGA_DEMO_HOME || die "cannot cd to  demo home $SAGA_DEMO_HOME"
  
  $SAGA_LOCATION/bin/saga-run.sh $HOME/mandelbrot/master/mandelbrot_master \
    2> $SAGA_DEMO_STDERR \
     > $SAGA_DEMO_STDOUT \
    || die "demo failed"
  
  echo "demo done "                      >> $SAGA_DEMO_ROOT/index
  echo "===============================" >> $SAGA_DEMO_ROOT/index
  
  # new demo starts shortly after the last one finished 
  /bin/sleep 60

done

rm -f $SAGA_DEMO_ROOT/stop_demo

echo "===============================" >> $SAGA_DEMO_ROOT/index
echo "demo stopped "                   >> $SAGA_DEMO_ROOT/index
echo "===============================" >> $SAGA_DEMO_ROOT/index

