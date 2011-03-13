#!/usr/bin/env python
import sys
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

def sub_jobs_submit(job_type, number_of_jobs, jd_executable, jd_number_of_processes ):
        
        jobs = []
        job_start_times = {}
        job_states = {}
        jd = saga.job.description()

        
        for i in range(0, int(number_of_jobs)):
            # create job description
            jd = saga.job.description()
            jd.executable = jd_executable
            jd.number_of_processes = jd_number_of_processes
            jd.spmd_variation = "single"
            if job_type == "reads":
                jd.arguments = ["-n",  "%s" %(bfast_reads_num),  
                                "-o", "%s/reads.%s" %(bfast_reads_dir, bfast_uuid),
                                 "%s/*.csfasta",  "%s/*.qual" %(bfast_raw_reads_dir,bfast_raw_reads_dir)]
                                  
            elif job_type == "count":
                jd.arguments = [" -altr" ,"%s/reads*" %(bfast_reads_dir),  
                                "|",  "/usr/bin/wc", "-l" , 
                                ">", "%s/out.%s.txt"%(bfast_raw_reads_dir, bfast_uuid)]
                                
            elif job_type == "matches":
                jd.arguments = ["match",  
                                "-f",  "%s/%s.fa" %( bfast_ref_genome_dir, refgnome) , 
                                "-A",  "1",
                                "-r",  "%s/reads.%s.%s.fastq"%(bfast_reads_dir, bfast_uuid, i+1),
                                "-n" ,"8" ,
                                "-T" , "%s" %(bfast_tmp_dir),
                                ">" , "%s/bfast.matches.file.%s.%s.bmf" %(bfast_matches_dir,refgnome,i+1)] 
                                
            elif job_type == "localalign":
                jd.arguments = ["localalign", 
                                "-f",  "%s/%s.fa"%(bfast_ref_genome_dir, refgnome),
                                "-A", "1",
                                "-m", "%s/bfast.matches.file.%s.%s.bmf"%(bfast_matches_dir,refgnome,i+1),
                                ">", "%s/bfast.aligned.file.%s.%s.baf" %(bfast_localalign_dir,refgnome,i+1)]
                                
            elif job_type == "postprocess":
                jd.arguments = ["postprocess"
                                "-f",  "%s/%s.fa" %(bfast_ref_genome_dir, refgnome),
                                "-A",  "1" ,
                                "-i", "%s/bfast.aligned.file.%s.%s.baf" %(bfast_localalign_dir,refgnome,i+1), 
                                ">", "%s/bfast.reported.file.%s.%s.sam" %(bfast_reported_dir,refgnome,i+1)]     
            else:
                jd.arguments = [""]
         
            
            jd.working_directory = work_dir
            jd.output =  log_dir + "/stdout_" + job_type + "-"+ str(bfast_uuid) + str(i) + ".txt"
            jd.error = log_dir + "/stderr_"+ job_type + "-"+str(bfast_uuid) +str(i) + ".txt"
            subjob = mjs.create_job(jd)
            subjob.run()
            print "Submited sub-job " + "%d"%i + "."
         
            jobs.append(subjob)
            job_start_times[subjob]=time.time()
            job_states[subjob] = subjob.get_state()
            logger.info( job_type + "subjob " + str(i))
            logger.info( "jd.number_of_processes " + str(jd.number_of_processes))
            logger.info( "jd.arguments" + jd.arguments[0])
            logger.info( "jd exec " + jd.executable)

        print "************************ All Jobs submitted ************************"
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

                  


