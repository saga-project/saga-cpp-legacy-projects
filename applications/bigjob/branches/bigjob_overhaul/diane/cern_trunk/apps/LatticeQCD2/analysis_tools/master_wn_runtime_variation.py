from diane.analysis_tools import Histogram1D
from utils import read_data

iters,betas = read_data()

workers = {}

for B in betas:
    for t in iters[B]:
        wid = t[2]
        try:
            h = workers[wid]
        except KeyError:
            h = Histogram1D(0,50000,100,'execution time of subsequent tasks on the same worker node (master.log)')
            workers[wid] = h
        h.add(t[1]-t[0])

dispersion = Histogram1D(0,1,100,'relative std deviation of the execution time of subsequent iterations on the same worker node (master.log)')
samples = Histogram1D(0.5,50.5,50,'number of subsequent iterations on the same worker node (master.log)')
#absdisp = Histogram1D(0,1000,100)
for w in workers:
    h = workers[w]
    if h.hits > 1:
        dispersion.add(h.stddev/h.mean)
        samples.add(h.hits)
#    absdisp.add(h.stddev)

dispersion.show()
samples.show()
#absdisp.show()

#h = workers[workers.keys()[1]]
#h.show()
#print h.stddev/h.mean
        
