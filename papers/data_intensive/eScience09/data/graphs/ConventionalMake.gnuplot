set term postscript eps enhanced
set output 'ConventionalFigure.eps'
set xlabel 'Number of workers'
set ylabel 'Time (Seconds)'
plot './LocalGridFTP.dat' using 1:2 title 'Local Data - [287 MB; E(Y, Y); GridFTP; Direct Access]' with lp, './RemoteGridFTP.dat' using 1:2 title 'Remote Data - [287 MB; E(Y, N), P(N, Y); GridFTP; Indirect Access]' with lp, './MixedGridFTP.dat' using 1:2 title 'Remote and Local Data - [287 MB; E(Y, Y), P(Y, Y); GridFTP; Indirect Access]' with lp
