###
### Example script/Application used to run mapreduce program
###

import sys
import time
import saga
import os
from optparse import OptionParser
import pdb

# Import mapreduce library which is in current directory.
sys.path.append(os.path.dirname(__file__))
from  MRwoBJ import *


def main():
    
    parser = OptionParser()
    parser.add_option("-i", "--input", dest="input",
                  help="SAGA url of the input data location")
    parser.add_option("-o", "--output", dest="output_dir", 
                  help="SAGA url of the output data location")
    parser.add_option("-t", "--temp", dest="temp_dir", 
                  help="SAGA url of the temporary data location")
    parser.add_option("-b", "--reduces", dest="nbr_reduces", 
                  help="number of reduces")
    parser.add_option("-m", "--mapper", dest="mapper", 
                  help="physical complete path of mapper")
    parser.add_option("-r", "--reducer", dest="reducer", 
                  help="physical complete path of reducer")
    parser.add_option("-u", "--resource", dest="resource_url", 
                  help="SAGA url of HPC resource. Could you gram/pbspro.")
    parser.add_option("-n", "--nodes", dest="number_nodes", 
                  help="Number of nodes requested for task execution")
    parser.add_option("-w", "--workingdirectory", dest="workingdirectory", 
                  help="Workingdirectory in which agent is executed and its log & error files are stored")
    parser.add_option("-c", "--chunk", dest="chunk", 
                  help="chunk size")
    parser.add_option("-k", "--workers", dest="workers",default=1, 
                  help="number of processes for each map/reduce function")

    (options, args) = parser.parse_args()
    print sys.argv
    
    print " **************************  Map Reduce Framework Started ************************* "
    initial_time = time.time()
    fsp = MapReduce(options.input,options.output_dir,options.temp_dir,options.nbr_reduces,options.mapper,options.reducer,options.chunk,options.resource_url,options.workers, options.workingdirectory)

    print " \n >>> All Configuration parameters provided \n"
    starttime = time.time()
    
    fsp.chunk_input()
    runtime = time.time()-starttime
    print " \n Time taken to chunk : " + str(round(runtime,3)) +  "\n\n";
    starttime = time.time()
    fsp.map_job_submit()
    runtime = time.time()-starttime
    print " >>> Map Phase completed and it took ... " + str(round(runtime,3)) + "\n\n"
    starttime = time.time()
    fsp.reduce_job_submit()
    runtime = time.time()-starttime
    print " >>> Reduce Phase completed and it took ... " + str(round(runtime,3)) + "\n\n"  
    final_time = time.time() - initial_time
    print " >>> The total time taken is " + str(round(final_time,3)) + "\n\n"
    sys.exit(0)
    
if __name__=="__main__":
    main()
