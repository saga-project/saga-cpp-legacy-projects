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

global jobs, job_start_times, job_states
jobs = []
job_start_times = {}
job_states = {}


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

def cloud_file_stage(source_url, dest_url):

    print "(DEBUG) Now I am tranferring the files from %s to %s"%(source_url, dest_url)

    try:
        cmd = "scp  -r -i /home/cctsg/install/euca/smaddi2.private %s %s"%(source_url, dest_url)
        os.system(cmd)
    except saga.exception, e:
        error_msg = "File stage in failed : from "+ source_url + " to "+ dest_url
    return None

def file_stage(source_url, dest_url):

    print "(DEBUG) Now I am tranferring the files from %s to %s"%(source_url, dest_url)
    if dest_url.startswith("root@"):
        try:
            cmd = "scp  -r -i /home/cctsg/install/euca/smaddi2.private %s %s"%(source_url, dest_url)
            print cmd
            os.system(cmd)
        except saga.exception, e:
            error_msg = "File stage in failed : from "+ source_url + " to "+ dest_url
    else:
        try:
            cmd = "globus-url-copy  -cd  %s %s"%("file://"+source_url, dest_url)
            print "globus :" +cmd
            os.system(cmd)
        except saga.exception, e:
            error_msg = "File stage in failed : from "+ source_url + " to "+ dest_url


    return None

def sub_jobs_submit( jd_executable, job_type, affinity ,  subjobs_start,  number_of_jobs, jd_number_of_processes):
                                 
        jd = saga.job.description()
        
        for i in range(subjobs_start, int(number_of_jobs) + int(subjobs_start) ):

                        ##pick the executble 
            if  jd_executable == "namd":
                 jd_executable_use = app_exe[affinity] 
            else:
                 jd_executable_use = jd_executable
         
            # create job description
            jd = saga.job.description()
            print jd_executable_use
            jd.executable = jd_executable_use
            jd.number_of_processes = str(jd_number_of_processes)
            jd.spmd_variation = "mpi"
            
            # choose the job description based on type of job
            
            jd.arguments = [namd_conf]
            
            jd.environment = ["affinity=affinity%s"%(affinity)]
            print "affinity%s"%(affinity)
            jd.working_directory = work_dir[affinity]+ namd_dir
            jd.output =  work_dir[affinity] + os.path.join(namd_dir,"stdout_" + job_type + "-"+ str(JOB_UUID)+"-"+ str(i) + ".txt")
            jd.error = work_dir[affinity] + os.path.join(namd_dir,"stderr_"+ job_type + "-"+str(JOB_UUID)+ "-"+str(i) + ".txt")
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
 
        
def wait_for_jobs(number_of_jobs):               

        print "************************ All Jobs submitted ************************" +  str(number_of_jobs)
        while 1:
            finish_counter=0
            result_map = {}
            for i in range(0, number_of_jobs):
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
            if finish_counter == number_of_jobs:
                break
                  


