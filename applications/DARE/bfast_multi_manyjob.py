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


def sub_jobs_submit( jd_executable, job_type, affinity ,  subjobs_start,  number_of_jobs, jd_number_of_processes):
                                 
        jd = saga.job.description()
        
        for i in range(subjobs_start, int(number_of_jobs) + int(subjobs_start) ):

            ##pick the executble 
            if  jd_executable == "bfast":
                 jd_executable_use = bfast_exe[affinity] + "/bfast"
            elif jd_executable == "solid2fastq":   
                 jd_executable_use = bfast_exe[affinity] + "/solid2fastq"
 
            else:
                 jd_executable_use = jd_executable
         
            # create job description
            jd = saga.job.description()
            print jd_executable_use
            jd.executable = jd_executable_use
            
            jd.number_of_processes = str(jd_number_of_processes)
            jd.spmd_variation = "single"
            
            # choose the job description based on type of job

            if job_type == "reads":
                jd.arguments = ["-n",  "%s" %(bfast_reads_num[affinity]),  
                                "-o", "%s/%s.%s" %(bfast_reads_dir[affinity],shortreads_name ,bfast_uuid),
                                 "%s/*.csfasta"%(bfast_raw_reads_dir[affinity]),
                                 "%s/*.qual" %(bfast_raw_reads_dir[affinity])]
                                  
            elif job_type == "count":
                jd.arguments = [" -altr" , "%s/%s.%s.*" %(bfast_reads_dir[affinity],shortreads_name ,bfast_uuid), 
                                "|",  "/usr/bin/wc", "-l" , 
                                ">", "%s/out.%s.txt"%(bfast_raw_reads_dir[affinity], bfast_uuid)]
                                
            elif job_type == "matches":
                jd.arguments = ["match",  
                                "-f",  "%s/%s.fa" %( bfast_ref_genome_dir[affinity], refgnome) , 
                                #"-A",  "1",
                                "-r",  "%s/%s.%s.fastq"%(bfast_reads_dir[affinity], shortreads_name,i+1),
                                #"-r",  "%s/reads.%s.%s.fastq"%(bfast_reads_dir[affinity], bfast_uuid, i+1),
                                "-n" ,"8" ,
                                "-T" , "%s" %(bfast_tmp_dir[affinity]),
                               # ">" , "%s/bfast.matches.file.%s.%s.bmf" %(bfast_matches_dir[affinity],refgnome,i+1)] 
                                ">" , "%s/bfast.matches.file.%s.%s.%s.bmf" %(bfast_matches_dir[affinity],bfast_uuid,refgnome,i+1)]  

                                
            elif job_type == "localalign":
                jd.arguments = ["localalign", 
                                "-f",  "%s/%s.fa"%(bfast_ref_genome_dir[affinity], refgnome),
                                "-A", "1",
                                #"-m", "%s/bfast.matches.file.%s.%s.bmf"%(bfast_matches_dir[affinity],refgnome,i+1),
                                "-m", "%s/bfast.matches.file.%s.%s.%s.bmf"%(bfast_matches_dir[affinity],bfast_uuid,refgnome,i+1),
                                #">", "%s/bfast.aligned.file.%s.%s.baf" %(bfast_localalign_dir[affinity],refgnome,i+1)]
                                ">", "%s/bfast.aligned.file.%s.%s.%s.baf" %(bfast_localalign_dir[affinity],bfast_uuid,refgnome,i+1)]
                                
            elif job_type == "postprocess":
                jd.arguments = ["postprocess",
                                "-f",  "%s/%s.fa" %(bfast_ref_genome_dir[affinity], refgnome),
                                "-A",  "1" ,
                                #"-i", "%s/bfast.aligned.file.%s.%s.baf" %(bfast_localalign_dir[affinity],refgnome,i+1), 
                                "-i", "%s/bfast.aligned.file.%s.%s.%s.baf" %(bfast_localalign_dir[affinity],bfast_uuid,refgnome,i+1),
                                #">", "%s/bfast.postprocess.file.%s.%s.sam" %(bfast_postprocess_dir[affinity],refgnome,i+1)]
                                ">", "%s/bfast.postprocess.file.%s.%s.%s.sam" %(bfast_postprocess_dir[affinity],bfast_uuid,refgnome,i+1)]     
            else:
                jd.arguments = [""]
            
            #jd.environment = ["affinity=affinity%s"%(affinity)]
            print "affinity%s"%(affinity)
            jd.working_directory = work_dir[affinity]
            jd.output =  os.path.join(work_dir[affinity], "stdout_" + job_type + "-"+ str(bfast_uuid)+"-"+ str(i) + ".txt")
            jd.error = os.path.join(work_dir[affinity], "stderr_"+ job_type + "-"+str(bfast_uuid)+ "-"+str(i) + ".txt")
            subjob = mjs[int(affinity)].create_job(jd)
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

    bfast_uuid = uuid.uuid1()
    
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
    
    refgnome = config.get('Bfast', 'refgnome')
    job_id = config.get('Bfast', 'job_id')
    machu = config.get('Bfast', 'resources_use')
    resources_used = machu.replace(' ','').split(',')    
    machs = config.get('Bfast', 'resources_job_count')
    resources_job_count = machs.replace(' ','').split(',')
    refgnome = config.get('Bfast', 'refgnome')
    source_refgnome =config.get('Bfast', 'source_refgnome')
    source_raw_reads =config.get('Bfast', 'source_raw_reads')
    source_shortreads =config.get('Bfast', 'source_shortreads')
    ##to check whether to run the prepare_read files step?
    prepare_shortreads = config.get('Bfast', 'prepare_shortreads')
    resource_list = config.get('Bfast', 'resource_list')
    resource_app_list = config.get('Bfast', 'resource_app_list')
    shortreads_name = config.get('Bfast', 'shortreads_name')
    walltime = config.get('Bfast', 'walltime')

    
    work_dir = []
    gram_url= []
    re_agent= []
    allocation= []
    queue = []
    processors_per_node = []
    machine_proxy = []
    ft_name= []
    #parse dare_resource conf file
    
    config = initialize(resource_list)
    
    for machine in resources_used:
        print machine
        work_dir.append(config.get(machine, 'work_dir'))
        if (config.get(machine, 'RESOURCE_proxy') == "NA") :
           machine_proxy.append(None)
        else:
           machine_proxy.append(config.get(machine, 'RESOURCE_proxy'))
        gram_url.append(config.get(machine, 'gram_url')) 
        re_agent.append(config.get(machine, 're_agent'))
        allocation.append(config.get(machine, 'allocation'))
        queue.append( config.get(machine, 'queue'))
        processors_per_node.append(config.get(machine, 'processors_per_node'))
        ft_name.append(config.get(machine, 'ft_name'))
        
    #dare_bfast conf file applicatio specific config file
    
    bfast_exe = []
    bfast_raw_reads_dir = [] 
    bfast_reads_num = [] 
    bfast_reads_dir = []
    bfast_ref_genome_dir = [] 
    bfast_tmp_dir = []
    bfast_matches_dir = []
    bfast_num_cores = []
    bfast_localalign_dir = []
    bfast_postprocess_dir = []
    jd_executable_bfast = []
    jd_executable_solid2fastq = []

    
    
    config = initialize(resource_app_list)
    
    for machine in ['fgeuca']:
        print machine
        
        bfast_exe.append(config.get(machine, 'bfast_exe'))
        bfast_raw_reads_dir.append(config.get(machine, 'bfast_raw_reads_dir'))
        bfast_reads_num.append(config.get(machine, 'bfast_reads_num') )
        bfast_reads_dir.append(config.get(machine, 'bfast_reads_dir') )
        bfast_ref_genome_dir.append(config.get(machine, 'bfast_ref_genome_dir') )
        bfast_tmp_dir.append(config.get(machine, 'bfast_tmp_dir') )
        bfast_matches_dir.append(config.get(machine, 'bfast_matches_dir'))
        bfast_num_cores.append(config.getint(machine, 'bfast_num_cores_threads'))
        bfast_localalign_dir.append(config.get(machine, 'bfast_localalign_dir'))
        bfast_postprocess_dir.append(config.get(machine, 'bfast_postprocess_dir'))        
        
    
    LOG_FILENAME = os.path.join(cwd, 'dare_files/logfiles/', '%s_%s_log_bfast.txt'%(job_id, bfast_uuid))

    logger = logging.getLogger('dare_bfast_manyjob')
    hdlr = logging.FileHandler(LOG_FILENAME)
    formatter = logging.Formatter('%(asctime)s %(levelname)s %(message)s')
    hdlr.setFormatter(formatter)
    logger.addHandler(hdlr)
    logger.setLevel(logging.INFO)


    logger.info("Job id  is "  + str(job_id) )
    logger.info("Machine used is " + resources_used[0] )
    logger.info("Reference GNOME " + refgnome)
    
    
    try:  
   
        
        
        # submit via mj abstraction        
        
        ## start the big job agents
        resource_list = []
        mjs = []
        
        for i in range(0,len(resources_used) ):
            
            resource_list.append([])
                       
            resource_list[i].append({"gram_url" : gram_url[i], "walltime": walltime ,
                                   "number_cores" : str(int(resources_job_count[i])*int(bfast_num_cores[i])), "processes_per_node":processors_per_node[i], "allocation" : allocation[i],
                                   "queue" : queue[i], "re_agent": re_agent[i], "userproxy": machine_proxy[i], "working_directory": work_dir[i]})

            logger.info("gram_url" + gram_url[i])
            logger.info("affinity%s"%(i))            
            print "Create manyjob service "
            mjs.append(many_job.many_job_service(resource_list[i], None))
       
        """
        ### transfer the needed files
        if not (source_refgnome == "NONE"):       
            for i in range(0,len(resources_used) ):
                globus_file_stage("file://%s"%(source_refgnome), ft_name[i]+bfast_ref_genome_dir[i])        
        
        if not (source_refgnome == "NONE"):       
            for i in range(0,len(resources_used) ):
                globus_file_stage("file://" + source_raw_reads, ft_name[i]+bfast_raw_reads_dir[i])
                        
        if not (reads_refgnome == "NONE"):       
            for i in range(0,len(resources_used) ):
                globus_file_stage("file://" + source_shortreads, ft_name[i]+bfast_reads_dir[i])     

        ### transfer the needed files
        if not (source_refgnome == "NONE"):       
            for i in range(0,len(resources_used) ):
                cloud_file_stage("file://%s"%(source_refgnome), ft_name[i]+bfast_ref_genome_dir[i])        
        
        if not (source_refgnome == "NONE"):       
            for i in range(0,len(resources_used) ):
                cloud_file_stage("file://" + source_raw_reads, ft_name[i]+bfast_raw_reads_dir[i])
                       
        if not (source_shortreads == "NONE"):       
            for i in range(0,len(resources_used) ):
                for k in range(i+1,i+5):
                    cloud_file_stage(source_shortreads+"readss.%s.fastq"%(k), ft_name[i]+bfast_reads_dir[i])     
        """ 

        ####file tramfer step
        #globus_file_stage(,)
        """
        if (prepare_shortreads == "true"):
            
            prep_reads_starttime = time.time
            ### run the preparing read files step
            #sub_jobs_submit("new", "2" ,"4","/bin/date", "2") ##dummy job for testing
            sub_jobs_submit("reads" , "10" ,"1", "solid2fastq", "4")
            prep_reads_runtime = time.time()-prep_reads_starttime
            
            logger.info("prepare reads Runtime: " + str( prep_reads_runtime))
              
            # job to get the count of number of read files
            sub_jobs_submit("count",  "10","1", "/bin/ls", "8")
       
            # transfer the files
            output = saga.filesystem.file("gridftp://eric1.loni.org//%s/out.%s.txt"%(bfast_raw_reads_dir, bfast_uuid))
            output.copy("file://localhost//%s/dare_files/"%(cwd))        
        
            f = open(r'%s/logfiles/out.%s.txt'%(cwd, bfast_uuid))
            num_reads=f.readline()
            f.close()
            
            ### tranfer the prepared read files to other resources
            for i in range(1,len(resources_used) ):
                globus_file_stage( ft_name[0] +bfast_reads_dir[0] , ft_name[i]+bfast_reads_dir[i])     
      
        """  
        
        matches_starttime = time.time()
        
        ### run the matching step
        #sub_jobs_submit("new", "4", "/bin/date", "2") ##dummy job for testing
        #sub_jobs_submit(0, "matches" , "15", "30", "bfast", "2")
        
        total_number_of_jobs=0
        
        #sub_jobs_submit( jd_executable, job_type, affinity = 0,  subjobs_start = 0 ,  number_of_jobs = 0, jd_number_of_processes = 0 ):

        for i in range (0, len(resources_used)):
            
            sub_jobs_submit("bfast","matches", i , total_number_of_jobs , resources_job_count[i],int(bfast_num_cores[i]))
            logger.info( " machine " + str(i))
            logger.info( "total_number_of_jobs " + str(total_number_of_jobs))
            logger.info( "resources_job_count " + str(resources_job_count[i]))
            logger.info( "int(bfast_num_cores" + str(bfast_num_cores[i]))
            
            total_number_of_jobs = total_number_of_jobs + int(resources_job_count[i])     

        wait_for_jobs(total_number_of_jobs)
        
        matches_runtime = time.time()-matches_starttime
        logger.info("Matches Runtime: " + str( matches_runtime) )
        
        """
        ### run the local-alignment step
        localalign_starttime = time.time()
        
        #sub_jobs_submit("new", "4", "/bin/date", "2") ##dummy job for testing
        sub_jobs_submit("localalign" , "15", "30", "bfast", "2")

        localalign_runtime = time.time() - localalign_starttime
        logger.info("localalign Runtime: " + str( localalign_runtime) )

        postprocess_starttime = time.time()

        ### run the postprocess step        
        #sub_jobs_submit("new", "4", "/bin/date", "2") ##dummy job for testing
        sub_jobs_submit("postprocess" , "15", "30", "bfast", "2")

        postprocess_runtime = time.time() - postprocess_starttime
        logger.info("Postporcess Runtime: " + str( postprocess_runtime) )
        """
        for i in range(0,len(resources_used) ):
            mjs[i].cancel()

    except:
        traceback.print_exc(file=sys.stdout)
        try:
            for i in range(0,len(resources_used) ):
                mjs[i].cancel()
            
        except:
            pass
