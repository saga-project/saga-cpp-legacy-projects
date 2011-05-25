from LatticeQCD2.analysis_tools.analyzers import main

analyzer = main()

#############################################################################
# plots

import matplotlib.pylab
from matplotlib.pylab import *
import datetime

# convert from seconds to datetime objects
time_v = [datetime.datetime.fromtimestamp(x) for x in analyzer.time.points]

from matplotlib.ticker import NullFormatter, NullLocator, FormatStrFormatter, FuncFormatter, ScalarFormatter
from matplotlib.dates import WeekdayLocator, MONDAY, DateFormatter

def worker_history_plot(analyzer):
    ax1=subplot(111)

    ax1.xaxis.set_major_locator(DayLocator([1,15]))
    ax1.xaxis.set_major_formatter(DateFormatter('%d/%m'))
    ax1.xaxis.set_minor_locator(DayLocator())
    setp(ax1.get_xticklabels(),fontsize=10)

    points1, = ax1.plot_date(time_v, analyzer.n_invalid_workers,'or')
    ax1.plot_date(time_v, analyzer.n_invalid_workers,'-r')

    points2, = ax1.plot_date(time_v, analyzer.n_workers,'ob')
    ax1.plot_date(time_v, analyzer.n_workers,'-b')

    points1.set_markersize(2)
    points2.set_markersize(2)

    grid(True)

def iterations_plot(analyzer):
    ax1=subplot(111)

    ax1.plot_date(time_v,analyzer.n_snapshots_per_time_unit,':b')

    ax2=twinx()
    ax2.plot_date(time_v, analyzer.n_snapshots,'-r')
    ax2.yaxis.tick_right()

    ax1.xaxis.set_major_locator(DayLocator([1,15]))
    ax1.xaxis.set_major_formatter(DateFormatter('%d/%m/%Y'))
    ax1.xaxis.set_minor_locator(DayLocator())
    setp(ax1.get_xticklabels(),fontsize=10)

    ax2.xaxis.set_visible(False) # switch off duplicated labels

#worker_history_plot(analyzer.worker_history_analyzer)
#show()

#iterations_plot(analyzer.task_execution_analyzer)
#show()

