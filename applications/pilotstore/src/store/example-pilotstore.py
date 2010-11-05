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
    pd1 = pilot_data("affinity1") 
    pd1.add_file(saga.url("file://" + os.getcwd()+"/test/store1/test1.txt"))
    pd1.add_file(saga.url("file://" + os.getcwd()+"/test/store1/test2.txt"))
    ps.add_pilot_data(pd1)
    
    # Variant 2
    pd2 = ps.create_pilot_data("affinity2")
    pd2.add_file(saga.url("file://" + os.getcwd()+"/test/store1/test3.txt"))
    
    # move store to other resource
    pd2.copy("file://" + os.getcwd()+"/test/store2/")
    
    
    del ps #deletes file from resource
    