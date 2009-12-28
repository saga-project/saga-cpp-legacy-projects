set term postscript eps enhanced
set output 'IntelligentVsConventionalFigure.eps'
set xlabel 'Number of workers'
set ylabel 'Time (Seconds)'
plot './LocalGridFTP.dat' using 1:2 title 'C1 - [287 MB; P(Y, Y), O(N, N); Local; GridFTP]' with lp,\
     './RemoteGridFTP.dat' using 1:2 title 'C2 - [287 MB; P(Y, N), O(N, Y); GridFTP; Total] ' with lp,\
     './MixedGridFTP.dat' using 1:2 title 'C3 - [287 MB; P(Y, Y), O(Y, Y); GridFTP; Total]' with lp,\
     './IntelligentGridFTP.dat' using 1:2 title 'C4 - Intelligence - [287 MB; P(Y, Y), O(Y, Y); Local; GridFTP]' with lp
