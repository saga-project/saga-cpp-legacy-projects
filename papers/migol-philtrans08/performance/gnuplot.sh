#!/bin/sh
# CUSTOMIZE
# set term postscript eps enhanced linewidth 1.42
#set term postscript eps enhanced linewidth 1.42 "Helvetica" 17 

set term postscript eps enhanced color linewidth 1.42 "Helvetica" 17 
                                      

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
set yrange [0:50] 
set boxwidth 0.8 relative
set style fill solid 0.9
#set linestyle 1 lt 1 lw 50
#"data.txt" using 1:2:6 title "Job Startup Time" with boxes linetype 1  fs solid 0.25,\
   
set output "perf_submission.eps" 
plot "data.txt" using 1:2:3  title "Submission Time" with boxes linetype 1  fs solid 0.5
    #"data.txt" using 1:4:6 title "Resource Creation" with boxes linetype 1  fs solid 0.5
    # "data.txt" using 1:5:6 title "Delegation on Demand" with boxes fs pattern
 
set output "perf_glidein.eps"
set xtics ("No Glide-In" 1, "1 Glide-In" 2.0, "2 Glide-Ins" 3.0, "4 Glide-Ins" 4.0)
set yrange [0:120]  
set xrange [0.5:2.5]    
set style fill solid 0.9   
set ylabel "Runtime (in min)"  
set ylabel "Time-to-Completion (in min)"     
plot "data_remd_glidein_16re.txt" using 1:2:3:(0.75)  title "Runtime" with boxerrorbars linetype 1  fs solid 0.5


set output "perf_distributed_number_replica.eps"
set xtics ("Abe" 1, "Abe/Ranger" 2.1, "Abe/QB/Ranger" 3.2)
set yrange [0:300]    
set style fill solid 0.9 
set xrange [0.5:3.8]      

plot "data_remd_distributed_numberre.txt" using 2:3:4:(0.75)  title "Runtime 64 Exchanges" with boxerrorbars linetype 1  fs solid 0.5
  
set output "perf_distributed_size_replica.eps"
set xtics ("QB" 1, "Abe/QB" 2.1, "Abe/QB/Ranger" 3.2)
set yrange [0:100]    
set style fill solid 0.9 
set xrange [0.5:3.8]    
plot "data_remd_distributed_sizere.txt" using 2:3:4:(0.75)  title "Runtime 64 Exchanges" with boxerrorbars linetype 1  fs solid 0.5

 
       

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
set yrange [5:9] 
#set xrange [2:16]
set xrange [2:16]        
set xtics (2,4,8,16)  
set ytics (5,6,7,8,9)
set logscale x
set ylabel "Runtime (in min)" #font "Helvetica, 20"
set xlabel "Number Replica Processes"   
#plot "data-remd.txt" using 1:4 title "SAGA/Migol (QB/Poseidon)" with lp,\
#     "data-remd.txt" using 1:5 title "SAGA/GRAM  (QB/Poseidon)" with lp
plot  "data-remd.txt" using 1:2 title "SAGA/Migol (QB)" with lp,\
      "data-remd.txt" using 1:3 title "SAGA/GRAM  (QB)" with lp         
set xrange [1.75:17] 
set yrange [0:65]  
set ytics (10, 20, 30, 40, 50,60) 
set output "perf_remd_glidin.eps"                              

plot  "data-remd-glidein.txt" using 1:2 title "Without Glide-In" with lp,\
      "data-remd-glidein.txt" using 1:3 title "With Glide-In" with lp
      #errorlines      
      

set output "perf_distributed_line.eps"  
set xlabel "Number Grid Resources"
set xrange [0.9:4.1]        
set xtics (1,2,4)
set yrange [10:130]    
set ytics (20, 40, 60, 80, 100, 120) 
plot  "data_remd_distributed_numberre.txt" using 1:3 title "Runtime (normalized)" with lp,\
      "data_remd_distributed_numberre.txt" using 1:5 title "Runtime (abolute)" with lp

# plot "data-remd.txt" using 1:7 title "SAGA/Migol (QB/Poseidon/Eric)" with lp,\
#      "data-remd.txt" using 1:6 title "SAGA/GRAM (QB/Poseidon/Eric)" with lp,\
#      "data-remd.txt" using 1:2 title "SAGA/Migol (QB)" with lp,\
#      "data-remd.txt" using 1:3 title "SAGA/GRAM  (QB)" with lp     


set output "perf_remd_timesteps.eps"
set ylabel "NAMD Timesteps/Second" #font "Helvetica, 20"
set xlabel "Number Replica Processes"
set yrange [0:45]  
set ytics (10, 20, 30, 40)   
#set xrange [0.9:9.25]  
set xrange [1.95:16.5]
set xtics (1,2,4,8,16)

plot  "data_remd_timesteps.txt" using 4:3 title "With Glide-In" with lp linewidth 3,\
      "data_remd_timesteps.txt" using 4:2 title "Without Glide-In" with lp linewidth 3


set output "perf_re_numberresource_tc_64ex.eps"
set ylabel "Time-to-Completion (in min)" 
set xlabel "Number Replica Processes"
set yrange [0:140]  
set ytics (10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120, 130, 140)   
#set xrange [0.9:9.25]  
set xrange [1.95:16.5]
set xtics (1,2,4,8,16)   
#set style line  1 linewidth 16 
#set style line  2 linewidth 12 
                                                                                          

plot  "data_re_numberresources_tc.txt" using 4:3 title "With Glide-In" with lp linewidth 4,\
      "data_re_numberresources_tc.txt" using 4:2 title "Without Glide-In" with lp linewidth 4

set output "perf_glidein_tc_64ex.eps"
set xtics ("No Glide-In" 1, "With Glide-In" 2.0, "2 Glide-Ins" 3.0, "4 Glide-Ins" 4.0)
set yrange [0:80]  
set xrange [0.5:2.5]    
set style fill solid 0.9   
set nokey
set nologscale
set ylabel "Time-to-Completion (in min)"   
set noxlabel  
plot "data-remd-glidein_tc_64ex.txt" using 1:2:3:(0.75)  title "Runtime" with boxerrorbars linetype 1  fs solid 0.5

