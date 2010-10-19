set term postscript eps enhanced color "Times-Roman" 23
set output 'sagamr_varying_workers.eps'
set pointsize 2
set key Left
set xlabel 'Number of workers' font "Times-Italic, 28"
set ylabel 'T_s (s)' font "Times-Italic, 28"
set mxtics 2
set mytics 5
plot[0:11][1000:3000] \
    './sagamr_varying_workers.dat' using 1:4     title 'SAGA-MR (FG, local)' with lines            lt 1 lc 2 lw 3, \
    './sagamr_varying_workers.dat' using 1:6     title 'SAGA-MR (FG, dist.)' with linespoints pt 6 lt 3 lc 3 lw 3, \
    './sagamr_varying_workers.dat' using 1:4:5 notitle                       with errorbars   pt 0 lt 1 lc 2 lw 1

