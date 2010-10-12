set term postscript eps enhanced color solid "Times-Roman" 23
set output 'sagamr_varying_workers.eps'
set pointsize 2
set key Left
set xlabel 'Number of workers' font "Times-Italic, 28"
set ylabel 'T_s (s)' font "Times-Italic, 28"
set mxtics 2
set mytics 5
plot[0:11][0:9000] \
    './sagamr_varying_workers.dat' using 1:2 title 'SAGA-MR (HC, local)' with lp pt 1 lw 3,\
    './sagamr_varying_workers.dat' using 1:3 title 'SAGA-MR (HC, dist.)' with lp pt 4 lw 3,\
    './sagamr_varying_workers.dat' using 1:4 title 'SAGA-MR (FG, local)' with lp pt 1 lw 3,\
    './sagamr_varying_workers.dat' using 1:5 title 'SAGA-MR (FG, dist.)' with lp pt 4 lw 3

