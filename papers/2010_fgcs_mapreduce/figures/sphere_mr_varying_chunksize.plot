set term postscript eps enhanced color solid "Times-Roman" 23
set output 'sphere_mr_varying_chunksize.eps'
set log x
set pointsize 2
set key Left reverse
set xlabel 'Chunk size (MB)' font "Times-Italic, 28"
set ylabel 'Time (s)' font "Times-Italic, 28"
set xtic 0,2
set mxtics 16
set mytics 10
set y2range [256:1200]
set y2tics 0,100
set y2tics nomirror
plot[12:300][256:2900] \
    './sphere_mr_varying_chunksize.dat' using 1:2 title 'SAGA-MapReduce' axis x1y1 with lp pt 1 lw 3,\
    './sphere_mr_varying_chunksize.dat' using 1:3 title 'SAGA-Sphere'    axis x1y2 with lp pt 4 lw 3
