#!/bin/sh
# CUSTOMIZE
# set term postscript eps enhanced linewidth 1.42
#set term postscript eps enhanced linewidth 1.42 "Helvetica" 17 

set term postscript eps enhanced color linewidth 1.42 "Helvetica" 17 
#set term postscript eps enhanced linewidth 1.42 "Helvetica" 17 
                                      

set encoding iso_8859_1

set output "perf.eps"
#set title "Submission Times"
set ylabel "Runtime (in s)" #font "Helvetica, 20"
set xlabel ""
#set xtics rotate 1 
#set size 0.9,1.03
#set xtics ("GRAM 2" 1, "GRAM 4\n(C)" 2.1, "GRAM 4\n(Java)" 3.2, "JBS"  4.25, "JBS\n(op.)" 4.85, "MS" 5.75, "MS\n (op.)" 6.35)       

# SAGA/GRAM   
# SAGA/Migol
# SAGA/Migol w/ Initialization
# Runtime w/ CPR
# Runtime w CPR

#set xtics ("GRAM\nSubm." 1, "Migol\nSubm." 2.0, "Migol Subm.\n w/ init." 3.0, "Migol\nChpt Reg." 4.0, "Migol\nRecovery" 5.0)
set xtics ("SAGA/GRAM\nSubmission" 1, "SAGA/Migol\nSubmission" 2.0, "SAGA/Migol\n Subm. w/ init." 3.0, "SAGA/Migol\nRecovery" 4.0)

set size 0.8,0.8
set lmargin 9
set bmargin 3
set rmargin 2
set tmargin 1
                 
set nokey
#set xdata time
#set timefmt "%d.%m%y"
#set format x "%d.%m."
set format y "%.0f"
set xrange [0.5:4.7] 
set yrange [0:55] 
set boxwidth 0.8 relative
set style fill solid 0.9
# set linestyle 1 lt 1 lw 50
#"data.txt" using 1:2:6 title "Job Startup Time" with boxes linetype 1  fs solid 0.25,\
   
set output "perf_submission.eps" 
#plot "data.txt" using 1:2:3  title "Submission Time" with boxes linetype 1  fs solid 0.5  
plot "data.txt" using 1:2:3:(0.75)  title "Submission Time" with boxerrorbars linetype 1  fs solid 0.5 

    #"data.txt" using 1:4:6 title "Resource Creation" with boxes linetype 1  fs solid 0.5
    # "data.txt" using 1:5:6 title "Delegation on Demand" with boxes fs pattern

# set output "perf_runtime.eps" 
# set xtics ("NAMD runtime\n (w/o SAGA/Migol) "  1, "NAMD runtime\n (with SAGA/Migol)" 2)  
# set yrange [0:30]
# set xrange [0.5:2.5]   
# set ylabel "Runtimes (in min)" font "Helvetica, 24" 
# plot "data2.txt" using 1:2:3  title "Submission Time" with boxes linetype 1  fs solid 0.5
         
      
set output "perf_monitoring.eps"       
unset xtics 
set xtics (20,40,60,80,100,120,140,160,180,200,220,240)
#set logscale x 
set xrange [0:260]       
set yrange [20:23]  
set ytics (20,21,22,23)
#set logscale x 2
set ylabel "Runtime (in min)" #font "Helvetica, 20" 
set xlabel "Monitoring Intervall (in s)"    
f(x)=21.3                           
set key
plot "data-monitoring.txt" using 1:2  title "With Migol" with lp 13,\
     f(x) title "Without Migol" with line  14
     
set output "perf_remd.eps"       
#set logscale x 2
#set logscale x                                 
set key
set xtics autofreq
set yrange [3:12] 
#set xrange [2:16]
set xrange [2:16]        
set xtics (2,4,8,16)
set logscale x
set ylabel "Runtime (in min)" #font "Helvetica, 20"
set xlabel "Number Replica Processes"   
plot "data-remd.txt" using 1:4 title "SAGA/Migol (QB/Poseidon)" with lp,\
     "data-remd.txt" using 1:5 title "SAGA/GRAM  (QB/Poseidon)" with lp,\
     "data-remd.txt" using 1:2 title "SAGA/Migol (QB)" with lp 9,\
     "data-remd.txt" using 1:3 title "SAGA/GRAM  (QB)" with lp 14  

# plot "data-remd.txt" using 1:7 title "SAGA/Migol (QB/Poseidon/Eric)" with lp,\
#      "data-remd.txt" using 1:6 title "SAGA/GRAM (QB/Poseidon/Eric)" with lp,\
#      "data-remd.txt" using 1:2 title "SAGA/Migol (QB)" with lp,\
#      "data-remd.txt" using 1:3 title "SAGA/GRAM  (QB)" with lp     