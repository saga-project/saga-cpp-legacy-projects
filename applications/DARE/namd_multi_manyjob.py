#!/usr/bin/env python
import sys
sys.path.append("BigJob")

import getopt
import saga
import time
import pdb
import os
import traceback
import bigjob
import logging
import many_job
import ConfigParser
import optparse
import uuid


def initialize(conf_filename):
    adams_config = ConfigParser.ConfigParser()
    adams_config.read(conf_filename)
    sections = adams_config.sections()
    return adams_config

def has_finished(state):
        state = state.lower()
        if state=="done" or state=="failed" or state=="canceled":
            return True
        else:
            return False

def globus_file_stage(source_url, dest_url):

    print "(DEBUG) Now I am tranferring the files from %s to %s"%(source_url, dest_url)

    try:
        cmd = "globus-url-copy  -cd  %s %s"%(source_url, dest_url)
        os.system(cmd)
    except saga.exception, e:
        error_msg = "File stage in failed : from "+ source_url + " to "+ dest_url
    return None


def sub_jobs_submit(job_type, subjobs_per_resource,number_of_jobs,jd_executable, jd_number_of_processes ):
  
        jobs = []
        job_start_times = {}
        job_states = {}
        jd = saga.job.description()
        affinity = 0

        for i in range(0, int(number_of_jobs)):
            
            ##check if multiple RESOURCEs were used and pick the affinity
            if len(RESOURCES) > 1:
                if (i+1) > (int(subjobs_per_resource)):
                    affinity= 1 
            ##pick the executble 
            if  jd_executable == "namd":
                 jd_executable_use = app_exe[affinity] 
            else:
                 jd_executable_use = jd_executable
         
            # create job description
            jd = saga.job.description()
            print jd_executable_use
            jd.executable = jd_executable_use
            jd.number_of_processes = jd_number_of_processes
            jd.spmd_variation = "mpi"
            
            # choose the job description based on type of job
            
            jd.arguments = [NAMD_CONF]
            
            jd.environment = ["affinity=affinity%s"%(affinity)]
            print "affinity%s"%(affinity)
            jd.working_directory = work_dir[affinity]+ NAMD_DIR
            jd.output =  os.path.join(work_dir[affinity],NAMD_DIR,"stdout_" + job_type + "-"+ str(JOB_UUID)+"-"+ str(i) + ".txt")
            jd.error = os.path.join(work_dir[affinity], NAMD_DIR,"stderr_"+ job_type + "-"+str(JOB_UUID)+ "-"+str(i) + ".txt")
            subjob = mjs[affinity].create_job(jd)
            subjob.run()
            print "Submited sub-job " + "%d"%i + "."
         
            jobs.append(subjob)
            job_start_times[subjob]=time.time()
            job_states[subjob] = subjob.get_state()
            logger.info( job_type + "subjob " + str(i))
            logger.info( "jd.number_of_processes " + str(jd.number_of_processes))
            print "jd.arguments"
            for item in jd.arguments:
                logger.info( "jd.arguments" + item)
                print " ",item
            logger.info("affinity%s"%(affinity))
            logger.info( "jd exec " + jd.executable)
            
        #number_of_jobs = int(end_of_subjobs) - int(start_of_subjobs)
        print "************************ All Jobs submitted ************************" +  str(number_of_jobs)
        while 1:
            finish_counter=0
            result_map = {}
            for i in range(0, int(number_of_jobs)):
                old_state = job_states[jobs[i]]
                state = jobs[i].get_state()
                if result_map.has_key(state) == False:
                    result_map[state]=0
                result_map[state] = result_map[state]+1
                #print "counter: " + str(i) + " job: " + str(jobs[i]) + " state: " + state
                if old_state != state:
                    print "Job " + str(jobs[i]) + " changed from: " + old_state + " to " + state
                if old_state != state and has_finished(state)==True:
                     print "Job: " + str(jobs[i]) + " Runtime: " + str(time.time()-job_start_times[jobs[i]]) + " s."
                if has_finished(state)==True:
                     finish_counter = finish_counter + 1
                job_states[jobs[i]]=state

            print "Current states: " + str(result_map)
            time.sleep(5)
            logger.info("Current states: " + str(result_map))
            if finish_counter == int(number_of_jobs):
                break
                  


