#!/usr/bin/env python

import os
import sys
import string
import math
import traceback
from string import * 
import re

def filterdups(iterable):
    result = []
    for item in iterable:
        if item not in result:
            result.append(item)
    return result

def list_difference(list1, list2):
    diff_list = []
    for item in list1:
        if not item in list2:
            diff_list.append(item)
    return diff_list

# begin the checking module
def FileTest(TEST_FILE_DIR,  TEST_DIR_SUFFIX,  TEST_FILE_EXTENSION,  NUMBER_OF_JOBS_PER_STAGE):
    # The list of all the simulations we have files for
    ALLSIMS_LIST=[]
    # The list of simulations that need to be re-run:
    RERUN_LIST=[]

    # The find command test
    FIND_CMD = 'find ' + TEST_FILE_DIR +"/*" + '  -name *.' + TEST_FILE_EXTENSION+' -print'
     
    # Check that we can search for  the file using python
    try:
        cmd_out = os.popen(FIND_CMD).readlines()
        # Get the list of all the Simulations
        for file_out in cmd_out:
            ftemp = re.search(TEST_DIR_SUFFIX + "([0-9]*)", file_out).group()
            ALLSIMS_LIST.append(atoi( re.sub(TEST_DIR_SUFFIX,'',ftemp)))
        ALLSIMS_LIST=sorted(ALLSIMS_LIST)
        ALLSIMS_LIST=filterdups(ALLSIMS_LIST)
        
        # Check that the files are not zeroes
        for file_out in cmd_out:
            fsize = os.path.getsize(file_out.rstrip("\n"))
            if(fsize==0):
                ftemp = re.search(TEST_DIR_SUFFIX + "([0-9]*)", file_out).group()
                RERUN_LIST.append(atoi( re.sub(TEST_DIR_SUFFIX,'',ftemp) ) )

        # Check that the number of simulations is the number of jobs
        if(len(ALLSIMS_LIST) !=NUMBER_OF_JOBS_PER_STAGE):
            original = range(0, NUMBER_OF_JOBS_PER_STAGE)
            difference = list_difference(original, ALLSIMS_LIST)
            print original
            print difference
            RERUN_LIST=RERUN_LIST+(difference)
        return RERUN_LIST
    except:
        traceback.print_exc(file=sys.stdout)
    
