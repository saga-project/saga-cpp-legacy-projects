import datetime

from os.path import join
from pylab import *

PATH = None

for arg in sys.argv:
    if arg.startswith('--gangadir='):
        PATH = join(arg.replace('--gangadir=', ''), 'agent_factory', 'failure_log')

if PATH is None:
    try:    
        import Ganga.GPI
    except ImportError:
        print 'Unable to import Ganga.GPI. Be sure to either run history-plot.py as a Ganga script or use --gangadir option to specify the gangadir directly.'
        sys.exit(1)
        
    PATH = join(Ganga.GPI.config.Configuration.gangadir, 'agent_factory', 'failure_log')


lines = file(join(PATH, 'log')).readlines()

lines = filter(lambda x: '}' in x, lines)

data = {}

for l in lines:
    _dat = l.split()
    (y,m,d) = _dat[0].split('-')
    (h,min,s) = _dat[1].split(':')
    (s,ns) = s.split(',')
    time = datetime.datetime(int(y),int(m),int(d),int(h),int(min),int(s),int(ns))
    name = _dat[4][:-1]
    fitness = float(_dat[5][:-1])
    running = int(_dat[6][:-1])
    completed = int(_dat[7][:-1])
    total = int(_dat[8])

    try:
        data[name]
    except:
        data[name] = []

    data[name].append((time, fitness, running, completed, total))

print 'Generating plots...'

#plot a graph for each computing element
for ce in data:
    print 'Generating plot for', ce
    if len(data[ce]) < 10:
        continue
    time, fitness, running, completed, total = [], [], [], [], []
    max_total = 0
    min_date, max_date = datetime.datetime.now(), datetime.datetime(1900, 1, 1)
    for d in data[ce]:
        time.append(d[0])
        fitness.append(d[1])
        running.append(d[2])
        completed.append(d[3])
        total.append(d[4])
        if d[4] > max_total:
            max_total = d[4]
        if d[0] > max_date:
            max_date = d[0]
        if d[0] < min_date:
            min_date = d[0]

    fig = figure()
    ax = fig.add_subplot(211)
    title(ce.split('/')[0])
    #plot(time, fitness, 'b,')
    plot(time, fitness, 'b.', markersize=4)
    ylabel('fitness')
    xlim([min_date, max_date])
    ax.xaxis.set_major_locator(DayLocator(interval=3))
    ax.xaxis.set_major_formatter(DateFormatter('%d/%m'))
    #ax.xaxis.set_minor_locator(HourLocator())
    xlabel('time')
    ylim([-0.5, 1.5])
    grid(True)
    #ax.autoscale_view()
    xlabel('time')

    bx = subplot(212)
    #plot(time, running, 'g,', time, total, 'b,', time, completed, 'r,')
    plot(time, running, 'g.', time, total, 'b.')
    #xs, ys = poly_between(time, running, total)
    #bx.fill(xs, ys, facecolor='blue', alpha=0.7)
    xs, ys = poly_between(time, 0, running)
    bx.fill(xs, ys, facecolor='green', alpha=0.5, edgecolor='green', linewidth=0)
    xlim([min_date, max_date])
    bx.xaxis.set_major_locator(DayLocator(interval=3))
    bx.xaxis.set_major_formatter(DateFormatter('%d/%m'))
    #bx.xaxis.set_minor_locator(HourLocator())
    ylim([-1,max_total + max(int(max_total*0.2), 1)])
    ylabel('total/running jobs')
    xlabel('time')
    grid(True)

    savefig(ce.split('/')[0].split(':')[0] + '.png')
