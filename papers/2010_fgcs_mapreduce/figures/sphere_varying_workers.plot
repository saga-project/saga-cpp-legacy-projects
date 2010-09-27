set term postscript eps enhanced color solid "Times-Roman" 23
set output 'sphere_varying_workers.eps'
set pointsize 2
set key Right
set xlabel 'Number of workers' font "Times-Italic, 28"
set ylabel 'T_s (sec)' font "Times-Italic, 28"
set mxtics 2
set mytics 5
plot[0:11][0:6000] \
    './sphere_varying_workers.dat' using 1:2 title 'SAGA-Sphere (HC, local data and compute)'       with lp pt 1 lw 3,\
    './sphere_varying_workers.dat' using 1:3 title 'SAGA-Sphere (HC, distributed data and compute)' with lp pt 4 lw 3

