set term postscript eps enhanced
set output 'CloudStoreFigure.eps'
set xlabel 'Number of workers'
set ylabel 'Time (Seconds)'
plot './localCloudStore.dat' using 1:2 title 'Local Data - [287 MB; E(Y,Y); CloudStore; Total]' with lp, './remoteCloudStore.dat' using 1:2 title 'Remote Data - [287 MB; E(Y, N), P(N, Y); CloudStore; Total] ' with lp, './localCloudStore2Chunks.dat' using 1:2 title 'Remote and Local - [287 MB; E(Y, Y), P(Y,Y); CloudStore; Total]' with lp, './2ChunksReplication1.dat' using 1:2 title 'Remote and Local - [287 MB; E(Y, Y), P(Y,Y); CloudStore; Incomplete]' with lp
