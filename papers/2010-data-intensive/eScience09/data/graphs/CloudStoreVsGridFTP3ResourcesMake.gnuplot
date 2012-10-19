set term postscript eps enhanced color solid "Times-Roman" 23
set output 'CloudStoreVsGridFTPFigure.eps'
set pointsize 2
set key Left reverse width -6
set xlabel 'N_w' font "Times-Italic, 28"
set ylabel 't_c(s)' font "Times-Italic, 28"
set mxtics 2
set mytics 2
plot[0:10][0:1000] './IntelligentGridFTP3Resources.dat' using 1:2 title 'S1: (GridFTP, heuristic)' with lp lw 3,\
'./CloudStore3Mach3R.dat' using 1:2 title 'S0: (CloudStore, {/Times-Italic r} = 1)' with lp lw 3
