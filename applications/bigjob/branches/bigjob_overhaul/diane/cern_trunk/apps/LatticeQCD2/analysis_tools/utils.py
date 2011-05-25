def read_data():
    iters = eval(file('execution_time.dat').read())

    betas = iters.keys()
    betas.sort()
    print 'all betas recodred in the file:',' '.join(betas)

    import sys
    if len(sys.argv) < 2:
        print 'specify a beta or "all"'
        sys.exit()

    BETA=sys.argv[1]

    if BETA == 'all':
        BETAS = betas
    else:
        BETAS = [BETA]

    return iters,BETAS

def generate_data():
    import os,sys
    import time
    import subprocess
    
    def timestamp(l):
        return time.mktime(time.strptime(l[0:19],'%Y-%m-%d %H:%M:%S'))

    def cmd(c):
        
        return subprocess.Popen(c, shell=True, stdout=subprocess.PIPE).communicate()[0].splitlines()
    def exec_time(BETA,SEED):
        iter = []

        ts = None
        wid = None
        lines = cmd("grep '%s,%s[,)]' master.log | grep -v ended"%(BETA,SEED))
        for l in lines:
            if l.find('scheduling') != -1:
                ts=timestamp(l)
                wid=int(l.split('->')[1].strip())
                #print ts,l
            if l.find('starting moving file set') != -1:
                #print '*',timestamp(l),l
                if ts is None:
                    print 'LINE DUMP'
                    print '\n'.join(lines)
                    print ''
                    print 'ERROR in line:',l
                    sys.exit(-1)
                iter.append((ts,timestamp(l),wid))
                ts = None
        return iter

    iters = {}
    snaps = cmd('grep /storage/lqcd/apps/output/dat master.log')
    cnt = 0
    for snap in snaps:
        if cnt%10 == 0:
            print "%d snapshots already processed out of %d"%(cnt,len(snaps))
        cnt += 1
        vals=snap.split()[-1].split('_')
        beta,seed = vals[-2],vals[-1]
        iters.setdefault(beta,[])
        iters[beta].extend(exec_time(beta,seed))

    file('execution_time.dat','w').write(str(iters))
    print 'written to file execution_time.dat'


def get_betas(fn):
    for l in file(fn).readlines()[:10000]: # first 10K lines (optimization for LQCD)
        if l.find('INFO: /storage/lqcd/apps/output/dat/snap') != -1:
             yield l.split()[-1].split('_')[-2:]
