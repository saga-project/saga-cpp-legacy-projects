#!/bin/bash

for pl in `ls *.plot`; do
  gnuplot $pl
done

for ps in `ls *.eps`; do
  epstopdf $ps
done
