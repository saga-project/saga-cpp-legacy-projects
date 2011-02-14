#!/usr/bin/env python

import sys
import saga

# input comes from STDIN (standard input)
file1=sys.argv[1]
print " input file to mapper " + file1 
f=open(file1,"r")

for line in f:
    # remove leading and trailing whitespace
    line = line.strip()
    # split the line into words
    words = line.split()
    # increase counters
    for word in words:
        # write the results to STDOUT (standard output);
        # what we output here will be the input for the
        # Reduce step, i.e. the input for reducer.py
        #
        # tab-delimited; the trivial word count is 1
        print '%s\t%s' % (word, 1)