if __name__ == "__main__":
    config = {}

    #cwd = "/home/cctsg/pylons/DARE-BIOSCOPE/darebioscope/lib/adams/"
    cwd = os.getcwd()
    APP_NAME="NAMD"
    bfast_uuid = uuid.uuid1()
    JOB_UUID = bfast_uuid
    #bfast_uuid = "371064a4-4e5c-11e0-88e1-d8d385abb2b0"
    # parse conf files
    parser = optparse.OptionParser()    
    parser.add_option("-j", "--job-conf", dest="job_conf", help="job configuration file")
    (options, args) = parser.parse_args()
    
    resources_used = []
    global shortreads_name

    #parse job conf file
    job_conf = options.job_conf
    config = initialize(job_conf)
    #print RESOURCES
    
    
    job_id = config.get(APP_NAME, 'job_id')
    machu = config.get(APP_NAME, 'resources_use')
    resources_used = machu.replace(' ','').split(',')    
    machs = config.get(APP_NAME, 'resources_job_count')
    resources_job_count = machs.replace(' ','').split(',')
    source_dir =config.get(APP_NAME, 'source_dir')
    namd_dir = config.get(APP_NAME, 'namd_dir')
    namd_conf= config.get(APP_NAME, 'namd_conf')
    jobs_size = config.get(APP_NAME, 'namd_jobs_size')
    namd_jobs_size = jobs_size.replace(' ','').split(',')
    ##to check whether to run the prepare_read files step?
    resource_list = config.get(APP_NAME, 'resource_list')
    resource_app_list = config.get(APP_NAME, 'resource_app_list')
    walltime = config.get(APP_NAME, 'walltime')

    
    work_dir = []
    gram_url= []
    re_agent= []
    allocation= []
    queue = []
    processors_per_node = []
    resource_proxy = []
    ft_name= []
    #parse dare_resource conf file
    
    config = initialize(resource_list)
    
    for resource in resources_used:
        print resource
        
        work_dir.append(config.get(resource, 'work_dir'))
        if (config.get(resource, 'RESOURCE_proxy') == "NA") :
           resource_proxy.append(None)
        else:
           resource_proxy.append(config.get(resource, 'RESOURCE_proxy'))
        gram_url.append(config.get(resource, 'gram_url')) 
        re_agent.append(config.get(resource, 're_agent'))
        allocation.append(config.get(resource, 'allocation'))
        queue.append( config.get(resource, 'queue'))
        processors_per_node.append(config.get(resource, 'processors_per_node'))
        ft_name.append(config.get(resource, 'ft_name'))
        
    #dare_bfast conf file applicatio specific config file
    
    
    app_exe = []
    work_dir = []
    
    
    config = initialize(resource_app_list)
    
    for resource in resources_used:
        if resource.startswith("fgeuca"):
           resource = "fgeuca"

        print resource
        
        work_dir.append(config.get(resource, 'work_dir'))
        app_exe.append(config.get(resource, 'app_exe'))       
        
    
    LOG_FILENAME = os.path.join(cwd, 'dare_files/logfiles/', '%s_%s_log_bfast.txt'%(job_id, bfast_uuid))

    logger = logging.getLogger('dare_bfast_manyjob')
    hdlr = logging.FileHandler(LOG_FILENAME)
    formatter = logging.Formatter('%(asctime)s %(levelname)s %(message)s')
    hdlr.setFormatter(formatter)
    logger.addHandler(hdlr)
    logger.setLevel(logging.INFO)


    logger.info("Job id  is "  + str(job_id) )
    logger.info("Machine used is " + resources_used[0] )
    
    
    try:  
   
        
        
        # submit via mj abstraction        
        
        ## start the big job agents
        resource_list = []
        mjs = []
        
        for i in range(0,len(resources_used) ):
            
            resource_list.append([])
            cppn= int(processors_per_node[i]) 
            crjc= int(resources_job_count[i])
            cnnc= int(namd_jobs_size[i])
            k=0
            if (cnnc*crjc%cppn !=0):
               k =1
            coress = cppn * (cnnc*crjc/cppn +k ) 
            print namd_jobs_size[i],"vhjghjm", resources_job_count[i]           
            resource_list[i].append({"gram_url" : gram_url[i], "walltime": walltime ,
                                   "number_cores" : str(coress), "processes_per_node":processors_per_node[i], "allocation" : allocation[i],
                                   "queue" : queue[i], "re_agent": re_agent[i], "userproxy": resource_proxy[i], "working_directory": work_dir[i]})

            logger.info("gram_url" + gram_url[i])
            logger.info("affinity%s"%(i))            
            print "Create manyjob service "
            mjs.append(many_job.many_job_service(resource_list[i], None))
        """
        
        ### transfer the needed files
                               
        if not (reads_refgnome == "NONE"):       
            for i in range(0,len(resources_used) ):
                globus_file_stage("file://" + source_shortreads, ft_name[i]+bfast_reads_dir[i])     

        ### transfer the needed files
        p = 1
        if not (source_shortreads == "NONE"):       
            for i in range(0,len(resources_used) ):
                for k in range(p,p+4):
                    cloud_file_stage(source_shortreads+"readss.%s.fastq"%(k), ft_name[i]+bfast_reads_dir[i])     
                p = p +4

        """
        
        ### transfer the needed files
        if not (source_dir== "NONE"):       
            for i in range(0,len(resources_used) ):
                file_stage("/%s/%s/"%(source_dir,namd_dir), ft_name[i]+work_dir[i]+ "/"+namd_dir +"/" ) 
       
        
        namd_starttime = time.time()
        
        total_number_of_jobs=0
        
        #sub_jobs_submit( jd_executable, job_type, affinity = 0,  subjobs_start = 0 ,  number_of_jobs = 0, jd_number_of_processes = 0 ):
        #sub_jobs_submit(0,APP_NAME, 1 , str(NAMD_JOBS_NUM),"namd", NAMD_JOBS_SIZE)

        for i in range (0, len(resources_used)):
            
            sub_jobs_submit("namd","namd", i , total_number_of_jobs, resources_job_count[i],str(namd_jobs_size[i]))
            logger.info( " resource " + str(i))
            logger.info( "total_number_of_jobs " + str(total_number_of_jobs))
            logger.info( "resources_job_count " + str(resources_job_count[i]))
            logger.info( "int(bfast_num_cores" + str(namd_jobs_size[i]))
            
            total_number_of_jobs = total_number_of_jobs + int(resources_job_count[i])     

        wait_for_jobs(total_number_of_jobs)
        
        namd_runtime = time.time()-namd_starttime
        logger.info( APP_NAME + "Runtime: " + str(namd_runtime) )
        
        #for i in range(0,len(resources_used) ):
         #   mjs[i].cancel()
        
    except:
        traceback.print_exc(file=sys.stdout)
        try:
            for i in range(0,len(resources_used) ):
                mjs[i].cancel()
            
        except:
            pass
