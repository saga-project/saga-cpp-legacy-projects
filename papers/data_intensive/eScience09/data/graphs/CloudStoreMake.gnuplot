set term postscript eps enhanced color solid "Times-Roman" 20
set output 'CloudStoreFigure.eps'
set xlabel 'Number of workers' 
set ylabel 'Time (Seconds)' 
set mxtics 5
set mytics 5
plot[0:8][0:2000] './localCloudStore.dat' using 1:2 title 'C1 - [287 MB; E(Y, Y); CloudStore; Direct; 1]' with lp, './remoteCloudStore.dat' using 1:2 title 'C2 - [287 MB; E(Y, N), P(N, Y); CloudStore; Direct; 1] ' with lp, './localCloudStore2Chunks.dat' using 1:2 title 'C3 - [287 MB; E(Y, Y), P(Y, N); CloudStore; Direct; 2]' with lp, './2ChunksReplication1.dat' using 1:2 title 'C4 - [287 MB; E(Y, Y), P(Y, Y); CloudStore; Direct, 1]' with lp, './2ChunksReplication2.dat' using 1:2 title 'C5 - [287 MB; E(Y, Y), P(Y, Y); CloudStore; Direct, 2]' with lp 
