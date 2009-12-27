set term postscript eps enhanced
set output 'IntelligentVsConventionalFigure.eps'
set xlabel 'Number of workers'
set ylabel 'Time (Seconds)'
plot './LocalGridFTP.dat' using 1:2 title 'Local Data - [287 MB; E(Y,Y); GridFTP; Total]' with lp, './RemoveGridFTP.dat' using 1:2 title 'Remote Data - [287 MB; E(Y, N), P(N, Y); GridFTP; Total] ' with lp, './MixedGridFTP.dat' using 1:2 title 'Remote and Local - [287 MB; E(Y, Y), P(Y,Y); GridFTP; Total]' with lp, './IntelligentGridFTP.dat' using 1:2 title 'Remote and Local intelligent - [287 MB; E(Y, Y), P(Y,Y); GridFTP; Incomplete]' with lp