def combined_plot(a,run_no,title=None):
    from datetime import datetime

    if title is None:
        title = "LQCD run %d"%run_no

    if run_no == 2:
        ax1_limits = ((datetime(2008,6,14),datetime(2008,8,2)),[0,1600])
    else:
        ax1_limits = None # auto

    wa = a.worker_history
    ta = a.task_execution
    matplotlib.pylab.title(title)
    grid(True)

    # left Y axis

    ax1=subplot(111)

    points1, = ax1.plot_date(time_v, wa.n_invalid_workers,'ro-')#ax1.plot_date(time_v, wa.n_invalid_workers,'-r')
    points1.set_markersize(3)
    points1.set_markeredgecolor('r')
    points1.set_markerfacecolor('w')

    points2, = ax1.plot_date(time_v, wa.n_workers,'bo-')#ax1.plot_date(time_v, wa.n_workers,'-b')
    points2.set_markersize(3)
    points2.set_markeredgecolor('b')
    points2.set_markerfacecolor('b')

    # plot task execution (snapshots)
    points3, = ax1.plot_date(time_v,ta.n_snapshots_per_time_unit,'sg')
    points3.set_markersize(3)
    points3.set_markeredgecolor('g')
    points3.set_markerfacecolor('w')

    if not ax1_limits:
        ax1_limits = (ax1.get_xlim(),ax1.get_ylim())


    #annotate('factory on',xy=(D(2008,7,15),800),xytext=(D(2008,7,5),900),
    #         arrowprops=dict(facecolor='red', shrink=0.15),
    #         horizontalalignment='left', verticalalignment='center')


    leg_loc = 'upper left'
    

    def mevent(text,month,day,y,h=0):
        annotate('$'+text+'$',xy=(datetime(2008,month,day,h),y),xytext=(datetime(2008,month,day,h),y+100),
                 size=18,arrowprops=dict(frac=0.4,facecolor='red', shrink=0.15),
                 horizontalalignment='center', verticalalignment='center')

    day_ticks = [15]

    print 'run_no',run_no
    if run_no == 1:
        day_ticks = [1,10]

    if run_no == 2:
        annotate('$f_{scale} = 1$',
                 xy=(datetime(2008,6,25),600),
                 xytext=(datetime(2008,6,29),900),
                 xycoords='data',textcoords='data',
                 size=14,arrowprops=dict(shrink=0,width=0.1,facecolor='black',headwidth=5,frac=0.05),
                 horizontalalignment='left',verticalalignment='center')

        annotate('$f_{scale} = 1.5$',
                 xy=(datetime(2008,7,19),1000),
                 xytext=(datetime(2008,7,2),1300),
                 xycoords='data',textcoords='data',
                 size=14, arrowprops=dict(shrink=0,width=0.1,headwidth=5,facecolor='black',frac=0.05),
                 horizontalalignment='left',verticalalignment='center')


        annotate('',
                 xy=(datetime(2008,6,19),630),
                 xytext=(datetime(2008,7,1,12),1280),
                 xycoords='data',textcoords='data',
                 size=14, arrowprops=dict(shrink=0,width=0.1,facecolor='black',headwidth=5,frac=0.05),
                 horizontalalignment='left',verticalalignment='center')


        mevent('F_{800}',7,15,800)
        mevent('F_{1200}',7,18,1200)
        mevent('E',7,21,700)
        mevent('F_{1450}',7,23,1450)
        
        day_ticks = [1,15]

    if run_no == 3:
        mevent('F_{1063}',8,5,1050)
        mevent('P',8,14,800,h=12)
        leg_loc = 'lower right'
        day_ticks = [5,15,25]

    if run_no == 4:
        mevent('F_{1063}',8,27,1063)
        mevent('D',9,7,800)
        mevent('E',9,19,400)
        mevent('R',9,21,600)
        leg_loc = 'center right'
        day_ticks = [1,15]

    # right Y axis 
    ax2=twinx()
    ax2.yaxis.tick_right()
    ax2.xaxis.set_visible(False) # switch off duplicated labels
    
    def ff(x,pos):
        return "%d"%(x/1000)
    sf = FuncFormatter(ff)
    #sf.set_scientific(True)
    ax2.yaxis.set_major_formatter(sf)

    ax2.set_ylabel('Total iterations x $10^3$')

    points4, = ax2.plot_date(time_v, ta.n_snapshots,'-g')
    points4.set_linewidth(2)

    # the axis limits must go after last plot but before the ax1 formatters!
    ax1.set_xlim(ax1_limits[0]) 
    ax1.set_ylim(ax1_limits[1]) # reset limits (right axis seems to influence left axis limits)

    ax1.xaxis.set_major_locator(DayLocator(day_ticks))
    ax1.xaxis.set_major_formatter(DateFormatter('%d/%m/%Y'))
    ax1.xaxis.set_minor_locator(DayLocator())
    setp(ax1.get_xticklabels(),fontsize=12)

    ax1.set_ylabel('Worker pool size')

    from matplotlib.font_manager import FontProperties
    leg = ax1.legend((points1,points2,points3,points4),
                     ('invalid workers','active workers','iterations/h','total iterations'),
                     leg_loc,markerscale=5, shadow=False,prop=FontProperties(size='small'))

    savefig('lqcd-run%d'%run_no)

    print "saved figure", 'lqcd-run%d'%run_no

    #frame = leg.get_frame()
    #frame.set_facecolor('0.80')

    #for t in leg.get_texts():
    #    t.set_fontsize('small')

run_no = None

if 'run1' in analyzer.filename:
    run_no = 1
if 'run2' in analyzer.filename:
    run_no = 2
if 'run3' in analyzer.filename:
    run_no = 3
if 'run4' in analyzer.filename:
    run_no = 4

from diane.analysis_tools.pframework import choose_and_call, Parameter, ChoiceParameter, FileInputStream

#input_file = Parameter('input_file','')
#if input_file.value:
#    diane.analysis_tools.pframework.setInputStream(FileInputStream(input_file.value))

if not run_no:
    run_no = Parameter('run_no',1).value

import matplotlib

matplotlib.rcParams['savefig.dpi'] = 300

combined_plot(analyzer,run_no)

#combined_plot(analyzer,title='LQCD Run 1')
#combined_plot(analyzer,title='LQCD Run 2',ax1_limits=[0,1600])
#combined_plot(analyzer,title='LQCD Run 3')
#combined_plot(analyzer,title='LQCD Run 4')

show()

#clf()