if __name__ == "__main__":
    config = {}

    CWD = os.getcwd()

    JOB_UUID = uuid.uuid1()
    APP_NAME = "NAMD"
    
    # parse conf files
    parser = optparse.OptionParser()    
    parser.add_option("-j", "--job-conf", dest="job_conf", help="job configuration file")
    (options, args) = parser.parse_args()
    
    RESOURCES = []
    #parse job conf file
    job_conf = options.job_conf
    config = initialize(job_conf)

    JOB_ID = config.get(APP_NAME, 'JOB_ID')
    RESOURCES = config.get(APP_NAME, 'RESOURCES_USED')
    RESOURCES = RESOURCES.replace(' ','').split(',')
    #print RESOURCES
    SOURCE_DIR = config.get(APP_NAME, 'SOURCE_DIR')
    NAMD_DIR = config.get(APP_NAME, 'NAMD_DIR')
    NAMD_CONF = config.get(APP_NAME, 'NAMD_CONF')
    NAMD_JOBS_NUM = config.get(APP_NAME, 'NAMD_JOBS_NUM')
    NAMD_JOBS_SIZE = config.get(APP_NAME, 'NAMD_JOBS_SIZE') 
    
        
    work_dir = []
    gram_url= []
    re_agent= []
    allocation= []
    queue = []
    processors_per_node = []
    RESOURCE_proxy = []
    ft_name= []
    #parse dare_resource conf file
    resource_conf = os.path.join(CWD, "dare_files/resource.conf")
    config = initialize(resource_conf)
    
    for RESOURCE in RESOURCES:
        print RESOURCE
        RESOURCE_proxy.append(config.get(RESOURCE, 'RESOURCE_proxy'))
        gram_url.append(config.get(RESOURCE, 'gram_url')) 
        re_agent.append(config.get(RESOURCE, 're_agent'))
        allocation.append(config.get(RESOURCE, 'allocation'))
        queue.append( config.get(RESOURCE, 'queue'))
        processors_per_node.append(config.get(RESOURCE, 'processors_per_node'))
        ft_name.append(config.get(RESOURCE, 'ft_name'))
        
    #conf file application specific config file
    
    app_exe = []
    jd_executable_app = []

    
    app_conf = os.path.join(CWD, "dare_files/namd_resource.conf")
    config = initialize(app_conf)

    for RESOURCE in RESOURCES:
        print RESOURCE
        
        work_dir.append(config.get(RESOURCE, 'work_dir'))
        app_exe.append(config.get(RESOURCE, 'app_exe'))
    
    
    LOG_FILENAME = os.path.join(CWD, 'dare_files', 'logfiles/', '%s_%s_log_%s.txt'%(JOB_ID, JOB_UUID, APP_NAME))

    logger = logging.getLogger('dare_%s_manyjob'%(APP_NAME))
    hdlr = logging.FileHandler(LOG_FILENAME)
    formatter = logging.Formatter('%(asctime)s %(levelname)s %(message)s')
    hdlr.setFormatter(formatter)
    logger.addHandler(hdlr)
    logger.setLevel(logging.INFO)


    logger.info("Job id  is "  + str(JOB_ID) )
    logger.info("Machine used is " + RESOURCES[0] )
    
    
    try:  
        
        # submit via mj abstraction        
        
        ## start the big job agents
        resource_list = []
        mjs = []
        for i in range(0,len(RESOURCES) ):
            
            resource_list.append([])
            resource_list[i].append({"gram_url" : gram_url[i], "walltime": "30" ,
                                   "number_cores" : str(int(NAMD_JOBS_NUM)*int(NAMD_JOBS_SIZE)), "processes_per_node":processors_per_node[i],"allocation" : allocation[i],
                                   "queue" : queue[i], "re_agent": re_agent[i], "userproxy":RESOURCE_proxy[i], "working_directory": work_dir[i]+"/output/"})
            logger.info("gram_url" + gram_url[i])
            logger.info("affinity%s"%(i))            
            print "Create manyjob service "
            mjs.append(many_job.many_job_service(resource_list[i], None))
       
        ### transfer the needed files
        if not (SOURCE_DIR== "NONE"):       
            for i in range(0,len(RESOURCES) ):
                globus_file_stage("file://%s/%s/"%(SOURCE_DIR,NAMD_DIR), ft_name[i]+work_dir[i]+ "/"+NAMD_DIR +"/" ) 
                       
        namd_starttime = time.time()
        ### run the namd step
        #sub_jobs_submit("new", "4", "/bin/date", "2") ##dummy job for testing
        
        sub_jobs_submit(APP_NAME, str(int(NAMD_JOBS_NUM)/2) , str(NAMD_JOBS_NUM),"namd", NAMD_JOBS_SIZE) 
        
        namd_runtime = time.time()-namd_starttime
        logger.info( APP_NAME + "Runtime: " + str(namd_runtime) )
        
        ###transfer ouput files from resource to local machine
        for i in range(0,len(RESOURCES) ):
                globus_file_stage(ft_name[i]+work_dir[i]+ "/"+NAMD_DIR +"/", "file://%s/%s_output/"%(SOURCE_DIR,NAMD_DIR) ) 
        
        
        
        
        for i in range(0,len(RESOURCES) ):
            mjs[i].cancel()
    except:
        traceback.print_exc(file=sys.stdout)
        try:
            for i in range(0,len(RESOURCES) ):
                mjs[i].cancel()            
        except:
            pass
