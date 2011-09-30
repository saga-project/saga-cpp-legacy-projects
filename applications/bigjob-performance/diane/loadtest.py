import saga
import os
import time
import sys
import pdb
import datetime

from bigjob.bigjob_manager import bigjob, subjob
import subprocess

NUMBER_RUNS=10

#NUMBER_JOBS=[256]
# Scenario WUs
#NUMBER_JOBS=[64,128,256,512,1024,2048]
#NUMBER_NODES=[8,8,8,8,8,8]
#NUMBER_JOBS=[64]
# Scenario cores
#NUMBER_JOBS=[32,64,128,256,512]
#NUMBER_NODES=[1,2,4,8,16]
COORDINATION_URL=["diane://s1"]
NUMBER_JOBS=[32,64,128,256,512,64,128,256,512,1024,2048]
NUMBER_NODES=[1,2,4,8,16,8,8,8,8,8,8]
#NUMBER_JOBS=[1024,4096]
#NUMBER_NODES=[32,8]
#NUMBER_NODES=[16]
NUMBER_CORES_PER_NODE=8
RESULT_DIR="results"
RESULT_FILE_PREFIX="results/results-"
LRMS_URL="pbspro://localhost"


def has_finished(state):
        state = state.lower()
        if state=="done" or state=="failed" or state=="canceled":
            return True
        else:
            return False

def load_test(coordination_url, number_jobs, number_nodes, number_cores_per_node):
    
    print "\n**************************************************************************************************************************************************\n"
    print ("Start DIANE test scenario - #nodes:%d, #cores/node:%d, #jobs: %d, coordination-url:%s, lrms-url:%s"%
          (number_nodes, number_cores_per_node, number_jobs, coordination_url, LRMS_URL))
    print "\n**************************************************************************************************************************************************\n"      
    
    starttime=time.time()
    ##########################################################################################
    # Start DIANE
    runfile="run-diane.py"
    rf = open(runfile, "w")
    rf.write("""
from diane_test_applications import crash as application

def run(input,config):
    config.MSGMonitoring.MSG_MONITORING_ENABLED = False
    input.data.commands = ['time.clock()']*%d
"""%int(number_jobs))
    rf.close()

    p = subprocess.Popen(args=["diane-run " + runfile], stdout=sys.stdout, stderr=sys.stderr, shell=True)

    agent_command = "diane-submitter SAGA --jobservice-url=pbspro://localhost --filesystem-url=file:///N/u/luckow/diane-worker/ --diane-worker-number=%d"%(number_nodes)
    os.system(agent_command)
    print "Wait for tasks to complete"
    p.wait()    
    runtime = time.time()-starttime

    ##########################################################################################
    # Cleanup 
    
    result = ("%d,%d,%d,%s,%s,%s,%s"% 
             (number_nodes, number_cores_per_node, number_jobs, str(runtime), "0",coordination_url, LRMS_URL))
    
    result_tab = ("%d\t%d\t%d\t%s\t%s\t%s\t%s"% 
             (number_nodes, number_cores_per_node, number_jobs, str(runtime), "0", coordination_url, LRMS_URL))
    print ("#Nodes\t#cores/node\t#jobs\tRuntime\tQueuing Time\tCoordination URL\tLRMS URL")
    print result_tab
    
    # hack: delete manually pbs jobs of user
    os.system("qstat -u `whoami` | grep -o ^[0-9]* |xargs qdel")
    #os.system("saga-advert remove_directory advert://advert.cct.lsu.edu:8080/bigjob")
    return result
          
    

""" Test Job Submission via BigJob """
if __name__ == "__main__":
    try:
        os.mkdir(RESULT_DIR)
    except:
        pass
    d =datetime.datetime.now()
    result_filename = RESULT_FILE_PREFIX + d.strftime("%Y%m%d-%H%M%S") + ".csv"
    f = open(result_filename, "w")
    f.write("#Nodes,#cores/node,#jobs,Runtime,Queuing Time,Coordination URL,LRMS URL,SubJob Submission Time\n")
    for k in range(0, NUMBER_RUNS):
        print ("\n**************\n RUN %d \n*****************\n"%k)
        for j in range(0, len(COORDINATION_URL)):
            c = COORDINATION_URL[j]
            print ("\n**************\n COORDINATION: %s \n*****************\n"%c)
            for i in range(0, len(NUMBER_JOBS)):
                result = load_test(c, NUMBER_JOBS[i], NUMBER_NODES[i], NUMBER_CORES_PER_NODE)
                f.write(result)
                f.write("\n")
                f.flush()
    f.close()
