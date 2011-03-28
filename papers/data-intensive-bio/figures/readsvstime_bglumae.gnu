set terminal postscript eps enhanced "Helvetica" 28
set output 'readsvstime_bglumae.eps'

set xlabel "Read File Size(in GB)" font "Helvetica,26"
set ylabel "Time to Completion(in 1000 seconds)" font "Helvetica,26"

set autoscale
#set xrange [0.001:0.005]
#set yrange [20:500]
set xtics 0.5
set style line 1 lc rgb '#0000' lt 1 lw 2 pt 5 ps 1.5
set style line 2 lc rgb '#0000' lt 1 lw 4 pt 7 ps 1.5
set key left 
plot 'readsvstime_bglumae.data' u 1:2 title "10 index files"  w lp ls 1, 'readsvstime_bglumae.data' u 1:3 title "40 index files"  w lp ls 2 
