import sys
from diane.analysis_tools import *

#concatenate master journals in a way which preserves the uniqueness of wid
#the journal files should be ordered chronologically

wid_offset = 0
wid_step = 100000
max_wid = 0

for fn in sys.argv[1:]:
    print >> sys.stderr, "processing",fn,'wid_offset=',wid_offset
    for e in read_journal(fn):
        (time,event,params) = e
        try:
            max_wid = max(params['wid'],max_wid)
            if wid_offset:
                params['wid'] += wid_offset
        except KeyError:
            pass
        print e

    e[0] += .1
    e[1] = 'master_shutdown'
    e[2] = {}
    print e
    wid_offset += wid_step
