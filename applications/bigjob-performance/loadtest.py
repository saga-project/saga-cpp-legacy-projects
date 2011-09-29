import saga
import os
import time
import sys
import pdb
import datetime

from bigjob.bigjob_manager import bigjob, subjob

# configurationg
#COORDINATION_URL = "advert://advert.cct.lsu.edu:8080"
#COORDINATION_URL = "redis://localhost"
#COORDINATION_URL = "redis://i136"
#COORDINATION_URL = "redis://cyder.cct.lsu.edu:8080"
#COORDINATION_URL = "tcp://localhost"

NUMBER_RUNS=10
#COORDINATION_URL = ["tcp://login1", "redis://cyder.cct.lsu.edu:8080", "redis://login1","advert://advert.cct.lsu.edu:8080"]
COORDINATION_URL = ["advert://advert.cct.lsu.edu:8080"]

#NUMBER_JOBS=[256]
# Scenario WUs
#NUMBER_JOBS=[64,128,256,512,1024,2048]
#NUMBER_NODES=[8,8,8,8,8,8]
#NUMBER_JOBS=[64]
# Scenario cores
#NUMBER_JOBS=[32,64,128,256,512,64,128,256,512,1024,2048]
NUMBER_JOBS=[1024,4096]
NUMBER_NODES=[32,8]
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
    print ("Start test scenario - #nodes:%d, #cores/node:%d, #jobs: %d, coordination-url:%s, lrms-url:%s"%
          (number_nodes, number_cores_per_node, number_jobs, coordination_url, LRMS_URL))
    print "\n**************************************************************************************************************************************************\n"      
    
    starttime=time.time()
    ##########################################################################################
    # Start BigJob
    # Parameter for BigJob
    lrms_url = LRMS_URL
    workingdirectory="/N/u/luckow/src/bigjob-performance/agent"  # working directory for agent
   
    # start pilot job (bigjob_agent)
    print "Start Pilot Job/BigJob at: " + lrms_url
    bj = bigjob(coordination_url)
    bj.start_pilot_job(lrms_url=lrms_url,
                       number_nodes=number_nodes,
                       processes_per_node=number_cores_per_node,
                       working_directory=workingdirectory
                      )
        
    queueing_time = None    
    subjob_submission_time = None    
    pilot_state = str(bj.get_state_detail())
    if pilot_state=="Running" and queueing_time==None:
            queueing_time=time.time()-starttime
            print "*** Pilot State: " + pilot_state + " queue time: " + str(queueing_time)
    print "Pilot Job/BigJob URL: " + bj.pilot_url + " State: " + pilot_state

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

        if pilot_state != "Running":
            pilot_state = str(bj.get_state_detail())
            if pilot_state=="Running" and queueing_time==None:
                queueing_time=time.time()-starttime
                print "*** Pilot State: " + pilot_state + " queue time: " + str(queueing_time)

    subjob_submission_time = time.time()-starttime
    # busy wait for completion
    while 1:        
        pilot_state = str(bj.get_state_detail())
        if pilot_state=="Running" and queueing_time==None:
            queueing_time=time.time()-starttime
            print "*** Pilot State: " + pilot_state + " queue time: " + str(queueing_time)
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
        if finish_counter >= number_jobs-1 or pilot_state == "Failed":
            break
        time.sleep(2)

    runtime = time.time()-starttime
    #print "Runtime: " + str(runtime) + " s; Runtime per Job: " + str(runtime/NUMBER_JOBS)
    ##########################################################################################
    # Cleanup - stop BigJob
    
    result = ("%d,%d,%d,%s,%s,%s,%s,%s"% 
             (number_nodes, number_cores_per_node, number_jobs, str(runtime), str(queueing_time),coordination_url, LRMS_URL,str(subjob_submission_time)))
    
    result_tab = ("%d\t%d\t%d\t%s\t%s\t%s\t%s"% 
             (number_nodes, number_cores_per_node, number_jobs, str(runtime), str(queueing_time), coordination_url, LRMS_URL))
    print ("#Nodes\t#cores/node\t#jobs\tRuntime\tQueuing Time\tCoordination URL\tLRMS URL")
    print result_tab
    
    bj.cancel()
    # hack: delete manually pbs jobs of user
    os.system("qstat -u `whoami` | grep -o ^[0-9]* |xargs qdel")
    os.system("saga-advert remove_directory advert://advert.cct.lsu.edu:8080/bigjob")
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
