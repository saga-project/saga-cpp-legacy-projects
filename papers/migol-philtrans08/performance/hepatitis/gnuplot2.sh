set term postscript eps enhanced color linewidth 1.42 "Helvetica" 25 

set encoding iso_8859_1

set ylabel "Time per Generation (in min)"  #font "Helvetica, 20"
set xlabel "Number of Active Glide-Ins"    #font "Helvetica, 20"
set y2label "Speed Up" 


#set size 0.8,0.8
#set lmargin 9
#set bmargin 3
#set rmargin 2
#set tmargin 1
                 
#set xdata time
#set timefmt "%d.%m%y"
#set format x "%d.%m."
#set format y "%.0f"
set xrange [1:8] 
set yrange [0:35] 

set y2range [0:10.5] 
set y2tics 0, 1
set ytics nomirror
#set arrow .
#set linestyle 1 lt 1 lw 50
#"data.txt" using 1:2:6 title "Job Startup Time" with boxes linetype 1  fs solid 0.25,\
   

set output "perf_repex2.eps" 
plot "data2.txt" using 1:($3/60) axis x1y1 title "Time per Generation" with lp lw 4,\
     "data2.txt" using 1:6 axis x1y2 title "Speed Up" with lp lw 4

#set terminal x11
#replot
