import datetime, os, sys, time

from os.path import exists, join, isdir, split
from os import listdir

PATH = None

for arg in sys.argv:
    if arg.startswith('--gangadir='):
        PATH = join(arg.replace('--gangadir=', ''), 'agent_factory', 'failure_log')

if PATH is None:
    try:
        import Ganga.GPI
    except ImportError:
        print 'Unable to import Ganga.GPI. Be sure to either run failure-log-report.py as a Ganga script or use --gangadir option to specify the gangadir directly.'
        sys.exit(1)
    PATH = join(Ganga.GPI.config.Configuration.gangadir, 'agent_factory', 'failure_log')

USAGE = """
Usage: ganga select.py [OPTION]
List information about failed jobs satisfying specified conditions (all failed jobs by default). Requires to be run as a Ganga script to automatically discover the gangadir and the failure log. Otherwise, one can specify the gangadir directly via --gangadir=DIR option. 

Triggers:
--with-loginfo / --without-loginfo      with / without loginfo information
--with-stdout / --without-stdout        with / without stdout or stdout.gz files
--with-stderr / --without-stderr        with / without stderr or stderr.gz files
--with-outputdir / --without-outputdir  with / without output directory

Options:
--gangadir=GANGADIR     check the specified gangadir
 -H                     print running time in hours instead of seconds

Instead of job id print the following:
--run-time              job id and running time for each time the job enters "Running" status
--run-time-summary      job id and aggregated running time
--report                simple report: number of jobs, total running time, number of false positives, etc
"""

if '--help' in sys.argv or '-h' in sys.argv:
    print USAGE
    sys.exit(0)

HOURLY = '-H' in sys.argv

TRIGGERS = ['--with-loginfo', '--with-stdout', '--with-stderr', '--with-outputdir', '--without-loginfo', '--without-stdout', '--without-stderr', '--without-outputdir']

def check(job, trigger):
    opts = {
        '--with-loginfo' : lambda x: exists(join(PATH, job, 'loginfo')),
        '--with-stdout' : lambda x: exists(join(PATH, job, 'output', 'stdout')) or exists(join(PATH, job, 'output', 'stdout.gz')),
        '--with-stderr' : lambda x: exists(join(PATH, job, 'output', 'stderr')) or exists(join(PATH, job, 'output', 'stderr.gz')),
        '--with-outputdir' : lambda x: exists(join(PATH, job, 'output')),
        '--without-loginfo' : lambda x: not opts['--with-loginfo'](x),
        '--without-stdout' : lambda x: not opts['--with-stdout'](x),
        '--without-stderr' : lambda x: not opts['--with-stderr'](x),
        '--without-outputdir' : lambda x: not opts['--with-outputdir'](x)
    }
    return opts[trigger](job)

def time2str(time):
    if HOURLY:
        return '%d' % (time/3600)
    return '%d' % time

# get the list of failed jobs stored in agent_factory/failure_log directory
jobs = map( lambda x: split(x)[1], filter( isdir, map(lambda x: join(PATH, x), listdir(PATH)) ) )

stuff = []

trig = [t for t in TRIGGERS if t in sys.argv]

for j in jobs:
    if reduce(lambda x, y: x and y, [check(j,t) for t in trig], True):
        stuff.append(j)

good_jobs = []

# check which jobs were in 'Running' status and for how long
for job in stuff:
    log = join(PATH, job, 'loginfo')
    if exists(log):
        data = file(log).readlines()
        for d in range(len(data)):
            try: # necessary for index method
                data[d].index('Running')
                index = 0
                # search for the first timestamp, just after the job enters Running status
                while index < 10:
                    index += 1
                    if data[d+index].startswith('- timestamp'):
                        _from = data[d+index].split()[3:]
                        break
                # search for the next timestamp
                while index < 20:
                    index += 1
                    if data[d+index].startswith('- timestamp'):
                        _to = data[d+index].split()[3:]
                        break
                # convert to time structure (9-tuple)
                _from = time.strptime(' '.join(_from), '%a %b %d %H:%M:%S %Y')
                _to   = time.strptime(' '.join(_to), '%a %b %d %H:%M:%S %Y')
                # compute and store the time delta
                delta = time.mktime(_to) - time.mktime(_from)
                good_jobs.append((job, delta))
            except:
                    pass

summary = []

if len(good_jobs) > 0:
    # sort the jobs by name
    good_jobs.sort()
    current = good_jobs[-1][0]
    run_time = 0
    for (job, rt) in good_jobs:
        if current != job:
            summary.append((current, run_time))
            current = job
            run_time = 0
        run_time += rt

if '--report' in sys.argv:
    print 80 * '-'
    print 'REPORT'
    print 80 * '-'
    print '* Jobs total:', len(jobs)
    print '* Triggers:', ' '.join(trig)
    print '* Jobs found:', len(stuff)
    print '* (Possible) False positives:', len(summary)
    print '* Total running time (false positives):', time2str(sum([rt for (j,rt) in summary]))
    print 80 * '-'
    sys.exit(0)

if '--run-time-summary' in sys.argv:
    for (job, rt) in summary:
        print job, time2str(rt)
    sys.exit(0)

if '--run-time' in sys.argv:
    for (job, rt) in good_jobs:
        print job, time2str(rt)
    sys.exit(0)

for j in stuff:
    print j
