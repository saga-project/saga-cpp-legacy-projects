import glob,sys


from diane.analysis_tools.stats import Histogram1D, QuantileFinder

hCPU = Histogram1D(0.5,32.5,32,'number of processors per node')
hCPUavg = Histogram1D(500,6000,20,'average processor speed')
hCPUtotal = Histogram1D(500,25000,20,'total processor speed available on the system')

hCPUavginv = Histogram1D(1000./6000,1000./500,30,'CPUs in the Grid')
hCPUtotalinv = Histogram1D(1000./30000,1000./2000,30,'CPUs in the Grid')

CEs = []
HOSTs = []

HOSTCEs = []

CE_DOMAINs = {} 

CNT = 1

# invalid workers only
#FLIST = ['RUN_41_vcard_summary_INVALID_WORKERS.dat']
#LIMIT=0

# all workers in the time period  for which AF fail logs are available
FLIST = ['RUN_41_vcard_summary.dat']
LIMIT = 12069 #last wid to be processed

if not FLIST:
    FLIST = glob.glob("RUN_*_vcard_summary.dat")
    LIMIT = 0

for fn in FLIST:
    print 'analyzing',fn
    wlist = eval(file(fn).read())
    if LIMIT:
        wlist = wlist[:LIMIT]
        print 'applying LIMIT, last worker vcard=',LIMIT

    for d in wlist:

        def check(k):
            if not d.has_key(k):
                print >> sys.stderr,'problem: NO %s'%k,fn,d
                return False
            return True

        CE=""
        try:
            CE = d['GLOBUS_CE']
        except KeyError,x:
            pass #print x, fn, d

        CPU_NUM = d['CPU_NUM']
        CPU_ALL = reduce(lambda x,y:x+y, d['CPUMHz'])

        hCPU.fill(CPU_NUM)
        hCPUtotal.fill(CPU_ALL)
        hCPUavg.fill(CPU_ALL/CPU_NUM)
        hCPUavginv.fill(1000.0/(CPU_ALL/CPU_NUM))
        hCPUtotalinv.fill(1000.0/CPU_ALL)

        HOST=""
        if check('HOSTNAME'):
            HOST=d['HOSTNAME']

        HOSTs.append(HOST)
        CEs.append(CE)

        CE_DOMAIN = '.'.join((CE.split(':')[0]).split('.')[1:])
        CE_DOMAINs.setdefault(CE_DOMAIN,0)
        CE_DOMAINs[CE_DOMAIN] += 1

        HOSTCEs.append((HOST,CE_DOMAIN))

        CNT += 1

print "unique CEs",len(set(CEs))
print "all CEs",len(CEs)
print "unique HOSTs",len(set(HOSTs))
print "unique HOST/CE combination", len(set(HOSTCEs))

print "all jobs",CNT

hCPU.printout()
hCPUavg.printout()
hCPUtotal.printout()

#hCPU.plot()

CE_DOMAINs_sorted = zip(CE_DOMAINs.values(),CE_DOMAINs.keys())
CE_DOMAINs_sorted.sort()

print CE_DOMAINs

for i, ce in enumerate(CE_DOMAINs_sorted):
    print i, ce[0], ce[1]

#for i,ce in enumerate(set(CE_DOMAINs)):
#    print i,ce

#<<<<<<< analyze_vcard.py

print 'plotting'

#=======
from matplotlib import rcParams
#>>>>>>> 1.7
from matplotlib.pylab import *
matplotlib.rcParams['figure.subplot.left'] = 0.05
matplotlib.rcParams['figure.subplot.right'] = 0.95
matplotlib.rcParams['figure.figsize']= (10, 5)
matplotlib.rcParams['savefig.dpi'] = 300
ax1 = subplot(111)
ax1.set_yscale('log')
#<<<<<<< analyze_vcard.py
#print 'YSCALE:', ax1.get_yscale()
h = hCPUavginv
#h.bins = [b+0.001 for b in h.bins]
#=======
#ax1.set_ylim(1, 100000)
ax1.set_xlabel('time to complete $10^9$ CPU cycles, seconds')
#>>>>>>> 1.7
hCPUavginv.plot()
savefig('lqcd-run4-nominal-processors-node-statistics.png')



#hCPU.plot()

#ax2 = subplot(212)
#hCPUavginv.plot()
#hCPUtotalinv.plot()
#hCPUavg.plot()

show()
