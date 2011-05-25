# faster but less compatible
import re
import sys

import time

t1 = time.time()
cnt = 0
lines = 0
c = re.compile('.+bytes\=(\d+).+')
for l in file(sys.argv[1]):
    r = c.match(l)
    if r:
        cnt += int(r.group(1))
        lines += 1

print 'total files',lines
print 'total bytes',cnt

print 'time:',time.time()-t1

t1 = time.time()

## # slower but journal-compliant implementation
## import diane.analysis_tools
## cnt = 0
## lines = 0
## for e in diane.analysis_tools.filelog2journal(sys.argv[1]):
##      try:
##          bytes = e[2]['bytes']
##          lines += 1
##          cnt += bytes
##      except KeyError:
##          pass

## print 'total files',lines
## print 'total bytes',cnt
## print 'time:',time.time()-t1

    
