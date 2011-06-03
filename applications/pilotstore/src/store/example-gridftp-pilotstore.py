""" Example application demonstrating pilot store
    (implementation currently based on local SAGA adaptors)
"""

from pilotstore import *
import os
import pdb
import saga
import sys
import time

def has_finished(state):
        state = state.lower()
        if state=="done" or state=="failed" or state=="canceled":
            return True
        else:
            return False

if __name__ == "__main__":

    starttime=time.time()
    pd = pilot_data()
    subtime1 = time.time()
    print "PD creation time: " + str(subtime1-starttime)
    ##########################################################################################
    base_dir="/work/luckow/data-small/"
    base_url="gsiftp://oliver1.loni.org/" + base_dir 
    ps = pd.create_pilot_store("affinity1", base_url)
    subtime2 = time.time()
    print "PS creation time: " + str(subtime2-subtime1)

    counter=0
    #pdb.set_trace()
    data_files=os.listdir(base_dir);
    ps.register_files(data_files)
    #for f in os.listdir(base_dir):
    #    file_url = base_url + f
    #    print "register: " + str(file_url)
    #    ps.register_file(saga.url(file_url))
    #    counter = counter + 1

    subtime3 = time.time()

    print "File registration creation time: " + str(subtime3-subtime2) + " # files: " + str(counter)
    #ps.register_file(saga.url("gsiftp://qb1.loni.org//work/luckow/test-data/testfile_10MB"))
    #ps.register_file(saga.url("gsiftp://qb1.loni.org//work/luckow/test-data/testfile_100MB"))
    #ps.register_file(saga.url("gsiftp://qb1.loni.org//work/luckow/test-data/testfile_1000MB"))

    
    # move store to other resource
    #ps1 = ps.copy("affinity2", "gsiftp://qb1.loni.org//work/luckow/data-small/")
    
    #ps.move("gsiftp://eric1.loni.org//work/luckow/data-small/")
    
    print "Pilot Store contains the following pilot data containers: "
    for ps_element in pd:
        print str(ps_element) + "@" + ps_element.get_resource()
        
        
    
    del pd #deletes file from resource
    
