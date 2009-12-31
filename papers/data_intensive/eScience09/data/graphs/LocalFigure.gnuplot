set term postscript eps enhanced color solid
set output 'LocalFigure.eps'
set xlabel 'Number of workers'
set ylabel 'Time (Seconds)'
plot[0:8] './LocalFileSystem.dat' using 1:2 title 'C1 - [287 MB; 8 Assignments; E(Y, Y); Local; Local]' with lp,\
     './LocalFileSystemHalfChunkSize16Assignments.dat' using 1:2 title 'C2 - [144 MB; 16; E(Y, Y); Local; Local] ' with lp,\
     './LocalFileSystemHalfChunkSize8Assignments.dat' using 1:2 title 'C3 - [144 MB; 8; E(Y, Y); Local; Local] ' with lp
