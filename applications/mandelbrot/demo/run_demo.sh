#!/bin/bash

function die()
{
  echo $*
  echo "==============================="
  exit -1 ; 
}


if [ -z "$SAGA_LOCATION" ]; then
  die "SAGA_LOCATION needs to be set"
fi

export SAGA_DEMO_ROOT=$HOME/saga/mandelbrot/demo
export SAGA_DEMO_LOG=$SAGA_DEMO_ROOT/demo.log
export SAGA_DEMO_LOCK=$SAGA_DEMO_ROOT/demo.lock
export SAGA_DEMO_STOP=$SAGA_DEMO_ROOT/demo.stop
export SAGA_DEMO_DELAY=$SAGA_DEMO_ROOT/demo.delay

export SAGA_MANDELBROT_INI=$SAGA_DEMO_ROOT/demo.ini

# have only one demo script running at any time
if [ -f $SAGA_DEMO_LOCK ]; then
  echo ""
  echo " Found lock file at $SAGA_DEMO_LOCK - demo is already running" 
  echo ""
  exit -1
else
  touch $SAGA_DEMO_LOCK

  # stop signal MUST be stale here
  rm -f $SAGA_DEMO_STOP 
fi

preprev="-"
prev="-"
this="-"

{
  # run as long as we don't get signalled to stop
  while [ ! -f $SAGA_DEMO_STOP ]
  do

    export SAGA_DEMO_TIME=`/bin/date "+%H:%M"`
    export SAGA_DEMO_DATE=`/bin/date "+%Y-%m-%d"`
    export SAGA_DEMO_HOME=$SAGA_DEMO_ROOT/demo-$SAGA_DEMO_DATE/$SAGA_DEMO_TIME
    export SAGA_DEMO_STDOUT=$SAGA_DEMO_HOME/stdout
    export SAGA_DEMO_STDERR=$SAGA_DEMO_HOME/stderr
    export SAGA_DEMO_OUTPUT=$SAGA_DEMO_HOME/

    export SAGA_MANDELBROT_PWD=$SAGA_DEMO_HOME

    preprev=$prev
    prev=$this
    this=$SAGA_DEMO_TIME

    echo "==============================="
    echo "running demo in $SAGA_DEMO_HOME"
    echo "using $SAGA_MANDELBROT_INI"

    mkdir -p $SAGA_DEMO_HOME || die "cannot create demo home $SAGA_DEMO_HOME"
    cd       $SAGA_DEMO_HOME || die "cannot cd to  demo home $SAGA_DEMO_HOME"

    # re-create html for last demo, to fix 'next'
    if [ -d $SAGA_DEMO_ROOT/demo-$SAGA_DEMO_DATE/$prev ]; then
      $SAGA_DEMO_ROOT/demo2html.pl $SAGA_DEMO_ROOT/demo-$SAGA_DEMO_DATE/$prev $preprev $this
    fi

    # this line expects mandelbrot_master to be installed in $SAGA_LOCATION/bin,
    # or elsewhere in $PATH
    cp $SAGA_MANDELBROT_INI $SAGA_DEMO_HOME
    $SAGA_LOCATION/bin/saga-run.sh mandelbrot_master 2> $SAGA_DEMO_STDERR > $SAGA_DEMO_STDOUT \
      || echo "demo failed"

    echo "demo done "
    echo "===============================" 

    # create html for the current demo - no 'next' nown yet
    $SAGA_DEMO_ROOT/demo2html.pl $SAGA_DEMO_ROOT/demo-$SAGA_DEMO_DATE/$this $prev -

    # new demo starts shortly after the last one finished (1 min sleep)
    if [ -f $SAGA_DEMO_DELAY ]; then
      sleep `cat $SAGA_DEMO_DELAY`
    else
      /bin/sleep 60
    fi

  done

  rm -f $SAGA_DEMO_STOP
  rm -f $SAGA_DEMO_LOCK

  echo "===============================" 
  echo "demo stopped "                   
  echo "===============================" 


} 2>&1 > $SAGA_DEMO_LOG &

echo ""
echo " $0 is going into background mode."
echo " Output will be captured in '$SAGA_DEMO_LOG'."
echo " The script's pid is '$!'"
echo " To cancel the demo, run 'stop_demo.sh'"
echo ""
