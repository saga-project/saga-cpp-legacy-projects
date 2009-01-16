#!/usr/bin/env python

import os
import sys
import string
import math
import traceback
from string import * 
from re import *

# begin the checking module
def Calibrate():
    # This script performs a calibrated test of the commands we will use in checking
    # the output from each stage
    
    # directory where the calibration file we want to check lives:
    CALIBRATE_FILE_DIR = "/work/yye00/ICAC/Calibrate/"
    # the calibration filename
    CALIBRATE_FILENAME = "Calibrate"
    # the calibration filename extension, important for hdf5 versus F5
    CALIBRATE_EXTENSION = ".h5"
    # The find command test
    FIND_CMD = 'find ' + CALIBRATE_FILE_DIR + '. -name "*.h5" -print'
    
    # Flags that specify the error
    EXIST_FLAG = 0
    FIND_FLAG = 0
    SIZE_FLAG = 0
    SANITY_FLAG = 0
    
    # get the full filename
    filename = CALIBRATE_FILE_DIR + CALIBRATE_FILENAME + CALIBRATE_EXTENSION
    # Check that the file exists using os module
    try:        
        if(os.path.exists(filename)):
            EXIST_FLAG = 1
    except:
        traceback.print_exc(file=sys.stdout)
     
    # Check that we can search for  the file using python
    try:
        cmd_out = os.popen(FIND_CMD).readlines()
        file_out = cmd_out[0]
        file_out=file_out.replace('/./','/')
        file_out=file_out.rstrip("\n")
        if(file_out==filename):
            FIND_FLAG = 1
    except:
        traceback.print_exc(file=sys.stdout)
    
    # Check that the file is not zero sized
    try:
        fsize= os.path.getsize(filename)
        if(fsize>0):
            SIZE_FLAG = 1
    except:
        traceback.print_exc(file=sys.stdout)
        
    # Check that we passed all three tests
    try:
        if(EXIST_FLAG*FIND_FLAG*SIZE_FLAG==1):
            SANITY_FLAG = 1
            return SANITY_FLAG 
        else:
            return -1 
    except:
        traceback.print_exc(file=sys.stdout)            

