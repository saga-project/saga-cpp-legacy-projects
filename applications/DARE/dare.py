#!/usr/bin/env python
import sys
import saga
import getopt
import time
import pdb
import os
import traceback
import logging
import many_job_affinity
import io
import ConfigParser
import optparse
import uuid
#import dare api base
import api.base

global jobs, job_start_times, job_states
jobs = []
job_start_times = {}
job_states = {}

class bigjob(api.base.bigjob):

    def has_finished(state):
        state = state.lower()
        if state=="done" or state=="failed" or state=="canceled":
            return True
        else:
            return False

    def calculate_nodes(c_cores_per_node,c_resources_job_count, task_num_cores):               
        #calculate exact number of cores to request
        k=0
        if (cbnc*crjc%ccpn !=0):
           k =1
        number_nodes = ccpn * (cbnc*crjc/ccpn +k ) 

        
    def read_job_conf(filename):
    
        conf_options = {}
        config = ConfigParser.RawConfigParser()
        config.read(filename)
       
        for option in config.options(resource):
            conf_options[option]=(config.get(resource, option))
                    
        return conf_options
        
    def read_conf(filename,resources_used):
        
        conf_options = {}
        config = ConfigParser.RawConfigParser()
        config.read(filename)
        
        for resource in resources_used:
            resources_info = {}
            for option in config.options(resource):
                print option
                resources_info[option]=(config_ri.get(resource, option) )
                
           resources_used_info[resource]=resources_info
        
    def file_stage(source_url, dest_url):

        logger.info("Now I am tranferring the files from %s to %s"%(source_url, dest_url))
        #fgeuca for clouds
        if dest_url.startswith("fgeuca"):
            try:
                #for cloud files
                cmd = "scp  -r -i /path/to/smaddi2.private %s %s"%(source_url, dest_url)
                os.system(cmd)
            except saga.exception, e:
                error_msg = "File stage in failed : from "+ source_url + " to "+ dest_url
        else:
            try:
                cmd = "globus-url-copy  -cd  %s %s"%(source_url, dest_url)
                os.system(cmd)
            except saga.exception, e:
                error_msg = "File stage in failed : from "+ source_url + " to "+ dest_url
        return None

    def subjob_submit( jd_executable, jd_number_of_processes, jd_spmd_variation, jd_arguments, affinity,
                jd_work_dir, jd_output, jd_error):
                                 
            # create job description
            jd = saga.job.description()
            jd.executable = str(jd_executable)            
            jd.number_of_processes = str(jd_number_of_processes)
            jd.spmd_variation = jd_spmd_variation            
            # choose the job arguments based on type of job
            jd.arguments = jd_arguments            
            jd.environment = ["affinity=%s"%(affinity)]
            jd.working_directory = jd_work_dir
            jd.output =  jd_output
            jd.error = jd_error
            subjob = mjs_affinity.create_job(jd)
            subjob.run()
            print "Submited sub-job " + "%d"%i + "."        
            jobs.append(subjob)
            job_start_times[subjob]=time.time()
            job_states[subjob] = subjob.get_state()
            logger.info( "subjob " + str(i))
            logger.info( "jd.number_of_processes " + str(jd.number_of_processes))
            for item in jd.arguments:
                logger.info( "jd.arguments" + item)
                print " ",item
            logger.info("%s"%(affinity))
            logger.info( "jd exec " + jd.executable)
            
 
    def wait_for_subjobs(number_of_jobs):               

        print logger.info("********All Jobs submitted********" +  str(number_of_jobs))
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
                  
     def launch_manyjob(self,resource_list):
        try:  
               
            # submit via mj abstraction        
            
            ## start the big job agents
            resource_list = []
            i =0
            print all_resources_used[resource]["resource_url"]                
            
            for resource in resources_used:
                
                num_nodes= calculate_nodes(resources_used_info[resource]["cores_per_node"],\
                                           resources_job_count[i],\
                                           resources_app[resource][cores])
            
                resource_list.append({ \
                        "resource_url" :  resources_used_info[resource]["resource_url"] , \
                        "walltime": resources_used_info[resource]["walltime"] , \
                        "number_nodes" : str(1), \
                        "cores_per_node" : resources_used_info[resource]["cores_per_node"], \
                        "allocation" : resources_used_info[resource]["allocation"], \
                        "queue" : resources_used_info[resource]["queue"], \
                        "bigjob_agent":  resources_used_info[resource]["bigjob_agent"], \
                        "userproxy": resources_used_info[resource]["proxy"], \
                        "working_directory":  resources_used_info[resource]["work_dir"],\
                        "affinity" :  resources_used_info[resource]["affinity"} \
                        )

                logger.info("resource_url" + resources_url[i])
                logger.info("affinity%s"%(i))            
                print "Create manyjob service "
                #create multiple manyjobs should be changed by bfast affinity implementation
                i = i+1
            mjs_affinity = many_job_affinity.many_job_service(resource_list, "advert.cct.lsu.edu")
        except:
            traceback.print_exc(file=sys.stdout)
            try:
                 mjs_affinity.cancel()           
            except:
                pass
        return mjs_affinity        

if __name__ == "__main__":
    config = {}
  
    #define app name
    DARE_APP_NAME="BFAST"   
    dare_uuid = uuid.uuid1()
    
    # parse conf files
    parser = optparse.OptionParser()    
    parser.add_option("-j", "--job-conf", dest="job_conf", help="job configuration file")
    (options, args) = parser.parse_args()
      
    #parse job conf file
    job_conf = options.job_conf

    job_info = read_job_conf(job_conf)
   
    job_id = job_info['job_id']    
    resources_used = []                 
    resources_used = job_info['resources_use'].replace(' ','').split(',')   
    resources_used_job_count = job_info['resources_job_count'].replace(' ','').split(',')
    resources_used_walltime = job_info['resources_walltime'].replace(' ','').split(',')
            
    #read resource_info conf file
    resources_used_info = read_conf(job_info['resources_info_filename'],resources_used)
    
    #read resource_app_info conf file
    resources_used_app_info = read_conf(job_info['resources_app_info_filename'],resources_used)
      
    
    #get the current working directory
    cwd = os.getcwd()
    
    #define log filename  
    LOG_FILENAME = os.path.join(cwd, 'dare_files/logfiles/', '%s_%s_log_%s.txt'%(job_id, \
                               dare_uuid,DARE_APP_NAME))

    logger = logging.getLogger('dare_bfast_manyjob')
    hdlr = logging.FileHandler(LOG_FILENAME)
    formatter = logging.Formatter('%(asctime)s %(levelname)s %(message)s')
    hdlr.setFormatter(formatter)
    logger.addHandler(hdlr)
    logger.setLevel(logging.INFO)
    
    logger.info("Reading conf files is done "  + str(job_id) )
    logger.info("Job id  is "  + str(job_id) )
    logger.info("Machine used is " + resources_used[0] )
    logger.info("Reference GNOME " + refgenome)
    
    #launch manyjob affinity    
    mjs_affinity = launch_manyjob(self,resource_list):
    
    try:    
        #sample subjob
        subjob_submit("/bin/date" , 1, "single", [""], "LONI", \
                    "/work/smaddi2/", "/work/smaddi2/stdout-1-now.out",  \
                    "/work/smaddi2/stderr-1-now.out") 
                    
        # wait for submitted subjobs to get to state done
        wait_for_subjobs(1) 
        
        #cancel the manyjob after done
        mjs_affinity.cancel()

    except:
        traceback.print_exc(file=sys.stdout)
        try:
             mjs_affinity.cancel()           
        except:
            pass