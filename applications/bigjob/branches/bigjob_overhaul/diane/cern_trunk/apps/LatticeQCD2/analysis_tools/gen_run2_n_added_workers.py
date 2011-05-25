import sys, time

from LatticeQCD2.analysis_tools.analyzers import main

analyzer = main()

#############################################################################
# plots

from diane.analysis_tools import Histogram1D 

import matplotlib.pylab
from matplotlib.pylab import *
import datetime

matplotlib.rcParams['savefig.dpi'] = 300


# convert from seconds to datetime objects
time_v = [datetime.datetime.fromtimestamp(x) for x in analyzer.time.points]

from matplotlib.ticker import NullFormatter, NullLocator
from matplotlib.dates import WeekdayLocator, MONDAY, DateFormatter

def combined_plot(a,run_no,title=None):

    if title is None:
        title = "LQCD run %d"%run_no

    startA, finishA, startB, finishB = a.time.points[0], a.time.points[720], a.time.points[721], a.time.points[-1]
    print 80 * '#'
    print 'Part 1 start: %s, part 1 finish: %s\nPart 2 start: %s, part 2 finish: %s' % (time.ctime(startA), time.ctime(finishA), time.ctime(startB), time.ctime(finishB))
    print 80 * '#'

    wa = a.worker_history

    vp = wa.n_added_workers_per_time_unit
    #vp = wa.n_removed_workers_per_time_unit
    #vp = [x[0]+x[1] for x in zip(wa.n_added_workers_per_time_unit,wa.n_removed_workers_per_time_unit)]

    #vp = [x[0]-x[1] for x in zip(wa.n_added_workers_per_time_unit,wa.n_invalid_workers_per_time_unit)]

    #vp = [(float(x[0])+x[1])/(x[2]+1) for x in zip(wa.n_added_workers_per_time_unit,wa.n_removed_workers_per_time_unit,wa.n_workers)]

    #vp = []
    #for x in zip(wa.n_workers,wa.n_invalid_workers):
    #    if x[0] == 0:
    #        vp.append(0.0)
    #    else:
    #        vp.append(float(x[1])/x[0])

    def count(vp):
        zero_cnt = 0
        one_cnt = 0
        for x in vp:
            if x == 0:
                zero_cnt += 1
            if x == 1:
                one_cnt += 1

        print 'zeros',zero_cnt
        print 'ones',one_cnt

    count(vp[:720])
    count(vp[721:])

    print vp[:720]

    print vp[721:]

    # REMOVE THE CRASH EVENT
    #vp = vp[0:841] + vp[877:]
    ta = a.task_execution

    matplotlib.pyplot.clf()

    ax1 = subplot(211)
    ax1.set_xlabel('added workers')
    ax1.set_ylabel('$T_{hours}$')
    h1_1 = Histogram1D(0,250,50, title='Run 2')
    for i in vp[0:720]:
        h1_1.fill(i)
    h1_1.plot()#annotate=False)

    ax2 = subplot(212)
    ax2.set_xlabel('added workers')
    ax2.set_ylabel('$T_{hours}$')
    h2_2 = Histogram1D(0,250,50) #, title=r'LQCD Run 2 (w/ AgentFactory)')
    for i in vp[721:]:
        h2_2.fill(i)
    h2_2.plot() #annotate=False)

    savefig('lqcd-run2-n-added-workers-per-time-unit')

run_no = None

if 'run1' in analyzer.filename:
    run_no = 1
if 'run2' in analyzer.filename:
    run_no = 2
if 'run3' in analyzer.filename:
    run_no = 3
if 'run4' in analyzer.filename:
    run_no = 4

if not run_no:
    from diane.analysis_tools.pframework import Parameter
    run_no = Parameter('run_no',1).value

combined_plot(analyzer,run_no)
show()
