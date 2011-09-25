#!/bin/sh
# set term postscript eps enhanced linewidth 1.42
#set term postscript eps enhanced linewidth 1.42 "Helvetica" 17 
#set term postscript eps enhanced color linewidth 1.42 "Helvetica" 17 
#set term postscript eps monochrome linewidth 1.42 "Helvetica" 17 

#set term postscript eps enhanced linewidth 1.42 "Helvetica" 17 

set terminal pdf color enhanced dashed
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
set output "results-processed/bigjob-varying-wus.pdf"       
#set logscale x 2
#set logscale x                                 
set key
set xtics autofreq
set yrange [0:500] 
#set xrange [2:16]
set xrange [62:2050]        
#set xtics (2,4,8,16)
set logscale x 2
set ylabel "Runtime (in sec)" # font "Helvetica, 17"
set xlabel "Number of WUs"  #font "Helvetica,17" 

plot "results-processed/data_file_redis" using 1:2 title "Redis (Remote)" with lp lt 8 linewidth 4,\
     "results-processed/data_file_redis_local" using 1:2 title "Redis (Local)" with lp  lt 4 linewidth 4,\
     "results-processed/data_file_zmq" using 1:2 title "ZMQ" with lp  lt 5 linewidth 4
     
      
set output "results-processed/bigjob-varying-cores.pdf" 
set xlabel "Number of Cores"  #font "Helvetica,17"      
#set xlabel "Number of Replicas"  font "Helvetica,24"
#set ylabel "Runtime (in min)"   font "Helvetica,24"
set xrange [6:130]
#set logscale x 2 
#set xtics (2,4,8,16,32)
#set yrange [20:115] 
plot "results-processed/data_file_cores_redis" using 1:2 title "Redis (Remote)" with lp lt 8 linewidth 4,\
     "results-processed/data_file_cores_redis_local" using 1:2 title "Redis (Local)" with lp  lt 4 linewidth 4,\
     "results-processed/data_file_cores_zmq" using 1:2 title "ZMQ" with lp  lt 5 linewidth 4
#     "repex-azure.txt" using 1:5 title "extra-large (8 cores)" with lp  lt 3 linewidth 4