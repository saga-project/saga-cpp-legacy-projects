#
# analyze agent factory logs: https://twiki.cern.ch/twiki/bin/view/ArdaGrid/SU3
#

import os,sys,shutil,glob

CEs = {}
SITEs = {}

CROSS_TYPE_ANALYSIS = sys.argv[1] == 'all_processed'
CTA_CEs = {}
CTA_SITEs = {}

for fn in file(sys.argv[1]).readlines():
    if CROSS_TYPE_ANALYSIS:
        fn2 = fn.strip()
    else:    
        fn2 = os.path.dirname(os.path.dirname(fn))

    fn3= 'processed/'+os.path.basename(fn2)

    for l in file(fn3+'/full_print').readlines():
        if l.find('actualCE') != -1:
            ce = l.split('=')[1].strip()[1:-3]
            CEs.setdefault(ce,0)
            CEs[ce] += 1

            site = '.'.join(ce.split(':')[0].split('.')[1:])
            SITEs.setdefault(site,0)
            SITEs[site] += 1

            if CROSS_TYPE_ANALYSIS:
                for idfn in glob.glob('processed?*'):
                    if file(idfn).read().find('/'+fn2+'/') != -1:
                        CTA_CEs.setdefault(ce,{})
                        CTA_CEs[ce].setdefault(idfn,0)
                        CTA_CEs[ce][idfn] += 1

                        CTA_SITEs.setdefault(site,{})
                        CTA_SITEs[site].setdefault(idfn,0)
                        CTA_SITEs[site][idfn] += 1                        
         

def ppr(d):
    print len(d)
    z = zip(d.values(),d.keys())
    z.sort()
    z.reverse()
    for x in z:
        print x

fail_classes = {4: 'net',  8: 'net', 9 : 'net', 910: 'net', 1 : 'exec', 2 : 'exec', 3 : 'exec' }
    
def pprCTA(d):
    print len(d)
    items = {}
    for x in d:
        items[x] = []
        totcnt = 0
        tfcnt = {}
        clscnt = {'net':0,'exec':0,'other':0}
        #print '* %70s'%x,        
        for dd in d[x]:
            ftn = int(dd[len('processed'):])
            failcnt = int(d[x][dd])
            #print "T%03d=%03d, " %(ftn,failcnt),
            tfcnt[ftn]=failcnt 
            try:
                cls = fail_classes[ftn]
            except KeyError:
                cls = 'other'
            clscnt[cls] += failcnt
            totcnt += failcnt

        items[x] =  (totcnt,clscnt,tfcnt)   

    sorted_items = zip(items.values(),items.keys())
    sorted_items.sort()
    #sorted_items.reverse()
    for y,x in sorted_items:
        print '* %70s'%x,
        print '%4d'%items[x][0],
        clscnt = items[x][1]
        for cls in clscnt:
            if clscnt[cls]:
                print 'C.%s=%d,'%(cls,clscnt[cls]),
        tfcnt = items[x][2]
        for tf in tfcnt:
            print "T%03d=%03d, " %(ftn,failcnt),
        print
    return items
 
def save(fn,items):        
    f = file(fn,'w')
    f.write('# ([name : (total_failures, [fail_class:cnt], [fail_type:cnt]), see also: https://twiki.cern.ch/twiki/bin/view/ArdaGrid/SU3\n')
    f.write(repr(items))
    f.close()
        
if CROSS_TYPE_ANALYSIS:
    save('CE_failures',pprCTA(CTA_CEs))
    print
    save('SITE_failures',pprCTA(CTA_SITEs))

else:    
    ppr(CEs)
    print
    ppr(SITEs)
