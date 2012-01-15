#!/bin/bash

mkdir results-india
mkdir results-qb

echo "Sync India"
rsync -av --rsh=ssh india:/N/u/luckow/src/bigjob-performance/results/ results-india/

echo "Sync Alamo"
rsync -av --rsh=ssh alamo:/N/u/luckow/src/bigjob-performance/results/ results-alamo/

#echo "Sync QB"
#rsync -av --rsh=ssh qb:/home/luckow/src/bigjob-performance/results/ results-qb/

echo "Sync S1"
rsync -av --rsh=ssh sierra:/N/u/luckow/src/bigjob-performance/results/  results-s1/
rsync -av --rsh=ssh sierra:/N/u/luckow/src/bigjob-performance/diane/results/  results-s1/diane/