set term postscript eps enhanced color solid "Times-Roman" 17
set output 'NumberResourcesFigure.eps'
set pointsize 2
set key Left reverse
set xlabel 'N_r' font "Times-Italic, 20"
set ylabel 't_c(s)' font "Times-Italic, 20"
set xtics 1
plot[.5:3.5][0:550] './GridFTPVsNumberResources.dat' using 1:2 title 'GridFTP with Intelligence' with boxes, \
'./CloudStoreVsNumberResources.dat' using 1:2 title 'Cloudstore' with boxes
