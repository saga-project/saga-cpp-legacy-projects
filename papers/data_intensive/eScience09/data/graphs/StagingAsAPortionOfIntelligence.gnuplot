set term postscript eps enhanced color solid "Times-Roman" 17
set output 'StagingAsAPortionOfIntelligenceFigure.eps'
set xlabel 'N_W' font "Times-Roman, 25"
set ylabel 'T_C(S)' font "Times-Roman, 25"
set mxtics 5
set mytics 5
plot[0:8] './IntelligentGridFTP.dat' using 1:2 title 'Intelligent Experiment' with lp lw 3,\
'./StagingTimes.dat' using 1:2 title 'Time Staging' with lp lw 3
