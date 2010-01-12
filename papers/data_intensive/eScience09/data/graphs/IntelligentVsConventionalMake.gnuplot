set term postscript eps enhanced color solid "Times-Roman" 17
set output 'IntelligentVsConventionalFigure.eps'
set xlabel 'N_W' font "Times-Roman, 25"
set ylabel 'T_C(S)' font "Times-Roman, 25"
set mxtics 5
set mytics 5
plot[0:8] './LocalGridFTP.dat' using 1:2 title 'C1 - [287 MB; P(Y, Y), O(N, N); Local; GridFTP]' with lp lw 3,\
'./RemoteGridFTP.dat' using 1:2 title 'C2 - [287 MB; P(Y, N), O(N, Y); Local; GridFTP]' with lp lw 3,\
'./MixedGridFTP.dat' using 1:2 title 'C3 - [287 MB; P(Y, Y), O(Y, Y); Local; GridFTP]' with lp lw 3,\
'./IntelligentGridFTP.dat' using 1:2 title 'C4 - Intelligence - [287 MB; P(Y, Y), O(Y, Y); Local; GridFTP]' with lp lw 3
