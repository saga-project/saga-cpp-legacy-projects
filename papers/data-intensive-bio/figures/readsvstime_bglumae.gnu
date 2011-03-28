set terminal postscript eps 
set output 'readsvstime_bglumae.eps'

set xlabel "Read file size(GB)"
set ylabel "Time to Completion (seconds)"

set autoscale
#set xrange [0.001:0.005]
#set yrange [20:500]
#set ytics ()

plot 'readsvstime_bglumae.data' u 1:2 title "10 index files" w l, 'readsvstime_hg18.data' u 1:3 title "40 index files" w l 

