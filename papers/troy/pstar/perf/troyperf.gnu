set terminal postscript eps enhanced "Helvetica,18" 
set output 'troyperf.eps'

set ylabel "Time to completion(seconds)" font "Helvetica,18"
set bmargin 3

#set datafile separator "," 
set style data histogram
set style histogram clustered #gap  3 title offset character 0, 0, 0
set style histogram errorbars 
#set xtic rotate by -45 scale 1
set auto x
set yrange[0:*]
set xtics ("BJ-SAGA" 0.5,"BJ-DIANE" 1.5, "BJ-SAGA,\n BJ-DIANE " 2.5)

#for startup times
#plot 'troyperf.data' using 2:3 title 'Startup Time' fs solid 3

#for bfast Match step times
#plot 'troyperf.data' using 4:5 title 'Bfast Kernel Time' fs solid 3


#for Total times using 4:5:xtic(1) title col fs solid lc rgb "grey"
plot 'troyperf.data' using 6:7 title 'Total Time' fs solid 3