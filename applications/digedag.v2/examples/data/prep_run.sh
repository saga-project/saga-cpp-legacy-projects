#!/bin/sh 

if test "x$MONTAGE_LOCATION" = "x"; then
  echo "MONTAGE_LOCATION not set"
  exit -1
fi

if ! test -d $MONTAGE_LOCATION/bin; then
  echo "MONTAGE_LOCATION/bin does not exist"
  exit -2
fi

src=$1
data=`pwd`

if test "x$src" = "x"; then
  src=m101_2mass_1.0_1.0_0.000278
fi

if ! test -d $data/$src/data; then
  echo data directory not found: $data/$src/data
  exit -3
fi

echo "preparing $data/$src/data"
read -p "continue? <Y|n> : " answer

if test "x$answer" = "x"; then
  answer=y
fi

if ! test "x$answer" = "xy"; then
  echo "abort ($answer)"
  exit -4
fi


cd /tmp

rm -f 0
ln -s $data/$src/data 0

cd 0

rm -f bin
ln -s $MONTAGE_LOCATION/bin .

cd $data

