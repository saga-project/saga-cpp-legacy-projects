set term postscript eps enhanced color solid "Times-Roman" 21
set output 'CloudStoreCompute.eps'
set xlabel 'N_w' font "Times-Italics, 25"
set ylabel 't_c(s)' font "Times-Italics, 25"
set mxtics 5
set mytics 5
plot[0:8][0:2000] './CloudStoreComputeLocal.dat' using 1:2 title 'C1 - [144 MB; E(Y, Y); CloudStore; Direct; 1]' with lp lw 3,\
'./CloudStoreComputeRemote.dat' using 1:2 title 'C2 - [144 MB; E(Y, N), P(N, Y); CloudStore; Direct; 1]' with lp lw 3,\
'./CloudStoreCompute2Chunkservers.dat' using 1:2 title 'C3 - [144 MB; E(Y, Y), P(N, Y); CloudStore; Direct; 2]' with lp lw 3,\
'./CloudStoreComputeMixed.dat' using 1:2 title 'C4 - [144 MB; E(Y, Y), P(Y, Y); CloudStore; Direct; 1]' with lp lw 3
