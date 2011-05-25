from LatticeQCD2.analysis_tools.analyzers import main
import sys

analyzers = []

for fn in ['data/run%d/run.j'%d for d in range(1,5)]:
    analyzer = main(['',fn]+sys.argv[2:])
    analyzers.append(analyzer)


import matplotlib.pylab
from matplotlib.pylab import *

def savefig(*args,**kwds):
    print 'savefig',args,kwds
    return matplotlib.pylab.savefig(*args,**kwds)

def unzip(i,l):
    return [x[i] for x in l]


##########################################

def save_task_duration_histogram():

    for i in range(0,4):
        a = analyzers[i]
        hists = a.task_execution.task_duration_histogram
        for b in hists:
            name = 'tmp.iteration-%d-task_duration_histogram_%s'%(i+1,b)
            ftxt = file(name+'.txt','w')
            fdat = file(name+'.dat','w')
            h = hists[b]
            h.printout(ftxt)
            for p in h.points():
                print >> fdat, p[0],p[1]
            print 'saved .txt and .dat files for',name

def plot_file_transfer_histograms():
    run_no = Parameter('run_no',1,doc='Run number (1,2,3,4)').value
    file_transfer_histograms(analyzers[run_no-1])

def file_transfer_histograms(analyzer):
    uh = analyzer.task_execution.snapshot_upload_duration
    dh = analyzer.task_execution.snapshot_download_duration

    subplot(2,1,1)
    uh.plot()
    subplot(2,1,2)
    dh.plot()
    

def plot_task_overhead_histogram():
    run_no = Parameter('run_no',1,doc='Run number (1,2,3,4)').value
    analyzer = analyzers[run_no-1].task_execution
    analyzer.task_master_overhead.plot()

def task_duration_histogram(analyzer):
    hists = analyzer.task_execution.task_duration_histogram

    all_betas = [(float(b),b) for b in hists.keys()]
    all_betas.sort()
    all_betas.reverse()
    all_betas = [b[1] for b in all_betas]

    print all_betas

    betas = []
    lines = []
    
    maxy = 0

    def pl(beta,fmt):
        lines.append(hists[beta].plot(fmt,annotate=False))
        betas.append(beta)
        return max(hists[beta].bins)

    #grayscale histograms
    i = 1
    step = 0.5/len(all_betas)*3
    for beta in all_betas[::3]:
        color = .2+step*i
        if i == 1: #len(all_betas):
            color = 0
        print beta,color
        maxy = max(pl(beta,str(color)),maxy)
        i += 1

    #pl(all_betas[0],'0') # plot the first beta (so it is "above" the others)

    gca().set_ylim(0,maxy)

    return lines,betas

def plot_task_duration_histogram():
    axes([0.1,0.1,0.71,0.8])

    for i in range(0,4):
        subplot(2,2,i+1)
        lines,betas = task_duration_histogram(analyzers[i])
        title('Distribution of iteration time (run %d)'%(i+1))
        legend(lines,betas,loc=(1.03,0.2))


def task_duration_histogram_mean(analyzer,fmt):
    hists = analyzer.task_execution.task_duration_histogram

    all_betas = [(float(b),b) for b in hists.keys()]
    all_betas.sort()
    all_betas.reverse()
    all_betas = [b[1] for b in all_betas]

    import copy
    h = copy.deepcopy(hists[all_betas[0]])

    for beta in all_betas[1:]:
        h.addHistogram(hists[beta])

    #normalize histogram
    maxbin = max(h.bins)
    for i in xrange(len(h.bins)):
        h.bins[i] /= float(maxbin)

    return h.plot(fmt),h

    #pl('5.18525','r-')
    #pl('5.1830','g-')
    #pl('5.1805','k-')

    #for beta in ['5.18525','5.1830','5.1805']:
    #    lines[beta] = hists[beta].plot('r-')


def plot_task_duration_histogram_mean():
    lines = []
    hists = []
    for i,fmt in zip(range(4),['r-','g-','b-','k-']):
        line,h = task_duration_histogram_mean(analyzers[i],fmt)
        lines.append(line)
        hists.append(h)

    title('Normalized distribution of iteration execution time')
    legend(lines,['run %d (mean: %.2fh)'%(d,hists[d-1].mean/3.0/3600.0) for d in range(1,5)])


##########################################

