

# test samples
#s1 = "2008-08-26 16:41:15,203 INFO: starting download session: /data/lqcd/apps/output/hmc_su3_newmuI2, session_uuid: 1225412189.32, peer_uuid: 1220094307.41"
#s2 = "2008-08-26 17:50:10,268 INFO: closing download session, status=OK: file=/data/lqcd/apps/output/dat/snap_0615_5.1805_6384, bytes=9437232, elapsed_time=3.587s, session_uuid=1278726775.76"
#lines = [s1,s2]

import re,time,sys

def filelog2journal(fn):

    """Read the file.log (fileserver log) and return an iterator of the
    journal entries. Usage: same as read_journal() function.

    In the future DIANE releases the file server fill create journal automatically
    and this converter will not be needed.
    """
    lines = file(fn).readlines()

    c_start_download = re.compile('(?P<time>[0-9 :,-]+) INFO: starting download session: (?P<fn>\S+), session_uuid: (?P<session_uuid>\S+), peer_uuid: (?P<peer_uuid>\S+)')

    c_stop_download = re.compile('(?P<time>[0-9 :,-]+) INFO: closing download session, status=(?P<status>\S+): file=(?P<fn>\S+), bytes=(?P<bytes>\S+), elapsed_time=(?P<elapsed_time>\S+)s, session_uuid=(?P<session_uuid>\S+)')

    c_start_upload = re.compile('(?P<time>[0-9 :,-]+) INFO: starting upload session: (?P<fn>\S+), session_uuid: (?P<session_uuid>\S+), peer_uuid: (?P<peer_uuid>\S+)')
    
    c_stop_upload = re.compile('(?P<time>[0-9 :,-]+) INFO: closing upload session, status=(?P<status>\S+): file=(?P<fn>\S+), bytes=(?P<bytes>\S+), elapsed_time=(?P<elapsed_time>\S+)s, session_uuid=(?P<session_uuid>\S+)')

    c_skip_download = re.compile('(?P<time>[0-9 :,-]+) INFO: md5_lazy: skipping download of file (?P<fn>\S+)')
    
    cs = {'start_download':c_start_download,'stop_download':c_stop_download,
          'start_upload':c_start_upload, 'stop_upload':c_stop_upload,
          'skip_download':c_skip_download}

    numeric_fields = ['bytes','elapsed_time']

    def mktime(d):
        dt,ms = d['time'].split(',')
        return time.mktime(time.strptime(dt,'%Y-%m-%d %H:%M:%S'))+float('0.'+ms)

    for line in lines:
        r = None
        for c in cs:
            r = cs[c].match(line)
            if r:
                break
        if not r: # unknown event
            continue
        d = r.groupdict()
        t = mktime(d)
        del d['time']

        for v in d:
            if v in numeric_fields:
                try:
                    d[v] = int(d[v])
                except ValueError:
                    try:
                        d[v] = float(d[v])
                    except ValueError:
                        print 'cannot convert to numeric value:',v,d[v],line
                        sys.exit(-1)    

        yield [t,c,d]


if __name__ == '__main__':
    try:
        fn = sys.argv[1]
    except IndexError:
        fn = 'file.log'
        print >> sys.stderr, "Usage: filelog2journal [file.log] [output_file]"
        print >> sys.stderr, """Convert the file.log to the journal format.
The name of the file.log may be ommited. If output_file is not specified
or '-' then print on stdout"""
        print >> sys.stderr
        print >> sys.stderr, "No input file, processing ",fn
        
    try:
        outfn = sys.argv[2]
    except IndexError:
        outfn = '-'

    if outfn == '-':
        outf = sys.stdout
    else:
        outf = file(outfn,'w')

    for e in filelog2journal(fn):
        print >> outf, repr(e)
