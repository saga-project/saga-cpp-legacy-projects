import sys
from diane.analysis_tools import *

#combine two journals (e.g. master.j, file.j) 
#so that event will appear in chronological order
#i.e. as if produced by the same process

iters = [read_journal(fn) for fn in sys.argv[1:]]

def test():
    global iters
    #test only ---------------------
    def mg(v):
        def gen():
            for x in v:
                yield x
        return gen()
    iters = [mg([102]),
             mg([100,200,300,400,500,600,700]),
             mg([110,210,330]),
             mg([101,102,910,920])]
    #test only ---------------------

terminated = []
queue = []

def cleanup(terminated):
    #print 'terminated',terminated
    for i in terminated:
        iters[i] = None
        queue[i] = None

for i,j in enumerate(iters):
    try:
        entry = j.next()
        queue.append((entry,i))
    except StopIteration:
        terminated.append(i)

cleanup(terminated)

while len(terminated) < len(queue):
    #print 'queue',queue
    # rely on the fact that entry[0] is time
    entry,i = min([e for e in queue if not e is None]) 
    print entry
    #process entry 
    try:
        queue[i] = (iters[i].next(),i)
    except StopIteration:
        terminated.append(i)
        cleanup([i])



