set term postscript eps enhanced color solid "Times-Roman" 23
set output 'LocalFigure.eps'
set pointsize 2
set key Left reverse
set xlabel 'N_w' font "Times-Italic, 25"
set ylabel 't_c(s)' font "Times-Italic, 25"
set mxtics 2
set mytics 5
plot[0:9][0:140]'./LocalFileSystem.dat' using 1:2 title 'S0: [{/Times-Italic c_s} = 287 MB; {/Times-Italic N_c} = 8]' with lp lw 3,\
'./LocalFileSystemHalfChunkSize16Assignments.dat' using 1:2 title 'S1: [{/Times-Italic c_s} = 144 MB; {/Times-Italic N_c} = 16] ' with lp lw 3,\
'./LocalFileSystemHalfChunkSize8Assignments.dat' using 1:2 title 'S2: [{/Times-Italic c_s} = 144 MB; {/Times-Italic N_c} = 8] ' with lp lw 3
