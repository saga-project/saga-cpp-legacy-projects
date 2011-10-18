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

function main
{
  # run as long as we don't get signalled to stop
  if [ ! -f $SAGA_DEMO_STOP ]; then

    export SAGA_DEMO_TIME=`/bin/date "+%H:%M"`
    export SAGA_DEMO_DATE=`/bin/date "+%Y-%m-%d"`
    export SAGA_DEMO_HOME=$SAGA_DEMO_ROOT/demo-$SAGA_DEMO_DATE/$SAGA_DEMO_TIME
    export SAGA_DEMO_STDOUT=$SAGA_DEMO_HOME/stdout
    export SAGA_DEMO_STDERR=$SAGA_DEMO_HOME/stderr
    export SAGA_DEMO_OUTPUT=$SAGA_DEMO_HOME/

    export SAGA_MANDELBROT_PWD=$SAGA_DEMO_HOME

    export SAGA_DEMO_PREPREV=$SAGA_DEMO_PREV
    export SAGA_DEMO_PREV=$SAGA_DEMO_THIS
    export SAGA_DEMO_THIS=$SAGA_DEMO_TIME

    echo "==============================="
    echo "running demo in $SAGA_DEMO_HOME"
    echo "using $SAGA_MANDELBROT_INI"

    mkdir -p $SAGA_DEMO_HOME || die "cannot create demo home $SAGA_DEMO_HOME"
    cd       $SAGA_DEMO_HOME || die "cannot cd to  demo home $SAGA_DEMO_HOME"

    # prepare today link and set static links for current and last demo
    rm -f $SAGA_DEMO_ROOT/today
    ln -s $SAGA_DEMO_ROOT/demo-$SAGA_DEMO_DATE/ $SAGA_DEMO_ROOT/today
    touch $SAGA_DEMO_ROOT/today/last
    rm -f $SAGA_DEMO_ROOT/today/last
    touch $SAGA_DEMO_ROOT/today/current
    mv -f $SAGA_DEMO_ROOT/today/current $SAGA_DEMO_ROOT/today/last
    ln -s $SAGA_DEMO_HOME               $SAGA_DEMO_ROOT/today/current

    # re-create html for last demo, to fix 'next'
    if [ -d $SAGA_DEMO_ROOT/demo-$SAGA_DEMO_DATE/$SAGA_DEMO_PREV ]; then
      echo "re-creating html: demo2html.pl $SAGA_DEMO_ROOT/demo-$SAGA_DEMO_DATE/$SAGA_DEMO_PREV" \
                                         " $SAGA_DEMO_PREPREV $SAGA_DEMO_THIS"
      test -f $SAGA_DEMO_FAKE \
        || $SAGA_DEMO_ROOT/demo2html.pl $SAGA_DEMO_ROOT/demo-$SAGA_DEMO_DATE/$SAGA_DEMO_PREV \
                                        $SAGA_DEMO_PREPREV $SAGA_DEMO_THIS
    fi

    # some debug info
    date
    id
    grid-proxy-info
    echo "--------------------------------------"
    echo


    # this line expects mandelbrot_master to be installed in $SAGA_LOCATION/bin,
    # or elsewhere in $PATH
    cp $SAGA_MANDELBROT_INI $SAGA_DEMO_HOME
    test -f $SAGA_DEMO_FAKE \
      || $SAGA_LOCATION/bin/saga-run.sh mandelbrot_master 2> $SAGA_DEMO_STDERR > $SAGA_DEMO_STDOUT \
      || echo "demo failed"

    echo "demo done "
    echo "===============================" 


    # Alas, if globus adaptor are loaded, we can't  kill mandelbrot_client  Argh.
    killall -s 9 -v -w  mandelbrot_client

    # create html for the current demo - no 'next' nown yet
    echo "creating html: demo2html.pl $SAGA_DEMO_ROOT/demo-$SAGA_DEMO_DATE/$SAGA_DEMO_THIS $SAGA_DEMO_PREV -"
    test -f $SAGA_DEMO_FAKE \
      || $SAGA_DEMO_ROOT/demo2html.pl $SAGA_DEMO_ROOT/demo-$SAGA_DEMO_DATE/$SAGA_DEMO_THIS $SAGA_DEMO_PREV -

    # new demo starts shortly after the last one finished (1 min sleep)
    if [ -f $SAGA_DEMO_DELAY ]; then
      sleep `cat $SAGA_DEMO_DELAY`
    else
      /bin/sleep 60
    fi

    # relaunch ourself
    $SAGA_DEMO_ROOT/run_demo.sh

  else # SAGA_DEMO_STOP exists

    rm -f $SAGA_DEMO_STOP
    rm -f $SAGA_DEMO_LOCK

    echo "===============================" 
    echo "demo stopped "                   
    echo "===============================" 

  fi
}

export SAGA_DEMO_ROOT=$HOME/www/mandelbrot/demo
export SAGA_DEMO_LOG=$SAGA_DEMO_ROOT/demo.log
export SAGA_DEMO_LOCK=$SAGA_DEMO_ROOT/demo.lock
export SAGA_DEMO_STOP=$SAGA_DEMO_ROOT/demo.stop
export SAGA_DEMO_DELAY=$SAGA_DEMO_ROOT/demo.delay
export SAGA_DEMO_FAKE=$SAGA_DEMO_ROOT/demo.fake

export SAGA_MANDELBROT_INI=$SAGA_DEMO_ROOT/demo.ini

# the first run needs some 4extra setup
if test "$SAGA_DEMO_SCRIPT_ITERATION" != "yes"; then

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

  export SAGA_DEMO_PREPREV="-"
  export SAGA_DEMO_PREV="-"
  export SAGA_DEMO_THIS="-"

  export SAGA_DEMO_STARTUP=yes

  # don't run that init phase again
  export SAGA_DEMO_SCRIPT_ITERATION=yes
fi


main 2>&1 >> $SAGA_DEMO_LOG &

if test "$SAGA_DEMO_STARTUP" = "yes"; then
  echo ""
  echo " $0 is going into background mode."
  echo " Output will be captured in '$SAGA_DEMO_LOG'."
  echo " The script's pid is '$!'"
  echo " To cancel the demo, run 'stop_demo.sh'"
  echo ""
fi

