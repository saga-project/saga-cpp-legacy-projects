set term postscript eps enhanced color solid "Times-Roman" 17
set output 'NumberResourcesFigure.eps'
set pointsize 2
set key Left reverse
set xlabel 'N_r' font "Times-Italic, 20"
set ylabel 't_c(s)' font "Times-Italic, 20"
set mxtics 2
set mytics 5
plot[0:3][0:1000] './GridFTPVsNumberResources.dat' using 1:2 title 'GridFTP with Intelligence' with lp lw 3, \
'./CloudStoreVsNumberResources.dat' using 1:2 title 'Cloudstore' with lp lw 3
