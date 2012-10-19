set term postscript eps enhanced color "Times-Roman" 23
set output 'sagamr_comparison.eps'
set pointsize 3
set key Left
set xlabel 'Data size (GB)' font "Times-Italic, 28"
set ylabel 'T_s (sec)' font "Times-Italic, 28"
set mxtics 2
set mytics 5
plot[0:9][0:3000] \
    './sagamr_comparison.dat' using 1:4 title 'Original SAGA-MapReduce (HC)' with lp lc 1 lt 1 pt 4 lw 3,\
    './sagamr_comparison.dat' using 1:5 title 'Enhanced SAGA-MapReduce (HC)' with lp lc 2 lt 3 pt 9 lw 3,\
    './sagamr_comparison.dat' using 1:3 title 'Chunking time (HC)'           with lp lc 3 lt 5 pt 1 lw 3

