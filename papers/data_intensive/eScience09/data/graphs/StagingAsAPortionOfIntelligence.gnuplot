set term postscript eps enhanced color solid
set output 'StagingAsAPortionOfIntelligenceFigure.eps'
set xlabel 'Number of workers'
set ylabel 'Time (Seconds)'
plot[0:8] './IntelligentGridFTP.dat' using 1:2 title 'Intelligent Experiment' with lp,\
     './StagingTimes.dat' using 1:2 title 'Time Staging' with lp
