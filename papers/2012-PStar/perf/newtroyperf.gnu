set terminal postscript eps enhanced "Helvetica,20"
set output 'newtroyperf.eps'

#
set style data histogram
set style histogram errorbars gap 2 lw 5
set boxwidth 0.5
set key invert reverse Left outside

set key samplen 0.5
set bmargin 1
set yrange[0:*]

set xtics ("\nTROY-BJ" 0,"II" 1, "III" 2,"IV" 3)
set style fill solid border -1
set xtic nomirror rotate by 45 scale -1
set title ""
plot newhistogram at 0, 'newtroyperf.data' using 2:3 title "Startup Time" fs solid 3, \
     '' using 4:5 title "Runtime of BFAST" fs solid 2,\
     '' using 6:7 title "Overhead" fs solid lc rgb "grey",\
     '' using 8:9 title "Total time" fs solid 7

#set style histogram rows

#plot newhistogram " " lt 1, 'newtroyperf.data' using 2 title "Startup Time",\
 #    ''  using 4 title "Runtime of BFAST"  , \
 #    '' using 6 title "Overhead" fs solid lc rgb "grey"
