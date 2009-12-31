set term postscript eps enhanced color solid
set output 'CloudStoreCompute.eps'
set xlabel 'Number of workers'
set ylabel 'Time (Seconds)'
plot './CloudStoreComputeLocal.dat' using 1:2 title 'C1 - [144 MB; E(Y, Y); CloudStore; Direct; 1]' with lp, './CloudStoreComputeRemote.dat' using 1:2 title 'C2 - [144 MB; E(Y, N), P(N, Y); CloudStore; Direct; 1]' with lp, './CloudStoreCompute2Chunkservers.dat' using 1:2 title 'C3 - [144 MB; E(Y, Y), P(Y, Y); CloudStore; Direct; 2]' with lp, './CloudStoreComputeMixed.dat' using 1:2 title 'C4 - [144 MB; E(Y, Y), P(Y, Y); CloudStore; Direct; 1]' with lp
