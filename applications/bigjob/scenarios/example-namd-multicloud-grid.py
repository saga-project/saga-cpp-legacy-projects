""" Example application demonstrating job submission via bigjob 
    cloud implementation to three different cloud backends
"""

import sys
sys.path.append("..")
sys.path.append("../cloud")

# load all required modules
import saga
import os
import bigjob
import bigjob_cloud
import time
import pdb
import threading

NUMBER_JOBS=1
advert_host = "fortytwo.cct.lsu.edu"

def check_all_jobs(jobs):
       """ return True if all jobs have been terminated """
       number_done=0
       for i in jobs:
            state = str(i.get_state())
            print "Job: " + str(i) + " state: " + str(state)
            if(state=="Failed" or state=="Done" or state==None):
                    number_done=number_done+1

       if (number_done == len(jobs)):
            return True
       else:
            return False    


""" Test Job Submission of NAMD via Cloud BigJob """
if __name__ == "__main__":

    ##########################################################################################
    # Start BigJob
    # Parameter for BigJob
    nodes = 1 # number nodes for agent
    current_directory=os.getcwd() +"/agent"  # working directory for agent
    start=time.time()

    print "Start Pilot Job/BigJob in the EC2 cloud. "
    bj_ec2 = bigjob_cloud.bigjob_cloud()
    
    big_ec2_args = {"number_nodes":nodes, 
            "working_directory":current_directory,
            "walltime":300,
            "cloud_type":"EC2",
            "image_name":"ami-644caf0d"}
    big_ec2_thread=threading.Thread(target=bj_ec2.start_pilot_job,
                                    kwargs=big_ec2_args)
    big_ec2_thread.start()      
    
    print "Start Pilot Job/BigJob in the Nimbus cloud. "
    big_nimbus_args = {"number_nodes":nodes, 
            "working_directory":current_directory,
            "walltime":300,
            "cloud_type":"NIMBUS",
            "image_name":"gentoo_saga-1.3.3_namd-2.7b1.gz"}
    bj_nimbus = bigjob_cloud.bigjob_cloud()
    big_nimbus_thread=threading.Thread(target=bj_nimbus.start_pilot_job,
                                    kwargs=big_nimbus_args)
    big_nimbus_thread.start()

    # ###################################################################################
    # TG/LONI Pilot Job
    # Parameter for BigJob
    re_agent = "/home/luckow/src/bigjob/bigjob_agent_launcher.sh" # path to agent
    nodes = 8  # number nodes for agent
    lrms_url = "gram://qb1.loni.org/jobmanager-pbs" # resource url
    #lrms_url = "gram://qb1.loni.org/jobmanager-fork" # resource url
    project = "" #allocation
    queue = "workq" # queue (PBS)
    workingdirectory="/home/luckow/"  # working directory
    userproxy = None # userproxy (not supported yet due to context issue w/ SAGA)

    print "Start Pilot Job/BigJob in TG at: " + lrms_url
    bj_tg = bigjob.bigjob(advert_host)
    #big_tg_args=[lrms_url,re_agent, nodes, queue, project, workingdirectory, userproxy, "120"]
    #big_tg_thread=threading.Thread(target=bj_tg.start_pilot_job,
    #                               args=big_tg_args)
    #big_tg_thread.start()

    bj_tg.start_pilot_job(lrms_url,
                           re_agent,
                           nodes,
                           queue,
                           project,
                           workingdirectory,
                           userproxy,
                           "120")
    big_ec2_thread.join()
    big_nimbus_thread.join()
    #big_tg_thread.join()

    # Barrier
    while bj_tg.get_state_detail()!="Running" and bj_ec2.get_state_detail()!="Running" and bj_ec2.get_state_detail!="Running":
        print "Pilot Job/BigJob URL: " + bj_ec2.pilot_url + " State: " + str(bj_ec2.get_state()) + " Time since launch: " + str(time.time()-start)
        print "Pilot Job/BigJob URL: " + bj_nimbus.pilot_url + " State: " + str(bj_nimbus.get_state()) + " Time since launch: " + str(time.time()-start)
        print "Pilot Job/BigJob URL: " + bj_tg.pilot_url + " State: " + str(bj_tg.get_state()) + " Time since launch: " + str(time.time()-start)
        time.sleep(10)

    ##########################################################################################
    # Submit SubJob through BigJob
    # NAMD command: $NAMD_HOME/charmrun ++verbose ++remote-shell ssh ++nodelist nodefile +p4 /usr/local/namd2/namd2 NPT.conf
    # working directory: $HOME/run       
    jd_euca = saga.job.description()
    jd_euca.executable = "/usr/local/NAMD_2.7b1_Linux-x86/charmrun"
    #jd.executable = "/bin/date"
    jd_euca.number_of_processes = "1"
    jd_euca.spmd_variation = "single"
    jd_euca.arguments = ["++remote-shell", "ssh", "++nodelist", "/root/machinefile", "+p2", "/usr/local/NAMD_2.7b1_Linux-x86/namd2", "/root/run/NPT.conf"]
    #jd.working_directory = "/root/run/"
    jd_euca.output = "stdout_euca.txt"
    jd_euca.error = "stderr_euca.txt"
    
    jd_ec2 = saga.job.description()
    jd_ec2.executable = "/usr/local/NAMD_2.7b1_Linux-x86/charmrun"
    #jd.executable = "/bin/date"
    jd_ec2.number_of_processes = "1"
    jd_ec2.spmd_variation = "single"
    jd_ec2.arguments = ["++remote-shell", "ssh", "++nodelist", "/root/machinefile", "+p2", "/usr/local/NAMD_2.7b1_Linux-x86/namd2", "/root/run/NPT.conf"]
    #jd.working_directory = "/root/run/"
    jd_ec2.output = "stdout_ec2.txt"
    jd_ec2.error = "stderr_ec2.txt"
    
    jd_nimbus = saga.job.description()
    jd_nimbus.executable = "/usr/local/NAMD_2.7b1_Linux-x86/charmrun"
    #jd.executable = "/bin/date"
    jd_nimbus.number_of_processes = "1"
    jd_nimbus.spmd_variation = "single"
    jd_nimbus.arguments = ["++remote-shell", "ssh", "++nodelist", "/root/machinefile", "+p2", "/usr/local/NAMD_2.7b1_Linux-x86/namd2", "/root/run/NPT.conf"]
    #jd.working_directory = "/root/run/"
    jd_nimbus.output = "stdout_nimbus.txt"
    jd_nimbus.error = "stderr_nimbus.txt"

    # submit sub-job through big-job
    jd = saga.job.description()
    jd.executable = "/usr/local/packages/namd-2.6-mvapich-1.0-intel10.1/namd2"
    jd.number_of_processes = "32"
    jd.spmd_variation = "mpi"
    jd.arguments = ["/home/luckow/run/NPT.conf"]
    # !!Adjust!!
    jd.working_directory = "/home/luckow/run/"
    jd.output = "output.txt"
    jd.error = "error.txt"
    subjob = bigjob.subjob(advert_host)
    subjob.submit_job(bj_tg.pilot_url, jd)


    jobs_ec2 = []
    jobs_nimbus = []
    jobs_tg = []
    
    for i in range (0, NUMBER_JOBS):
        
        print "Start job no.: " + str(i) + " in EC2 cloud"
        sj_ec2 = bigjob_cloud.subjob(bigjob=bj_ec2)
        sj_ec2.submit_job(jd_ec2)
        jobs_ec2.append(sj_ec2)
        
        print "Start job no.: " + str(i) + " in NIMBUS cloud"
        sj_nimbus = bigjob_cloud.subjob(bigjob=bj_nimbus)
        sj_nimbus.submit_job(jd_nimbus)
        jobs_nimbus.append(sj_nimbus)

        print "Start job no.: " +str(i) + " in TG"
        sj_tg = bigjob.subjob(advert_host)
        sj_tg.submit_job(bj_tg.pilot_url, jd)
        jobs_tg.append(sj_tg)
    
    # busy wait for completion
    while 1:
        try:
            
            # EC 2 jobs    
            ec2_done=check_all_jobs(jobs_ec2);
            print "EC2 All Done? "+ str(ec2_done)
            
            # Nibmus jobs    
            nimbus_done=check_all_jobs(jobs_nimbus);
            print "Nimbus All Done? "+ str(nimbus_done)

            # TG jobs    
            tg_done=check_all_jobs(jobs_tg);
            print "TG All Done? "+ str(tg_done)
                    
            if ec2_done and nimbus_done and tg_done:
                break
                 
            time.sleep(10)
        except KeyboardInterrupt:
            break

    ##########################################################################################
    # Cleanup - stop BigJob
    #bj_euca.cancel()
    bj_ec2.cancel()
    bj_nimbus.cancel()
    bj_tg.cancel()
    print "Runtime: " + str(time.time()-start)
