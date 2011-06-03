""" Example application demonstrating pilot store
    (implementation currently based on local SAGA adaptors)
"""


import os
import pdb
import saga
import sys
sys.path.append("../../src/store/")
from pilotstore import *
import time

def has_finished(state):
        state = state.lower()
        if state=="done" or state=="failed" or state=="canceled":
            return True
        else:
            return False

SCENARIO_DIR="/work/luckow/scenarios/"

if __name__ == "__main__":

    for i in os.listdir(SCENARIO_DIR):
        print "Scenario " + str(i)
        starttime=time.time()
        pd = pilot_data()
        subtime1 = time.time()
        print "PD creation time: " + str(subtime1-starttime)
        ##########################################################################################
        base_dir=SCENARIO_DIR + "/"+ str(i)
        base_url="gsiftp://oliver1.loni.org/" + base_dir 
        ps = pd.create_pilot_store("affinity1", base_url)
        subtime2 = time.time()
        print "PS creation time: " + str(subtime2-subtime1)

        data_files=os.listdir(base_dir);
        ps.register_files(data_files)

        subtime3 = time.time()

        print "Number files: " +str(i).zfill(5) + " File registration Time: " + str(subtime3-subtime2) + " sec"
    
        del pd #deletes file from resource
    
