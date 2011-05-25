reset

datfn='task-exec-distrib-iter4-5.1815.dat'
outfn='task-exec-distrib-iter4-5.1815.png'

set title "Task execution time, beta=5.1815, run 4"
set term png size 1000 500
set output outfn 
set samples 1000

set logscale y
set yr [1:]

set xlabel "seconds"

t0=8000;f0=1.e5;nu=1.5;c=3.
f(x) = f0*(x/t0-1.)**nu*exp(-c*x/t0)
g1=8000;g0=1.e5
g(x) = g0*(x/g1-1.)**1.5*exp(-3.*x/g1)
fit f(x) datfn u 1:2 via t0,f0,nu,c
fit g(x) datfn u 1:2 via g0,g1

plot datfn u 1:2 t 'task execution time', g(x) t 'fit nu=3/2, c=3'

#rep f(x) t 'Best fit'
#pause -1 'Linear scale'
#rep

print 'saved ',outfn
