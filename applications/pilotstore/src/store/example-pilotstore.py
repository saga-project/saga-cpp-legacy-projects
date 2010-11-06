""" Example application demonstrating pilot store
    (implementation currently based on local SAGA adaptors)
"""

from pilotstore import *
import os
import pdb
import saga
import sys
import time

if __name__ == "__main__":

    ps = pilot_store()
    
    ##########################################################################################
    # Variant 1
    base_dir = saga.url("file://localhost" + os.getcwd()+"/test/data1/")
    pd1 = pilot_data("affinity1", base_dir)
    # files can be added either relative to base_dir
    pd1.register_file(saga.url("test1.txt")) 
    # files can be added as absolute URL
    pd1.register_file(saga.url("file://localhost" + os.getcwd()+"/test/data1/test2.txt"))
    ps.add_pilot_data(pd1)
    
    ##########################################################################################
    # Variant 2
    pd2 = ps.create_pilot_data("affinity2", base_dir)
    pd2.register_file(saga.url("file://localhost" + os.getcwd()+"/test/data1/test3.txt"))
    
    # move store to other resource
    #pd3 = pd1.copy("affinity3", "file://localhost" + os.getcwd()+"/test/data2/")
    
    pd1.move("file://localhost" + os.getcwd()+"/test/data2/")
    pd1.move("file://localhost" + os.getcwd()+"/test/data1/")
    
    
    print "Pilot Store contains the following pilot data containers: "
    for pd_element in ps:
        print str(pd_element) + "@" + pd_element.get_resource()
    
    del ps #deletes file from resource
    