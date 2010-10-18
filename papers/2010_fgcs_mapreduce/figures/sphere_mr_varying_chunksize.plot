set term postscript eps enhanced color solid "Times-Roman" 23
set output 'sphere_mr_varying_chunksize.eps'
set log x
set pointsize 2
set key Left
set xlabel 'Chunk size (MB)' font "Times-Italic, 28"
set ylabel 'T_s (sec)' font "Times-Italic, 28"
set xtic 0,2
set mxtics 16
set mytics 5
plot[12:550][0:9000] \
    './sphere_mr_varying_chunksize.dat' using 1:4   title 'SAGA-MapReduce (HC, dist.)'  with linespoints pt 6 lc 1 lw 3,\
    './sphere_mr_varying_chunksize.dat' using 1:2   title 'SAGA-MapReduce (FG, dist.)'  with lines            lc 3 lw 3,\
    './sphere_mr_varying_chunksize.dat' using 1:5   title 'SAGA-Sphere (HC, dist.)'     with linespoints pt 6 lc 5 lw 3,\
    './sphere_mr_varying_chunksize.dat' using 1:6   title 'SAGA-Sphere (FG, dist.)'     with linespoints pt 6 lc 6 lw 3,\
    './sphere_mr_varying_chunksize.dat' using 1:2:3 notitle                      with errorbars   pt 0 lc 3 lw 3

