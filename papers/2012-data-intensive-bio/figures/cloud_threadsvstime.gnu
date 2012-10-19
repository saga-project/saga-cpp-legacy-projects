set terminal postscript eps enhanced "Helvetica" 28
set output 'cloud_threadsvstime.eps'

set xlabel "Number of Threads" font "Helvetica,26"
set ylabel "Time to Completion(in seconds)" font "Helvetica,26"

set autoscale
#set xrange [0.001:0.005]
#set yrange [20:500]
set logscale x 2
#set xtics 2
set style line 1 lc rgb '#0000' lt 1 lw 6 pt 2 ps 1.5
set style line 2 lc rgb '#0000' lt 2 lw 6 pt 4 ps 1.5

set key left 
set notitle
plot 'cloud_threadsvstime.data' u 1:2 title '100mb Read file'  w lp ls 1 ,'cloud_threadsvstime.data' u 1:3 title '800mb Read file'  w lp ls 2