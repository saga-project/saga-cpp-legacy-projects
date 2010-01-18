set term postscript eps enhanced color solid "Times-Roman" 17
set output 'ConventionalFigure.eps'
set pointsize 2
set key Left reverse
set xlabel 'N_w' font "Times-Roman, 25"
set ylabel 't_c(s)' font "Times-Roman, 25"
set mxtics 2
set mytics 5
plot[0:9][0:8000] './LocalGridFTP.dat' using 1:2 title 'S0: [C1]' with lp lw 3,\
'./RemoteGridFTP.dat' using 1:2 title 'S1: [C2]' with lp lw 3,\
'./MixedGridFTP.dat' using 1:2 title 'S2: [C4]' with lp lw 3
