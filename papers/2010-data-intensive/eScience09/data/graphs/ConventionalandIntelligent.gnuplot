set term postscript eps enhanced color solid "Times-Roman" 23
set output 'ConventionalandIntelligent.eps'
set pointsize 2
set key Left reverse width -7
set xlabel 'N_w' font "Times-Italic, 28"
set ylabel 't_c(s)' font "Times-Italic, 28"
set mxtics 2
set mytics 5
plot[0:10][0:5000] './LocalGridFTP.dat' using 1:2 title 'S0: ({/Times-Italic C1})' with lp lw 3,\
'./RemoteGridFTP.dat' using 1:2 title 'S1: ({/Times-Italic C2})' with lp lw 3,\
'./IntelligentGridFTP.dat' using 1:2 title 'S2: ({/Times-Italic C4}, heuristic)' with lp lw 3,\
'./MixedGridFTP.dat' using 1:2 title 'S3: ({/Times-Italic C4})' with lp lw 3,\
'./IntelligentGridFTP3Resources.dat' using 1:2 title 'S4: ({/Times-Italic C5}, heuristic)' with lp lw 3,\
'./NonIntelligentGridFTP3Resources.dat' using 1:2 title 'S5: ({/Times-Italic C5})' with lp lw 3 lt 7

