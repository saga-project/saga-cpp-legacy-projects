set term postscript eps enhanced color solid "Times-Roman" 23
set output 'CloudStoreComputeMinusNoCompute144.eps'
set pointsize 2
set key Left reverse
set xlabel 'N_w' font "Times-Italic, 28"
set ylabel '{/Symbol D} t_c(s)' font "Times-Italic, 28"
set mxtics 2
set mytics 5
plot[0:9][0:500] "< paste CloudStoreComputeLocal.dat CloudStoreNoComputeLocal.dat" using 1:($2-$4) title 'S0: ({/Times-Italic C1}, {/Times-Italic r} = 1)' with lp lw 3,\
"< paste CloudStoreComputeRemote.dat CloudStoreNoComputeRemote.dat" using 1:($2-$4) title 'S1: ({/Times-Italic C2}, {/Times-Italic r} = 1)' with lp lw 3,\
"< paste CloudStoreCompute2Chunkservers.dat CloudStoreNoCompute2Chunkservers.dat" using 1:($2-$4) title 'S2: ({/Times-Italic C3}, {/Times-Italic r} = 2)' with lp lw 3,\
"< paste CloudStoreComputeMixed.dat CloudStoreNoComputeMixed.dat" using 1:($2-$4) title 'S3: ({/Times-Italic C4}, {/Times-Italic r} = 1)' with lp lw 3