""" Test Job Submission via ManyJob abstraction """
if __name__ == "__main__":
    config = {}

    #cwd = "/home/cctsg/pylons/DARE-BIOSCOPE/darebioscope/lib/adams/"

    cwd = os.cwd()
 

    bfast_uuid = uuid.uuid1()
    # parse conf files
    parser = optparse.OptionParser()
    
    parser.add_option("-j", "--job-conf", dest="job_conf", help="job configuration file")

    (options, args) = parser.parse_args()
   
       #parse job conf file
    job_conf = options.job_conf
    config = initialize(job_conf)
    refgnome = config.get('Bfast', 'refgnome')
    job_id = config.get('Bfast', 'job_id')
    #number_of_jobs = config.getint('Bfast', 'numberofjobs')
    #number_of_bigjobs = 1
    machine_use=config.get('Bfast', 'machine_use') 
    dir_calc=config.get('Bfast', 'workdir')


    #parse adams conf file
    adams_conf = "adams.conf"
    config = initialize(adams_conf)
    
    work_dir = config.get(machine_use, dir_calc+'_dir')
    print "wor_dir"+work_dir 
    log_dir = config.get(machine_use, 'log_dir')   
    bioscope_exe = config.get(machine_use, 'bioscope_exe')  
    gram_url= config.get(machine_use, 'gram_url') 
    re_agent= config.get(machine_use, 're_agent')
    allocation= config.get(machine_use, 'allocation')
    queue = config.get(machine_use, 'queue')
    num_cores_machine= config.get(machine_use, 'num_cores') 
    bfast_raw_reads_dir = config.get(machine_use, 'bfast_raw_reads_dir') 
    bfast_reads_num = config.get(machine_use, 'bfast_reads_num') 
    bfast_reads_dir = config.get(machine_use, 'bfast_reads_dir') 
    bfast_ref_genome_dir = config.get(machine_use, 'bfast_ref_genome_dir') 
    bfast_tmp_dir = config.get(machine_use, 'bfast_tmp_dir') 
    bfast_matches_dir = config.get(machine_use, 'bfast_matches_dir') 
    bfast_num_threads = config.getint(machine_use, 'bfast_num_threads') 
    bfast_localalign_dir = config.get(machine_use, 'bfast_localalign_dir')
    bfast_reported_dir = config.get(machine_use, 'bfast_reported_dir')
    jd_executable_bfast = bioscope_exe + "bfast"
    jd_executable_solid2fastq = bioscope_exe + "solid2fastq"
    
    
   #parse dare conf file
    dare_conf = "dare.conf"
    config = initialize(dare_conf)
    refgnome = config.get('hg18-ch21', 'filename')
    
    LOG_FILENAME = cwd + 'logfiles/%s_%s_log_bfast.txt'%(job_id, bfast_uuid)

    logger = logging.getLogger('adams_bfast_manyjob')
    hdlr = logging.FileHandler(LOG_FILENAME)
    formatter = logging.Formatter('%(asctime)s %(levelname)s %(message)s')
    hdlr.setFormatter(formatter)
    logger.addHandler(hdlr)
    logger.setLevel(logging.INFO)


    logger.info("Job id  is "  + str(job_id) )
    logger.info("Working Dir is " + work_dir )  
    logger.info("Machine used is " + machine_use )
    logger.info("Log Dir is " + log_dir )
    logger.info("Reference GNOME " + refgnome)
    #logger.info("number_of_jobs " + str(number_of_jobs)) 
    
    
    try:
        #print "ManyJob load test with " + str(number_of_jobs) + " jobs."
        starttime=time.time()

        # submit via mj abstraction
        resource_list1 = []
        resource_list1.append( {"gram_url" : gram_url, "walltime": "60" , 
                                "number_cores" : 8, "allocation" : allocation,
                                "queue" : queue, "re_agent": re_agent, "working_directory": work_dir})
        print "Create manyjob service "
        mjs = many_job.many_job_service(resource_list1, None)
        prep_reads_starttime = time.time()

        ### run the preparing read files step
        
        sub_jobs_submit("new", "1", "/bin/date", "2") ##dummy job for testing
        #sub_jobs_submit("reads" , 1, jd_executable_solid2fastq, 8)
        
        prep_reads_runtime = time.time()-prep_reads_starttime
        logger.info("prepare reads Runtime: " + str( prep_reads_runtime))
        
        
        
        # job to get the count of read files
        sub_jobs_submit("count",  "1", "/bin/ls", "8")
       
       
        # transfer the files
        output = saga.filesystem.file("gridftp://qb1.loni.org//%s/out.%s.txt"%(bfast_raw_reads_dir, bfast_uuid))
        output.copy("file://localhost//%s/logfiles/"%(cwd))
        
        
        f = open(r'%s/logfiles/out.%s.txt'%(cwd, bfast_uuid))
        num_matches=f.readline()
        f.close()
        
        mjs.cancel()
        
       # for testing
        num_matches = 1
        
        resource_list = []
        # for real cal wall time should be > 240 minutes
        resource_list.append( {"gram_url" : gram_url, "walltime": "60" , 
                               "number_cores" : int(int(num_matches)*int(num_cores_machine)), "allocation" : allocation,
                               "queue" : queue, "re_agent": re_agent, "working_directory": work_dir})

        print "Creating manyjob service 2 "
        mjs = many_job.many_job_service(resource_list, None)
        matches_starttime = time.time()
        
        ### run the matching step
        sub_jobs_submit("new", "4", "/bin/date", "2") ##dummy job for testing
        #sub_jobs_submit("matches" , 1, jd_executable_solid2fastq, 8)
        
        matches_runtime = time.time()-matches_starttime
        logger.info("Matches Runtime: " + str( matches_runtime) )
        
        ### run the local-alignment step
        localalign_starttime = time.time()
        
        sub_jobs_submit("new", "4", "/bin/date", "2") ##dummy job for testing
        #sub_jobs_submit("localalign" , 1, jd_executable_solid2fastq, 8)

        localalign_runtime = time.time() - localalign_starttime
        logger.info("localalign Runtime: " + str( localalign_runtime) )

        postprocess_starttime = time.time()

        ### run the postprocess step        
        sub_jobs_submit("new", "4", "/bin/date", "2") ##dummy job for testing
        #sub_jobs_submit("postprocess" , 1, jd_executable_solid2fastq, 8)

        postprocess_runtime = time.time() - postprocess_starttime
        logger.info("Postporcess Runtime: " + str( postprocess_runtime) )
        mjs.cancel()

        
    except:
        traceback.print_exc(file=sys.stdout)
        try:
            mjs.cancel()
        except:
            pass