def beta_execution_time(analyzer,run_no,what_to_plot,fmt,remove_inner_bins=0,make_x_ticks=1):
    """
    remove_inner_bins: 0=no, 1=yes, 2=auto(depends on the run number)
    make_x_ticks: same logic
    The point is to use this function for overlaying plots and for individual subplots
    """
    hists = analyzer.task_execution.task_duration_histogram

    if remove_inner_bins == 2: #automode
        remove_inner_bins = run_no == 1

    if remove_inner_bins:
        for beta in hists.keys():
            if len(beta) == 7:
                del hists[beta]

    # compute mode (max)
    def hist_max(h):
        xm,ym = h.points()[0]
        for x,y in h.points():
            if y > ym:
                xm = x
                ym = y
        return xm,ym
    
    all_betas = [(float(b),b,
                  hists[b].mean,hists[b].stddev,
                  hist_max(hists[b])[0],
                  hists[b].qfinder.quantiles[50],hists[b].qfinder.quantiles[25],hists[b].qfinder.quantiles[75],
                  0) for b in hists.keys()]

    
    all_betas.sort()
    #all_betas.reverse()

    for x in all_betas:
        print 'run %d, beta %s, mean %f, stddev %f, mode %f, median %f' % (run_no,x[1],x[2],x[3],x[4],x[5])


    xlocations = unzip(0,all_betas)
    labels = unzip(1,all_betas)

    OPTION = what_to_plot

    if OPTION == 'mean':
        yi = 2 #MEAN
        si1 = -1 #3 = STDDEV
        si2 = -1

    if OPTION == 'mode':
        yi = 4 #MODE
        si1 = -1
        si2 = -1

    if OPTION == 'median':
        yi = 5 #MEDIAN
        si1 = 6
        si2 = 7

    ylocations = unzip(yi,all_betas)
    yspread1 = unzip(si1,all_betas) 
    yspread2 = unzip(si2,all_betas) 


    print 'run %d betas %d'%(run_no,len(xlocations))
    points, = plot(xlocations,
                   ylocations,
                   fmt)
    vls = 'dotted'
    if si2 != -1:
        vlines(xlocations,ylocations,yspread2,linestyles=vls)
    if si1 != -1:
        vlines(xlocations,yspread1,ylocations,linestyles=vls)


    for i in range(len(labels)):
        if i%3 != 0:
            labels[i] = ""

    if make_x_ticks == 2:
        make_x_ticks = run_no == 1
    if make_x_ticks: # just do ticks once, using all beta range (the case of run 1)
        xt = xticks(xlocations,labels)
        setp(gca().get_xticklabels(),fontsize=9)

    return points


def plot_beta_execution_time():
    run_no = Parameter('run_no',1,doc='Run number (1,2,3,4)').value
    what_to_plot = ChoiceParameter('what_to_plot','median',['mean','mode','median']).value
    fmt = Parameter('line_format','ro-').value
    beta_execution_time(analyzers[run_no-1],run_no,what_to_plot,fmt)
    title('Iteration time per beta (%s)'%what_to_plot)
    gca().set_xlabel('$\\beta$')
    gca().set_ylabel('time [s]')

def plot_beta_execution_time_all_runs():

    fmts = ['ro-','gs-','b+-','k^-']

    subplots = Parameter('subplots',False).value
    what_to_plot = ChoiceParameter('what_to_plot','median',['mean','mode','median']).value

    if subplots:
        matplotlib.rcParams['figure.figsize'] = (14,10)
        matplotlib.rcParams['figure.subplot.left'] = 0.08
        matplotlib.rcParams['figure.subplot.bottom'] = 0.05
        matplotlib.rcParams['figure.subplot.right'] = 0.95
        matplotlib.rcParams['figure.subplot.top'] = 0.95

    points = []
    for i in range(4):
        if subplots:
            subplot(2,2,i+1)
            remove_inner_bins = 0
            make_x_ticks = 1
            title('Run %d'%(i+1))
        else:
            remove_inner_bins = 2 # auto
            make_x_ticks = 2

        gca().set_xlabel('$\\beta$')
        gca().set_ylabel('time [s]')
        points.append(beta_execution_time(analyzers[i],i+1,what_to_plot,fmt=fmts[i],remove_inner_bins=remove_inner_bins,make_x_ticks=make_x_ticks))

    if not subplots:
        legend(points,['Run %d'%(i+1) for i in range(4)])
        title('Iteration time per beta (%s)'%what_to_plot)
        gca().set_xlabel('$\\beta$')
        gca().set_ylabel('time [s]')

    savefig('lqcd-task-execution-time-per-beta')


def plot_beta_exectime_histogram(analyzer):
    hists = analyzer.task_execution.task_duration_histogram

    nb = len(hists)

    all_betas = [(float(b),b) for b in hists]
    all_betas.sort()
    for i in range(len(all_betas)):
        b,bname = all_betas[i]
        if i>8:
            break
        subplot(3,3,i+1)
        hists[bname].title = '%s'%bname
        hists[bname].plot(annotate=False)


