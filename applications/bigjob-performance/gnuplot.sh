#!/bin/sh
# set term postscript eps enhanced linewidth 1.42
#set term postscript eps enhanced linewidth 1.42 "Helvetica" 17 
#set term postscript eps enhanced color linewidth 1.42 "Helvetica" 17 
#set term postscript eps monochrome linewidth 1.42 "Helvetica" 17 

#set term postscript eps enhanced linewidth 1.42 "Helvetica" 17 

set terminal pdf color enhanced dashed linewidth 4 font "Helvetica, 10"
set palette gray

#set encoding iso_8859_1

#set title "Submission Times"
set ylabel "Runtime (in s)" #font "Helvetica, 20"
set xlabel ""
#set size 0.8,0.8
#set lmargin 9 
#set bmargin 3
#set rmargin 2
#set tmargin 1
                 
##########################################################################
# NAMD Runtimes    
set output "results-processed/bigjob-varying-wus-alamo.pdf"       
#set logscale x 2
#set logscale x                                 
set key
set xtics autofreq
set yrange [0:3300] 
#set xrange [2:16]
set xrange [57:2200]        
#set xtics (2,4,8,16)
set logscale x 2
set ylabel "Runtime (in sec)"  #font "Helvetica, 10"
set xlabel "Number of WUs"  #font "Helvetica, 10" 

set pointsize 2
set key left

plot "results-processed/data_wu_advert_alamo.txt" using 1:2 title "Advert" with lp ps 3,\
	 "results-processed/data_wu_redis_alamo.txt" using 1:2 title "Redis (Remote)" with lp,\
     "results-processed/data_wu_redis_local_alamo.txt" using 1:2 title "Redis (Local)" with lp ps 1,\
     "results-processed/data_wu_zmq_alamo.txt" using 1:2 title "ZMQ" with lp,\
	 "results-processed/data_wu_diane_s1.txt" using 1:2 title "DIANE" with lp ps 1

set output "results-processed/bigjob-varying-wus-alamo-noadvert.pdf"   
set yrange [0:500]   
plot "results-processed/data_wu_redis_alamo.txt" using 1:2 title "Redis (Remote)" with lp,\
     "results-processed/data_wu_redis_local_alamo.txt" using 1:2 title "Redis (Local)" with lp ps 1,\
     "results-processed/data_wu_zmq_alamo.txt" using 1:2 title "ZMQ" with lp,\
	 "results-processed/data_wu_diane_s1.txt" using 1:2 title "DIANE" with lp ps 1

      
set output "results-processed/bigjob-varying-cores-alamo.pdf" 
set xlabel "Number of Cores"  #font "Helvetica,17"      
#set xlabel "Number of Replicas"  font "Helvetica,24"
#set ylabel "Runtime (in min)"   font "Helvetica,24"
set yrange [0:800] 
set xrange [7.5:140]
#set logscale x 2 
#set xtics (2,4,8,16,32)
#set yrange [20:115] 
plot "results-processed/data_cores_advert_alamo.txt" using 1:2 title "Advert" with lp ps 3,\
     "results-processed/data_cores_redis_alamo.txt" using 1:2 title "Redis (Remote)" with lp,\
     "results-processed/data_cores_redis_local_alamo.txt" using 1:2 title "Redis (Local)" with lp ps 1,\
     "results-processed/data_cores_zmq_alamo.txt" using 1:2 title "ZMQ" with lp,\
	 "results-processed/data_cores_diane_s1.txt" using 1:2 title "DIANE" with lp ps 1
#     "repex-azure.txt" using 1:5 title "extra-large (8 cores)" with lp  lt 3 linewidth 4

set output "results-processed/bigjob-varying-cores-alamo-noadvert.pdf" 
set yrange [0:300] 
plot "results-processed/data_cores_redis_alamo.txt" using 1:2 title "Redis (Remote)" with lp,\
     "results-processed/data_cores_redis_local_alamo.txt" using 1:2 title "Redis (Local)" with lp ps 1,\
     "results-processed/data_cores_zmq_alamo.txt" using 1:2 title "ZMQ" with lp,\
	 "results-processed/data_cores_diane_s1.txt" using 1:2 title "DIANE" with lp ps 1