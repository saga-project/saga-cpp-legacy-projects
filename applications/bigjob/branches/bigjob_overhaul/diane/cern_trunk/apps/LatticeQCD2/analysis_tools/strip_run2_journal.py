import sys

if sys.argv < 3:
    print """This script strips the LQCD run2 journal from unneeded fileserver events which corrupt the statistics. 
The processed journal file is an input for gen_run2_*.py scripts.
"""
    print 'USAGE: python strip_run2_journal.py RUN2 OUT\n\tRUN2 - run 2 journal file\n\tOUT - output file'
    sys.exit(1)

in_fn, out_fn = sys.argv[1], sys.argv[2]
data = open(in_fn, 'r').readlines()[236367:]
out = open(out_fn, 'a')
out.writelines(data)
