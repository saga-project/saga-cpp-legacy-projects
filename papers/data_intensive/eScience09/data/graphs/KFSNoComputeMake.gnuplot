set term postscript eps enhanced
set output 'KFSNoComputeSmallerDataSet.eps'
set xlabel 'Number of workers'
set ylabel 'Time (Seconds)'
plot './KFSNoComputeLocal.dat' using 1:2 title 'C1 - [144 MB; E(Y, Y); CloudStore; Direct; 1]' with lp, './KFSNoComputeRemote.dat' using 1:2 title 'C2 - [144 MB; E(Y, N), P(N, Y); CloudStore; Direct; 1]' with lp, './KFSNoComputeMixed.dat' using 1:2 title 'C3 - [144 MB; E(Y, Y), P(Y, Y); CloudStore; Direct; 1]' with lp
