set term postscript eps enhanced color solid "Times-Roman" 23
set output 'NumberResourcesFigure.eps'
set pointsize 2
set key Left reverse width -10
set xlabel 'N_r' font "Times-Italics, 25"
set ylabel 't_c(s)' font "Times-Italics, 25"
set xtics 1
plot[.5:3.5][0:600] './GridFTPVsNumberResources.dat' using 1:2 title 'S0: [{/Times-Italic fs} = G, {/Times-Italic m} = L, Intelligence]' with boxes, \
'./CloudStoreVsNumberResources.dat' using 1:2 title 'S1: [{/Times-Italic fs} = C, {/Times-Italic m} = D]' with boxes
