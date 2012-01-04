import saga
import os
import time
import sys
import pdb
import datetime

from bigjob import bigjob, subjob

# configurationg
#COORDINATION_URL = "advert://advert.cct.lsu.edu:8080"
COORDINATION_URL = "redis://localhost"
#COORDINATION_URL = "redis://i136"
#COORDINATION_URL = "redis://cyder.cct.lsu.edu:8080"
#COORDINATION_URL = "redis://cyder.cct.lsu.edu:8080"
#COORDINATION_URL = "tcp://localhost"


BFAST_JOBS = {"local":
                { "lrms_url":"fork://localhost/",
                  "bfast_exe":"bfast",
                  "bfast_ref_genome": "/Volumes/Media-2/DATA_BFAST/hg18chr21_40/alt_HuRef_chr21.fa",
                  "bfast_reads": "/Volumes/Media-2/DATA_BFAST/hg18chr21_40/reads.1.fastq",              
                  "bfast_tmp_dir" : "/Volumes/Media-2/DATA_BFAST/hg18chr21_40/", 
                  "bfast_matches_dir":"matches/",    
                  "working_directory" : os.path.join(os.getcwd(), "work"),     
                  "number_subjobs" : 1,
                  "number_nodes" : 1,  
                  "number_cores_per_node": 8         
                 }
              }

NUMBER_REPEATS=1

RESULT_DIR="results"
RESULT_FILE_PREFIX="results/results-"


def has_finished(state):
        state = state.lower() 
        if state=="done" or state=="failed" or state=="canceled":
            return True
        else:
            return False


def get_bj_states(jobs_dictionary, starttime):
    jobs = jobs_dictionary
    
    if (len(jobs.keys())>0):
        all_running = True
    else:
        all_running = False
        
    for i in jobs.keys():
        bj = jobs[i]["bigjob"]
        queueing_time = jobs[i]["queueing_time"] 
        pilot_state = str(bj.get_state_detail())
        if pilot_state=="Running" and queueing_time==None:
                queueing_time=time.time()-starttime
                print "Pilot: " + str(bj) + " Pilot State: " + pilot_state + " queue time: " + str(queueing_time)
                jobs[i]["queueing_time"] = queueing_time
        elif pilot_state != "Running":
            all_running == False
        print "Pilot: " + str(bj) + " Pilot Job/BigJob URL: " + bj.pilot_url + " State: " + pilot_state
    return all_running


