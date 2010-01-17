set term postscript eps enhanced color solid "Times-Roman" 17
set output 'CloudStoreVsGridFTPFigure.eps'
set pointsize 2
#set key Left reverse
set xlabel 'N_w' font "Times-Italic, 20"
set ylabel 't_c(s)' font "Times-Italic, 20"
set mxtics 2
set mytics 5
plot[0:9][0:750] './CloudStore3Mach3R.dat' using 1:2 title 'S0: [{/Times-Italic fs} = C, {/Times-Italic m} = D, {/Times-Italic r} = 1]' with lp lw 3,\
'./IntelligentGridFTP3Resources.dat' using 1:2 title 'S1: [{/Times-Italic fs} = G, {/Times-Italic m} = L, Intelligence]' with lp lw 3
