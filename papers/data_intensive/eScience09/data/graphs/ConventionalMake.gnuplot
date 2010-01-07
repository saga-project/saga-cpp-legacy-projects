set term postscript eps enhanced color solid "Times-Roman" 17
set output 'ConventionalFigure.eps'
set xlabel 'Number of workers'
set ylabel 'Time (Seconds)'
set mxtics 5
set mytics 5
plot[0:8] './LocalGridFTP.dat' using 1:2 title 'C1 - [287 MB; E(Y, Y); Local; GridFTP]' with lp, './RemoteGridFTP.dat' using 1:2 title 'C2 - [287 MB; E(Y, N), P(N, Y); Local; GridFTP]' with lp, './MixedGridFTP.dat' using 1:2 title 'C3 - [287 MB; E(Y, Y), P(Y, Y); Local; GridFTP]' with lp
