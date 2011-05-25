import sys

def match(label,s):
    return s.startswith(label)

def getval(s,sep='=',i=-1):
    return s.split(sep)[i].strip()

memUNITS = {"kB":1000}
def getmem(s):
    mem,memunit = [x.strip() for x in getval(s,":").split()]
    return int(mem) * memUNITS[memunit]

def scan(f):
    data = {"CPU_NUM":0}
    cnt = 0
    for l in f:
        if cnt == 1:
            data['HOSTNAME'] = l.strip()
            
        if match("HOSTNAME",l):
            data["HOSTNAME"] = getval(l)
        if match("OSG_HOSTNAME",l):
            data["OSG_HOSTNAME"] = getval(l)
        if match("GLOBUS_CE",l):
            data["GLOBUS_CE"] = getval(l)
        if match("GANGA_LCG_CE",l):
            data["GANGA_LCG_CE"] = getval(l)

        if match("GLOBUS_GRAM_JOB_CONTACT",l):
            data["GLOBUS_GRAM_JOB_CONTACT"] = getval(l)
            
        if match("cpu MHz",l):
            cpuMHz = float(getval(l,":"))
            data.setdefault("CPUMHz",[])
            #try:
            #    if data["CPUMHz"] != cpuMHz:
            #        print >> sys.stderr, "WARNING: NOT HOMOGENEOUS CPU MHZ",f.name
            #except KeyError:
            #    data["CPUMHz"] = cpuMHz
            data['CPUMHz'].append(cpuMHz)
            data["CPU_NUM"] += 1
        if match("MemTotal",l):            
            data['MemTotal'] = getmem(l)

        if match("MemFree",l):            
            data['MemFree'] = getmem(l)
        cnt += 1

    def check(*keys):
        ok = False
        for k in keys:
            ok = ok or data.has_key(k)
        if not ok:
            print >> sys.stderr, "WARNING: NO %s"%repr(keys), f.name

    check('GANGA_LCG_CE','GLOBUS_CE','OSG_HOSTNAME','GLOBUS_GRAM_JOB_CONTACT')
    check('HOSTNAME')
    
    return data

import glob

D = []
RUNS = [33,34,35] # this is RUN 2
cnt = 0
FLIST = None

#RUNS = [41] # this is RUN 4
#FLIST = []

#for wid in eval(file('RUN_%d_INVALID_WORKERS_IDS.dat'%RUNS[0]).read()):
#    FLIST.append('/data/lqcd2008/runs/00%d/%05d/vcard.txt'%(RUNS[0],wid))

RUNS = [41]
FLIST = None

for RUN in RUNS:
    if not FLIST:
        FLIST = glob.glob('/data/lqcd2008/runs/00%d/*/vcard.txt'%RUN)

    for fn in FLIST:
     d = scan(file(fn))
     D.append(d)
     if cnt%100 == 1:
         print 'processed files',cnt
     cnt += 1

    file("RUN_%d_vcard_summary.dat"%RUN,'w').write(repr(D))
    FLIST = None



