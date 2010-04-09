set term postscript eps enhanced color solid "Times-Roman" 23
set output 'sagamr_comparison.eps'
set pointsize 2
set key Left reverse
set xlabel 'Data size (GB)' font "Times-Italic, 28"
set ylabel 't_c (s)' font "Times-Italic, 28"
set mxtics 2
set mytics 5
plot[0:9][0:2500] './sagamr_comparison.dat' using 1:3 title 'Chunking time' with lp lw 3,\
'./sagamr_comparison.dat' using 1:4 title 'Original SAGA-MapReduce' with lp lw 3,\
'./sagamr_comparison.dat' using 1:5 title 'Enhanced SAGA-MapReduce' with lp lw 3
