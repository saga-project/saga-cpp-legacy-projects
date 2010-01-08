set term postscript eps enhanced color solid "Times-Roman" 17
set output 'CloudStore3Mach.eps'
set xlabel 'Number of workers' 
set ylabel 'Time (Seconds)' 
set mxtics 5
set mytics 5
plot[0:8][0:2000] './CloudStore3Mach2R.dat' using 1:2 title 'C1 - [287 MB; E(Y, Y), P(Y, Y), O(Y,Y); CloudStore; Direct; 2]' with lp,\
'./CloudStore3Mach3R.dat' using 1:2 title 'C2 - [287 MB; E(Y, N), P(N, Y),O(Y,Y); CloudStore; Direct; 3]' with lp
