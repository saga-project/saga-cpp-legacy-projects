set term postscript eps enhanced color solid "Times-Roman" 23
set output 'sphere_mr_varying_chunksize.eps'
set log x
set pointsize 2
set key Left reverse
set xlabel 'Chunk size (MB)' font "Times-Italic, 28"
set ylabel 'T_s (sec)' font "Times-Italic, 28"
set xtic 0,2
set mxtics 16
set mytics 5
plot[12:300][256:2900] \
    './sphere_mr_varying_chunksize.dat' using 1:2 title 'SAGA-MapReduce (HC)'  with lp pt 1 lw 3,\
 './sphere_mr_varying_chunksize.dat' using 1:3 title 'SAGA-MapReduce (FG)'  with lp pt 1 lw 3,\
    './sphere_mr_varying_chunksize.dat' using 1:4 title 'SAGA-Sphere (HC)' with lp pt 4 lw 3,\
    './sphere_mr_varying_chunksize.dat' using 1:5 title 'SAGA-Sphere (FG)' with lp pt 4 lw 3