def plot_beta_exectime_histogram_low_high():
    run_no = Parameter('run_no',1,doc='Run number (1,2,3,4)').value
    analyzer = analyzers[run_no-1]
    subplot(2,1,1)
    analyzer.task_execution.task_duration_histogram_low_beta.plot()
    ax2 = subplot(2,1,2)

    #h2 = analyzer.task_execution.task_duration_histogram_high_beta
    #h2.plot()

    betas = analyzer.task_execution.task_duration_histogram.keys()
    betas = sort(betas)
    maxbeta = max(betas)
    print betas, maxbeta
    h2 = analyzer.task_execution.task_duration_histogram[maxbeta]
    h2.plot()

    def overlay_empf(t0):
        #overlay empirical function
        import math
        #t0 = h2.min*1.5
        def empf(t):
            if t < t0: 
                return 0
            else:
                return math.pow(t/t0-1.0,1.5)*math.exp(-3*t/t0)

        xb = h2.xbinpos()
        print xb
        yb = [empf(t) for t in xb]
        print yb
        scale_f = max(h2.bins)/float(max(yb))
        print 'scale_f',scale_f
        #scale_f = 1782655.0
        yb = [y*scale_f for y in yb]
        print yb
        ax2.plot(xb,yb,'-')

    t0s = [1.2,1.25,1.3,1.4,1.45,1.5,1.55,1.6]
    t0s = [1.3,1.35,1.4]
    for t0 in t0s:
        overlay_empf(t0*h2.min)


##########################################       
from diane.analysis_tools.stats import barplot

def beta_completion(analyzer,run_no=None):

    total_iterations = analyzer.total_iterations.copy()

    if run_no==1: # remove inner bins
        for beta in total_iterations.keys():
            if len(beta) == 7:
                del total_iterations[beta]

    bars = zip([float(x) for x in total_iterations.keys()],total_iterations.values(), 
               total_iterations.keys())

    bars.sort()

    xlocations=unzip(0,bars)
    ylocations=unzip(1,bars)
    labels = unzip(2,bars)

    width = min([(xlocations[i+1]-xlocations[i]) for i in range(len(xlocations)-1)])
    width /= 2.0

    COLOR1 = '#dddddd'
    COLOR2 = '#ff2222' # in sensitive region

    # use different color for sensitive region
    color = [COLOR1]*len(xlocations)

    if run_no != 1:
        for i in range(2,18):
            try:
                color[i] = COLOR2
            except IndexError:
                break

    if run_no in [3,4]:
        color[0] = COLOR2
        color[1] = COLOR2

    bar(xlocations,ylocations,width,align='center',color=color)

    if run_no == 1:
        for i in range(len(labels)):
            if i%3 != 0:
                labels[i] = ""
    else:
        if run_no in [3,4]:
            ticks = [0,2,5,8,11,14,17,len(labels)-1]
        else: # run 2
            ticks = [0,2,7,12,17,20,len(labels)-1]

        for i in range(len(labels)):
            if not i in ticks:
                labels[i] = ""

    xt = xticks(xlocations,labels)
    setp(gca().get_xticklabels(),fontsize=12)
    #setp(gca().get_yticklabels(),fontsize=9)
    gca().set_xlabel("$\\beta$")
    gca().set_ylabel("iterations")

def plot_beta_completion():
    matplotlib.rcParams['figure.figsize'] = (14,10)
    matplotlib.rcParams['figure.subplot.left'] = 0.08
    matplotlib.rcParams['figure.subplot.bottom'] = 0.05
    matplotlib.rcParams['figure.subplot.right'] = 0.98
    matplotlib.rcParams['figure.subplot.top'] = 0.95

    for i in range(0,4):
        subplot(2,2,i+1)
        title('Run %d'%(i+1))
        beta_completion(analyzers[i].task_execution,run_no=i+1)

    savefig('lqcd-iterations-per-beta')

    #PENDING: how to set main title?   
    #title('Completed iterations per beta')

def plot_worker_lifetime():
    
    from diane.analysis_tools.stats import Histogram1D, QuantileFinder    


    def plot(a):
        h = Histogram1D(150000,200000,100,'worker lifetime (run %d)'%(i+1))
        for w in a.worker_history.workers.values():
            if w.removed != 0 and w.registered != 0:
                if w.task_cnt > 0:
                    h.fill(w.removed-w.registered) #,w.task_cnt

        h.plot()

    for i in range(4):
        subplot(2,2,i+1)
        a = analyzers[i]
        plot(a)


        
from diane.analysis_tools.pframework import choose_and_call, Parameter, ChoiceParameter,setInputStream, FileInputStream

import matplotlib

matplotlib.rcParams['savefig.dpi'] = 300

input_file = Parameter('input_file','',env=True)
if input_file.value:
    setInputStream(FileInputStream(input_file.value))

choose_and_call('plot',plot_task_duration_histogram,
                [ plot_task_duration_histogram,
                  save_task_duration_histogram,
                  plot_file_transfer_histograms,
                  plot_task_duration_histogram_mean,
                  plot_beta_execution_time_all_runs,
                  plot_beta_execution_time,
                  plot_beta_completion,
                  plot_beta_exectime_histogram_low_high,
                  plot_worker_lifetime,
                  plot_task_overhead_histogram
                  ])
                



#plot_beta_execution_time(subplots=False)
#plot_beta_exectime_histogram(analyzers[0])


#plot_beta_completion()
show()


