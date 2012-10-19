set term postscript eps enhanced color "Times-Roman" 23
set output 'sphere_varying_workers.eps'
set pointsize 2
set key Left
set xlabel 'Number of workers' font "Times-Italic, 28"
set ylabel 'T_s (sec)' font "Times-Italic, 28"
set mxtics 2
set mytics 5
plot[0:11][0:2500] \
    './sphere_varying_workers.dat' using 1:4 title 'SAGA-Sphere (FG, local)' with lp pt 6 lt 1 lc 8 lw 3, \
    './sphere_varying_workers.dat' using 1:5 title 'SAGA-Sphere (FG, dist.)' with lp pt 6 lt 3 lc 7 lw 3

