

# test samples
s1 = "2008-06-15 00:08:29,804 INFO: new worker registered: wid=82, worker_uuid=1312462502.96"
s2 = "2008-06-15 00:09:42,168 INFO: scheduling (5.18175,7672,27) -> 94"
s3 = "2008-06-15 00:09:23,452 INFO: worker removed: wid=90"
s4 = "2008-06-15 02:33:21,794 INFO: starting moving file set (5.18275,10031)"

import re,time,sys

def masterlog2journal(fn):

    """Read the file.log (fileserver log) and return an iterator of the
    journal entries. Usage: same as read_journal() function.

    In the future DIANE releases the file server fill create journal automatically
    and this converter will not be needed.
    """
    lines = file(fn).readlines()

    c_register_worker = re.compile(r'(?P<time>[0-9 :,-]+) INFO: new worker registered: wid=(?P<wid>\S+), worker_uuid=(?P<worker_uuid>\S+)')

    c_task_scheduled = re.compile(r'(?P<time>[0-9 :,-]+) INFO: scheduling \((?P<beta>\S+),(?P<seed>\S+),(?P<iter>\S+)\) -> (?P<wid>\S+)')

    c_worker_removed = re.compile(r'(?P<time>[0-9 :,-]+) INFO: worker removed: wid=(?P<wid>\S+)')
    
    c_task_result_processed = re.compile(r'(?P<time>[0-9 :,-]+) INFO: starting moving file set \((?P<beta>\S+),(?P<seed>\S+)\)')
    
    cs = {'register_worker':c_register_worker,'task_scheduled':c_task_scheduled,
          'worker_removed':c_worker_removed, 'task_result_processed':c_task_result_processed}

    numeric_fields = ['wid','seed']

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
        fn = 'master.log'
        print >> sys.stderr, "Usage: masterlog2journal [master.log] [output_file]"
        print >> sys.stderr, """Convert the master.log to the journal format.
The name of the master.log may be ommited. If output_file is not specified
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

    for e in masterlog2journal(fn):
        print >> outf, repr(e)
