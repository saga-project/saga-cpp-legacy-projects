set term postscript eps enhanced color solid "Times-Roman" 23
set output 'sphere_mr_varying_workers.eps'
set pointsize 2
set key Left
set xlabel 'Number of workers' font "Times-Italic, 28"
set ylabel 'T_s (sec)' font "Times-Italic, 28"
set mxtics 2
set mytics 5
plot[0:11][0:3400] \
    './sphere_mr_varying_workers.dat' using 1:2 title 'SAGA-MapReduce (HC, Dist.)'   with lp pt 6 lc 1 lw 3,\
    './sphere_mr_varying_workers.dat' using 1:4 title 'SAGA-MapReduce (FG, Dist.)'   with lp pt 6 lc 3 lw 3,\
    './sphere_mr_varying_workers.dat' using 1:3 title 'SAGA-Sphere (HC, Dist.)'      with lp pt 6 lc 5 lw 3,\
    './sphere_mr_varying_workers.dat' using 1:5 title 'SAGA-Sphere (FG, Dist.)'      with lp pt 6 lc 6 lw 3

