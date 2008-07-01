#!/usr/bin/env python
"""
Simple script for testing Replica Exchange Application Manager writing (time_step, random number, replica_id)

This script is run by Application Manager

"""

import sys, os
import time, random

if len(sys.argv) <= 2:
    iflag_fileout = False
    replica_id = 1
    NSTEP = eval(sys.argv[1])
else:
    iflag_fileout = True
    replica_id = sys.argv[1]
    replica_ofile_name = sys.argv[2]
    NSTEP = eval(sys.argv[3])
    
istep = 0; random.seed(time.time()/eval(replica_id))
while 1:
    time.sleep(0.5)
    rd = random.random()
        
    istep = istep + 1
    addline = "%d    %f   %s "%(istep, rd, replica_id)
        
    if iflag_fileout:
        ofile = open(replica_ofile_name,'a')
        ofile.write(addline+"\n")
        ofile.close()
    else:
        print addline
            
    if istep == NSTEP:
        break
            
        
    