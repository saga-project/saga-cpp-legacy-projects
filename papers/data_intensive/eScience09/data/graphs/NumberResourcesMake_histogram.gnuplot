set term postscript eps enhanced color solid "Times-Roman" 17
set output 'NumberResourcesFigure_histogram.eps'
set pointsize 2
set auto x
set yrange [0:600]
set key Left reverse
set style data histogram
set style histogram cluster gap 1
set style fill pattern 4 border -1
set boxwidth 1
set xlabel 'N_r' font "Times-Italic, 25"
set ylabel 't_c(s)' font "Times-Italic, 25"
set xtics 1
set mytics 2
#plot newhistogram , './GridFTPVsNumberResources.dat' using 1:2 title 'S0: [{/Times-Italic fs} = gridFTP, {/Times-Italic m} = Local, Intelligence]' \
# newhistogram , './CloudStoreVsNumberResources.dat' using 1:2 title 'S1: [{/Times-Italic fs} = CloudStore, {/Times-Italic m} = Direct]'
plot './GridFTPVsNumberResources.dat'    using 2:xtic(1) title 'S0: [{/Times-Italic fs} = gridFTP, {/Times-Italic m} = Local, Intelligence]' , \
     './CloudStoreVsNumberResources.dat' using 2        title 'S1: [{/Times-Italic fs} = CloudStore, {/Times-Italic m} = Direct]'
