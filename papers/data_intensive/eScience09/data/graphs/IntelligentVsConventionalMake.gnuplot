set term postscript eps enhanced color solid "Times-Roman" 17
set output 'IntelligentVsConventionalFigure.eps'
set pointsize 2
set key Left reverse
set xlabel 'N_w' font "Times-Italic, 20"
set ylabel 't_c(s)' font "Times-Italic, 20"
set mxtics 2
set mytics 5
plot[0:9][0:8000] './LocalGridFTP.dat' using 1:2 title 'S0: [C1]' with lp lw 3,\
'./RemoteGridFTP.dat' using 1:2 title 'S1: [C2]' with lp lw 3,\
'./MixedGridFTP.dat' using 1:2 title 'S3: [C4]' with lp lw 3,\
'./IntelligentGridFTP.dat' using 1:2 title 'S4: [C4, Intelligence]' with lp lw 3
