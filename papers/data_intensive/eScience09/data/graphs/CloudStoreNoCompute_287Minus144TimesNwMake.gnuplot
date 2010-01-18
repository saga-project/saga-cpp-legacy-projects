set term postscript eps enhanced color solid "Times-Roman" 17
set output 'CloudStoreNoCompute_287Minus144TimesNw.eps'
set pointsize 2
set key Left reverse
set xlabel 'N_w' font "Times-Roman, 25"
set ylabel '{/Symbol D} t_c(s) * N_w' font "Times-Roman, 25"
set mxtics 2
set mytics 5
plot[0:9][-200:2000] "< paste localCloudStore.dat CloudStoreNoComputeLocal.dat" using 1:(($2-$4)*$1) title 'S0: [C1, r = 1]' with lp lw 3,\
"< paste remoteCloudStore.dat CloudStoreNoComputeRemote.dat" using 1:(($2-$4)*$1) title 'S1: [C2, r = 1]' with lp lw 3,\
"< paste localCloudStore2Chunks.dat CloudStoreNoCompute2Chunkservers.dat" using 1:(($2-$4)*$1) title 'S2: [C3, r = 2]' with lp lw 3,\
"< paste 2ChunksReplication1.dat CloudStoreNoComputeMixed.dat" using 1:(($2-$4)*$1) title 'S3: [C4, r = 1]' with lp lw 3

