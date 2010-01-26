set term postscript eps enhanced color solid "Times-Roman" 17
set output 'CloudStoreVsGridFTPFigure.eps'
set pointsize 2
#set key Left reverse
set xlabel 'N_w' font "Times-Italic, 25"
set ylabel 't_c(s)' font "Times-Italic, 25"
set mxtics 2
set mytics 2
plot[0:10][0:800] './IntelligentGridFTP3Resources.dat' using 1:2 title 'S1: [{/Times-Italic fs} = gridFTP, {/Times-Italic m} = Local, Intelligence]' with lp lw 3,\
'./CloudStore3Mach3R.dat' using 1:2 title 'S0: [{/Times-Italic fs} = CloudStore, {/Times-Italic m} = Direct, {/Times-Italic r} = 1]' with lp lw 3