def load_test(bfast_jobs, run_id=0):
    starttime=time.time()
    workingdirectory=os.getcwd() +"/agent"  # working directory for agent
    print "\n**************************************************************************************************************************************************\n"
    print "START TEST %d Specification: %s"%(run_id, str(bfast_jobs))
    print "\n**************************************************************************************************************************************************\n"      
          
    for i in bfast_jobs.keys():
        resource = bfast_jobs[i]
        number_nodes = resource["number_nodes"]
        number_cores_per_node= resource["number_cores_per_node"]
        number_jobs = resource["number_subjobs"]
        lrms_url = resource["lrms_url"]
        workingdirectory = resource["working_directory"]
        print ("BJ: %s- #nodes:%d, #cores/node:%d, #jobs: %d, coordination-url:%s, lrms-url:%s"%
          (lrms_url, number_nodes, number_cores_per_node, number_jobs, COORDINATION_URL, lrms_url))
        
            # start pilot job (bigjob_agent)
        print "Start Pilot Job/BigJob at: " + lrms_url
        bj = bigjob(COORDINATION_URL)
        bj.start_pilot_job(lrms_url=lrms_url,
                           number_nodes=number_nodes,
                           processes_per_node=number_cores_per_node,
                           working_directory=workingdirectory
                          )
        resource["bigjob"]=bj
        resource["queueing_time"]=None
        resource["subjobs"]=[]
            
    subjob_submission_time = None    
    
    all_running = False
    while all_running == False:
        all_running = get_bj_states(bfast_jobs, starttime)
        if all_running == False: time.sleep(1)
    

    ##########################################################################################
    # Submit SubJob through BigJob
    job_start_times = {}
    job_states = {}
    jobs = []
    total_number_of_jobs=0
    for i in bfast_jobs.keys():
        resource = bfast_jobs[i]
        bj = resource["bigjob"]
        subjobs = resource["subjobs"]
        
        jd = saga.job.description()
        #jd.executable = "/bin/date"
        jd.executable = resource["bfast_exe"]
        jd.number_of_processes = "1"
        jd.spmd_variation = "single"
        jd.arguments = [""]
        jd.arguments = ["match",  
                        "-f",  resource["bfast_ref_genome"] , 
                        "-A 1",  
                        "-r",  resource["bfast_reads"],
                        "-n" ,"1" ,
                        "-T" , resource["bfast_tmp_dir"]
                        ]  
         
        jd.output = "bfast-stdout.txt"
        jd.error = "bfast-stderr.txt"

        sj = subjob()
        sj.submit_job(bj.pilot_url, jd)
        
        total_number_of_jobs = total_number_of_jobs + 1
        
        subjobs.append(sj)
        jobs.append(sj)
        job_start_times[sj]=time.time()
        job_states[sj] = sj.get_state()
        
        all_running = get_bj_states(bfast_jobs, starttime)

    subjob_submission_time = time.time()-starttime
    
    # busy wait for completion
    while 1:        
        all_running = get_bj_states(bfast_jobs, starttime)
        for i in bfast_jobs.keys():
            resource = bfast_jobs[i]
            bj = resource["bigjob"]
            subjobs = resource["subjobs"]
            number_subjobs_in_bigjob = resource["number_subjobs"]
            finish_counter=0
            result_map = {}      
            for i in subjobs: # iterate through sub-jobs
                old_state = job_states[i]
                state = i.get_state()
                if result_map.has_key(state)==False:
                    result_map[state] = 1
                else:
                    result_map[state] = result_map[state] + 1                
                if old_state != state:
                    print "Job " + str(i) + " changed from: " + old_state + " to " + state
                if old_state != state and has_finished(state)==True:
                    job_runtime = time.time()-job_start_times[i]
                    print "Job: " + str(i) + " Runtime: " + str(job_runtime) + " s."
                if has_finished(state)==True:
                    finish_counter = finish_counter + 1               
                    
                job_states[i]=state
            
            if result_map.has_key("Done") and number_subjobs_in_bigjob == result_map["Done"]:
                bj_completion_time = time.time() - starttime
                resource["completion_time"] = bj_completion_time
                print "BJ: " + str(bj) + " Result: " + str(result_map) + " Time: " + str(bj_completion_time)
                
            print "Pilot State: %s; %d/%d jobs finished"%(bj.get_state(), finish_counter, number_subjobs_in_bigjob)
            
            
        # check whether all BJs are finished
        finished_bj = 0
        for i in bfast_jobs.keys():
           resource = bfast_jobs[i]
           if resource.has_key("completion_time"):
               finished_bj = finished_bj + 1 

        if finished_bj == len(bfast_jobs.keys()):
            break

        time.sleep(2)

    runtime = time.time()-starttime

    results = ""   
    for i in bfast_jobs.keys():
        resource = bfast_jobs[i]
        bj = resource["bigjob"]
        number_nodes = resource["number_nodes"]
        number_cores_per_node = resource["number_cores_per_node"]
        number_jobs = resource["number_subjobs"]
        bj_runtime = resource["completion_time"]
        queueing_time = resource["queueing_time"]        
        lrms_url = resource["lrms_url"]
        print ("Run\tBJ\t#Nodes\t#cores/node\t#jobs\tQueuing Time\tBJ Runtime\tTotal Runtime\tCoordination URL\tLRMS URL")
        result_tuple = (run_id, str(bj), number_nodes, number_cores_per_node, number_jobs, 
                str(queueing_time), str(bj_runtime), str(runtime), COORDINATION_URL, lrms_url)
        results = results + ("%d,%s,%d,%d,%d,%s,%s,%s,%s,%s\n"%(result_tuple))        
        # print results
        result_tab = ("%d\t%s\t%d\t%d\t%d\t%s\t%s\t%s,\t%s\t%s"%(result_tuple))
        print result_tab
    
    # Cleanup - stop BigJob
    for i in bfast_jobs.keys():
        resource = bfast_jobs[i]
        bj = resource["bigjob"]
        bj.cancel()
    
    # hack: delete manually pbs jobs of user
    #os.system("qstat -u `whoami` | grep -o ^[0-9]* |xargs qdel")
    return results
          
    

""" Test Job Submission via BigJob """
if __name__ == "__main__":
    try:
        os.mkdir(RESULT_DIR)
    except:
        pass
    d =datetime.datetime.now()
    result_filename = RESULT_FILE_PREFIX + d.strftime("%Y%m%d-%H%M%S") + ".csv"
    f = open(result_filename, "w")
    f.write("Run,BJ,#Nodes,#cores/node,#jobs,Queuing Time,BJ Runtime,Total Runtime,Coordination URL,LRMS URL")
    for i in range(0, NUMBER_REPEATS):
        result = load_test(bfast_jobs=BFAST_JOBS, run_id=i)
        f.write(result)
        f.write("\n")
        f.flush()
    f.close()
