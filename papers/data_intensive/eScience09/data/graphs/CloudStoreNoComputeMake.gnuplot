set term postscript eps enhanced color solid
set output 'CloudStoreNoComputeSmallerDataSet.eps'
set xlabel 'Number of workers'
set ylabel 'Time (Seconds)'
plot './CloudStoreNoComputeLocal.dat' using 1:2 title 'C1 - [144 MB; E(Y, Y); CloudStore; Direct; 1]' with lp, './CloudStoreNoComputeRemote.dat' using 1:2 title 'C2 - [144 MB; E(Y, N), P(N, Y); CloudStore; Direct; 1]' with lp, './CloudStoreNoComputeMixed.dat' using 1:2 title 'C3 - [144 MB; E(Y, Y), P(Y, Y); CloudStore; Direct; 1]' with lp
