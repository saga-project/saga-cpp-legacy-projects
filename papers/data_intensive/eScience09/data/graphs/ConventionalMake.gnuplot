set term postscript eps enhanced
set output 'ConventionalFigure.eps'
set xlabel 'Number of workers'
set ylabel 'Time (Seconds)'
plot './LocalGridFTP.dat' using 1:2 title 'Local Data' with lp, './RemoteGridFTP.dat' using 1:2 title 'Remote Data' with lp, './MixedGridFTP.dat' using 1:2 title 'Remote and Local Data' with lp
