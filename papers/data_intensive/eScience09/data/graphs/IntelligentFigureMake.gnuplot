set term postscript eps enhanced color solid "Times-Roman" 21
set output 'IntelligentFigure.eps'
set pointsize 2
set key Left reverse
set xlabel 'N_w' font "Times-Italic, 25"
set ylabel 't_c(s)' font "Times-Italic, 25"
set mxtics 2
set mytics 5
plot[0:10][0:2000] './IntelligentGridFTP.dat' using 1:2 title 'S0: [C4, Intelligence]' with lp lw 3,\
'./MixedGridFTP.dat' using 1:2 title 'S1: [C4]' with lp lw 3,\
'./IntelligentGridFTP3Resources.dat' using 1:2 title 'S2: [C5, Intelligence]' with lp lw 3,\
'./NonIntelligentGridFTP3Resources.dat' using 1:2 title 'S3: [C5]' with lp lw 3
