import saga
import os
import time
import sys
import pdb
import datetime

from bigjob.bigjob_manager import bigjob, subjob

# configurationg
COORDINATION_URL = ["advert://localhost", "redis://localhost/"]
NUMBER_JOBS=[8,16]
NUMBER_NODES=[1,2]
NUMBER_CORES_PER_NODE=1
RESULT_DIR="results"
RESULT_FILE_PREFIX="results/results-"
LRMS_URL="fork://localhost"


def has_finished(state):
        state = state.lower()
        if state=="done" or state=="failed" or state=="canceled":
            return True
        else:
            return False


def load_test(coordination_url, number_jobs, number_nodes, number_cores_per_node):
    
    print "\n**************************************************************************************************************************************************\n"
    print ("Start test scenario - #nodes:%d, #cores/node:%d, #jobs: %d, coordination-url:%s, lrms-url:%s"%
          (number_nodes, number_cores_per_node, number_jobs, coordination_url, LRMS_URL))
    print "\n**************************************************************************************************************************************************\n"      
    
    starttime=time.time()
    ##########################################################################################
    # Start BigJob
    # Parameter for BigJob
    lrms_url = LRMS_URL
    workingdirectory=os.getcwd() +"/agent"  # working directory for agent
   
    # start pilot job (bigjob_agent)
    print "Start Pilot Job/BigJob at: " + lrms_url
    bj = bigjob(coordination_url)
    bj.start_pilot_job(lrms_url=lrms_url,
                       number_nodes=number_nodes,
                       processes_per_node=number_cores_per_node,
                       working_directory=workingdirectory
                      )
        
    queueing_time = None    
    state = str(bj.get_state())
    if state=="Running" and queueing_time==None:
            queueing_time=time.time()-starttime
    print "Pilot Job/BigJob URL: " + bj.pilot_url + " State: " + state

    ##########################################################################################
    # Submit SubJob through BigJob
    jobs = []
    job_start_times = {}
    job_states = {}
    for i in range(0, number_jobs):
        jd = saga.job.description()
        jd.executable = "/bin/date"
        jd.number_of_processes = "1"
        jd.spmd_variation = "single"
        jd.arguments = [""]
        jd.working_directory = os.getcwd() 
        jd.output = "sj-stdout-"+str(i)+".txt"
        jd.error = "sj-stderr-"+str(i)+".txt"

        sj = subjob()
        sj.submit_job(bj.pilot_url, jd)
        jobs.append(sj)
        job_start_times[sj]=time.time()
        job_states[sj] = sj.get_state()
        if state=="Running" and queueing_time==None:
            queueing_time=time.time()-starttime

    # busy wait for completion
    while 1:        
        pilot_state = str(bj.get_state())
        if str(bj.get_state())=="Running" and queueing_time==None:
            queueing_time=time.time()-starttime
        finish_counter=0
        result_map = {}
        for i in range(0, number_jobs):
            old_state = job_states[jobs[i]]
            state = jobs[i].get_state()
            if result_map.has_key(state)==False:
                result_map[state]=1
            else:
                result_map[state] = result_map[state]+1
            #pdb.set_trace()
            if old_state != state:
                print "Job " + str(jobs[i]) + " changed from: " + old_state + " to " + state
            if old_state != state and has_finished(state)==True:
                print "Job: " + str(jobs[i]) + " Runtime: " + str(time.time()-job_start_times[jobs[i]]) + " s."
            if has_finished(state)==True:
                finish_counter = finish_counter + 1                
            job_states[jobs[i]]=state

        print "Pilot State: %s; %d/%d jobs finished"%(pilot_state,finish_counter,number_jobs)
        if finish_counter == number_jobs:
            break
        time.sleep(2)

    runtime = time.time()-starttime
    #print "Runtime: " + str(runtime) + " s; Runtime per Job: " + str(runtime/NUMBER_JOBS)
    ##########################################################################################
    # Cleanup - stop BigJob
    
    result = ("%d,%d,%d,%s,%s,%s,%s"% 
             (number_nodes, number_cores_per_node, number_jobs, str(runtime), str(queueing_time),coordination_url, LRMS_URL))
    
    result_tab = ("%d\t%d\t%d\t%s\t%s\t%s\t%s"% 
             (number_nodes, number_cores_per_node, number_jobs, str(runtime), str(queueing_time), coordination_url, LRMS_URL))
    print ("#Nodes\t#cores/node\t#jobs\tRuntime\tQueuing Time\tCoordination URL\tLRMS URL")
    print result_tab
    
    bj.cancel()
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
    f.write("#Nodes,#cores/node,#jobs,Runtime,Queuing Time,Coordination URL,LRMS URL\n")
    for i in range(0, len(NUMBER_JOBS)):
        result = load_test(COORDINATION_URL[i], NUMBER_JOBS[i], NUMBER_NODES[i], NUMBER_CORES_PER_NODE)
        f.write(result)
        f.write("\n")
    f.close()
