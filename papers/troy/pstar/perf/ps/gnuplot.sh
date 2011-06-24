#!/bin/sh
# set term postscript eps enhanced linewidth 1.42
#set term postscript eps enhanced linewidth 1.42 "Helvetica" 17 
set term postscript eps enhanced color linewidth 1.42 "Helvetica" 14 
#set term postscript eps monochrome linewidth 1.42 "Helvetica" 17 

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
set output "pilot-store-fileregistration.eps"       
set logscale x 2
#set logscale y 2                                 
set key
set xtics autofreq
set yrange [1:1050] 
#set xrange [2:16]
set xrange [0.99:17000]        

set ylabel "Runtime (in s)" font "Helvetica, 18"
set xlabel "Number of Files"  font "Helvetica, 18" 

plot "data.txt" using 1:2 title "Local Advert" with lp lt 8 linewidth 4,\
     "data.txt" using 1:5 title "Remote Advert" with lp  lt 4 linewidth 4,\
     "data.txt" using 1:8 title "Fast Advert" with lp  lt 5 linewidth 4
      
