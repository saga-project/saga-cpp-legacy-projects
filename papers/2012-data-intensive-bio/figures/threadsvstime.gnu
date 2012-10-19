set terminal postscript eps enhanced "Helvetica" 28
set output 'threadsvstime.eps'

set xlabel "Number of Threads" font "Helvetica,26"
set ylabel "Time to Completion(in seconds)" font "Helvetica,26"

set autoscale
#set xrange [0.001:0.005]
#set yrange [20:500]
set logscale x 2
#set xtics 2
set style line 1 lc rgb '#0000' lt 1 lw 6 pt 2 ps 1.5
set key left 
set notitle
plot 'threadsvstime.data' u 1:2 title ''  w lp ls 1 