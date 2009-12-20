set term postscript eps enhanced
set output 'KFSFigure.eps'
set xlabel 'Number of workers'
set ylabel 'Time (Seconds)'
plot './localKFS.dat' using 1:2 title '1 Local and 0 Remote Dataserver (Replication 1)' with lp, './remoteKFS.dat' using 1:2 title '0 Local and 1 Remote Dataserver (Replication 1)' with lp, './localKFS2Chunks.dat' using 1:2 title '1 Local and 1 Remote Dataserver (Replication 2)' with lp, './2ChunksReplication1.dat' using 1:2 title '1 Local and 1 Remote Dataserver (Replication 1)' with lp
