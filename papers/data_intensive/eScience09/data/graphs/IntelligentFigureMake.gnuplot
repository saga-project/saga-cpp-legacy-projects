set term postscript eps enhanced color solid "Times-Roman" 17
set output 'IntelligentFigure.eps'
set xlabel 'Number of workers'
set ylabel 'Time (Seconds)'
set mxtics 5
set mytics 5
plot[0:9] './IntelligentGridFTP.dat' using 1:2 title 'C1 - Intelligence [287 MB; P(Y, Y), O(Y, Y); Local; GridFTP]' with lp,\
'./MixedGridFTP.dat' using 1:2 title 'C2 - [287 MB; P(Y, Y), O(Y, Y); Local; GridFTP]' with lp,\
'./IntelligentGridFTP3Resources.dat' using 1:2 title 'C3 - Intelligence [287 MB; P(Y, Y), O(Y, Y), A(Y, Y); Local; GridFTP]' with lp,\
'./NonIntelligentGridFTP3Resources.dat' using 1:2 title 'C4 - [287 MB; P(Y, Y), O(Y, Y), A(Y, Y); Local; GridFTP]' with lp
