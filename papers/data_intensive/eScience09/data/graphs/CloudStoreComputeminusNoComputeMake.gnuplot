set term postscript eps enhanced color solid "Times-Roman" 17
set output 'CloudStoreComputeMinusNoCompute144.eps'
set label "144MB, CloudStore" at 0.25, 375
set xlabel 'N_w' font "Times-Italic, 20"
set ylabel '{/Symbol D} t_c(s)' font "Times-Italic, 20"
set mxtics 2
set mytics 5
plot[0:8][0:400] "< paste CloudStoreComputeLocal.dat CloudStoreNoComputeLocal.dat" using 1:($2-$4) title '[C1, r = 1]' with lp lw 3,\
"< paste CloudStoreComputeRemote.dat CloudStoreNoComputeRemote.dat" using 1:($2-$4) title '[C2, r = 1]' with lp lw 3,\
"< paste CloudStoreCompute2Chunkservers.dat CloudStoreNoCompute2Chunkservers.dat" using 1:($2-$4) title '[C3, r = 2]' with lp lw 3,\
"< paste CloudStoreComputeMixed.dat CloudStoreNoComputeMixed.dat" using 1:($2-$4) title '[C4, r = 1]' with lp lw 3

