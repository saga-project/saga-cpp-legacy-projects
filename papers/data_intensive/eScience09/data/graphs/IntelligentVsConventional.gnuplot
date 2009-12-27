set term postscript eps enhanced
set output 'IntelligentVsConventionalFigure.eps'
set xlabel 'Number of workers'
set ylabel 'Time (Seconds)'
plot './LocalGridFTP.dat' using 1:2 title 'Local Data - [287 MB; P(Y, Y), O(N, N); GridFTP; Total]' with lp,\
     './RemoteGridFTP.dat' using 1:2 title 'Remote Data - [287 MB; P(Y, N), O(N, Y); GridFTP; Total] ' with lp,\
     './MixedGridFTP.dat' using 1:2 title 'Remote and Local - [287 MB; P(Y, Y), O(Y, Y); GridFTP; Total]' with lp,\
     './IntelligentGridFTP.dat' using 1:2 title 'Remote and Local with Intelligence - [287 MB; P(Y, Y), O(Y, Y); GridFTP; Incomplete]' with lp
