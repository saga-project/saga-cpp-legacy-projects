set term postscript eps enhanced color solid "Times-Roman" 23
set output 'CloudStore3Mach.eps'
set pointsize 2
set key Left reverse
set xlabel 'N_w' font "Times-Italic, 28"
set ylabel 't_c(s)' font "Times-Italic, 28"
set mxtics 2
set mytics 5
plot[0:9][0:2000] './CloudStore3Mach1R.dat' using 1:2 title 'S0: ({/Times-Italic C5}, {/Times-Italic r} = 1)' with lp lw 3,\
'./CloudStore3Mach2R.dat' using 1:2 title 'S1: ({/Times-Italic C5}, {/Times-Italic r} = 2)' with lp lw 3,\
'./CloudStore3Mach3R.dat' using 1:2 title 'S2: ({/Times-Italic C5}, {/Times-Italic r} = 3)' with lp lw 3
