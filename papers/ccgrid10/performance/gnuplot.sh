#!/bin/sh
# set term postscript eps enhanced linewidth 1.42
#set term postscript eps enhanced linewidth 1.42 "Helvetica" 17 
set term postscript eps enhanced color linewidth 1.42 "Helvetica" 17 
#set term postscript eps enhanced linewidth 1.42 "Helvetica" 17 

set encoding iso_8859_1

#set title "Submission Times"
set ylabel "Runtime (in s)" #font "Helvetica, 20"
set xlabel ""
set size 0.8,0.8
set lmargin 9 
set bmargin 3
set rmargin 2
set tmargin 1
                 
##########################################################################
# NAMD Runtimes    
set output "namd_run.eps"       
#set logscale x 2
#set logscale x                                 
set key
set xtics autofreq
set yrange [70:380] 
#set xrange [2:16]
set xrange [7:33]        
#set xtics (2,4,8,16)
#set logscale x
set ylabel "Runtime (in s)" #font "Helvetica, 20"
set xlabel "Number of Cores"   

plot "namd_run.txt" using 1:2 title "LONI (QB)" with lp lt 1 linewidth 4,\
     "namd_run.txt" using 1:3 title "LONI (Poseidon)" with lp  lt 2 linewidth 4,\
     "namd_run.txt" using 1:4 title "Science Cloud" with lp  lt 3 linewidth 4,\
     "namd_run.txt" using 1:5 title "EC2 (m2.4xlarge)" with lp  lt 4 linewidth 4,\
     "namd_run.txt" using 1:6 title "EC2 (m1.large)" with lp  lt 5 linewidth 4  

##########################################################################
# Scenarios Times
# Workload 50 Jobs Effect of Offloading to Cloud 
#set output "setup_time_core.eps"
#set xrange [0:32]
#set yrange [0:600]      
#plot "setup_time_core.txt" using 1:2 title "EC2 (m1.large)" with lp,\
#     "setup_time_core.txt" using 1:4 title "Nimbus" with lp,\
#     "setup_time_core.txt" using 1:5 title "Poseidon" with lp 9
#
##########################################################################
# Setup Times

set nokey
#set xdata time
#set timefmt "%d.%m%y"
#set format x "%d.%m."
#set format y "%.0f"
set xrange [0.5:3.5] 
set yrange [0:700] 
#set boxwidth 0.8 relative
#set style fill solid 0.9
# set linestyle 1 lt 1 lw 50
set output "setup_time.eps"  
set xtics ("EC2" 1, "Nimbus" 2.0, "Poseidon" 3.0, "QB" 4.0)
#plot "setup_time.txt" using 1:2:(0.7) title "Setup Times" with boxes linetype 1  fs solid 0.25

set boxwidth 0.8 relative
set style fill solid 0.9
plot "setup_time.txt" using 1:2:3:(0.75)  title "Setup Times" with boxerrorbars linetype 1  fs solid 0.5