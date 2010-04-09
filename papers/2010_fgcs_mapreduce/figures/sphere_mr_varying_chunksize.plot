set term postscript eps enhanced color solid "Times-Roman" 23
set output 'sphere_mr_varying_chunksize.eps'
set pointsize 2
set key Left reverse
set xlabel 'Chunk size size (MB)' font "Times-Italic, 28"
set ylabel 'Time (s)' font "Times-Italic, 28"
set mxtics 16
set mytics 5
plot[0:300][0:1400] './sphere_mr_varying_chunksize.dat' using 1:2 title 'SAGA-MapReduce' with lp lw 3,\
'./sphere_mr_varying_chunksize.dat' using 1:3 title 'SAGA-Sphere' with lp lw 3
