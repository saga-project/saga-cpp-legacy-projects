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

""" Test application 
    Start n number of jobs on whatever resources become available
"""

NUMBER_JOBS=50
RUNTIME_JOB_CLOUD=6 # in min
MAX_RUNTIME=60 #in min
advert_host = "fortytwo.cct.lsu.edu"

def check_all_jobs(jobs):
       """ returns number of Done/Failed jobs """
       number_done=0
       for i in jobs:
            state = str(i.get_state())
            print "Job: " + str(i) + " state: " + str(state)
            if(state=="Failed" or state=="Done" or state==None):
                    number_done=number_done+1
       return number_done

def start_cloud_pilots():
    """ Start BigJob in Clouds """
    # Parameter for BigJob
    nodes = 4 # number nodes for agent
    current_directory=os.getcwd() +"/agent"  # working directory for agent

    print "Start Pilot Job/BigJob in the EC2 cloud. "
    bj_ec2 = bigjob_cloud.bigjob_cloud()
    bj_ec2.start_pilot_job(number_nodes=nodes, 
                       working_directory=current_directory,
                       walltime=300,
                       cloud_type="EC2",
                       image_name="ami-644caf0d")
 
    print "Start Pilot Job/BigJob in the Nimbus cloud. "
    bj_nimbus = bigjob_cloud.bigjob_cloud()
    bj_nimbus.start_pilot_job(number_nodes=nodes, 
                       working_directory=current_directory,
                       walltime=60,
                       cloud_type="NIMBUS",
                       image_name="gentoo_saga-1.3.3_namd-2.7b1.gz")

    return bj_ec2, bj_nimbus

""" Test Job Submission of NAMD via Cloud BigJob """
if __name__ == "__main__":

    start=time.time()
    bj_ec2=None
    bj_nimbus=None
    # ###################################################################################
    # TG/LONI Pilot Job
    # Parameter for BigJob
    re_agent = "/home/luckow/src/bigjob/bigjob_agent_launcher.sh" # path to agent
    nodes = 32  # number nodes for agent
    lrms_url = "gram://poseidon1.loni.org/jobmanager-pbs" # resource url
    #lrms_url = "gram://qb1.loni.org/jobmanager-fork" # resource url
    project = "" #allocation
    queue = "checkpt" # queue (PBS)
    workingdirectory="/home/luckow/"  # working directory
    userproxy = None # userproxy (not supported yet due to context issue w/ SAGA)

    print "Start Pilot Job/BigJob in TG at: " + lrms_url
    bj_tg = bigjob.bigjob(advert_host)
    bj_tg.start_pilot_job(lrms_url,
                           re_agent,
                           nodes,
                           queue,
                           project,
                           workingdirectory,
                           userproxy,
                           "20")

    ##########################################################################################
    # Submit SubJob through BigJob
    # NAMD command: $NAMD_HOME/charmrun ++verbose ++remote-shell ssh ++nodelist nodefile +p4 /usr/local/namd2/namd2 NPT.conf
    # working directory: $HOME/run       
    jd_ec2 = saga.job.description()
    jd_ec2.executable = "/usr/local/NAMD_2.7b1_Linux-x86/charmrun"
    #jd.executable = "/bin/date"
    jd_ec2.number_of_processes = "4"
    jd_ec2.spmd_variation = "single"
    jd_ec2.arguments = ["++remote-shell", "ssh", "++nodelist", "/root/machinefile", "+p8", "/usr/local/NAMD_2.7b1_Linux-x86/namd2", "/root/run/NPT.conf"]
    #jd.working_directory = "/root/run/"
    jd_ec2.output = "stdout_ec2.txt"
    jd_ec2.error = "stderr_ec2.txt"
    
    jd_nimbus = saga.job.description()
    jd_nimbus.executable = "/usr/local/NAMD_2.7b1_Linux-x86/charmrun"
    #jd.executable = "/bin/date"
    jd_nimbus.number_of_processes = "4"
    jd_nimbus.spmd_variation = "single"
    jd_nimbus.arguments = ["++remote-shell", "ssh", "++nodelist", "/root/machinefile", "+p8", "/usr/local/NAMD_2.7b1_Linux-x86/namd2", "/root/run/NPT.conf"]
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
    jd.output = "stdout_tg.txt"
    jd.error = "stderr_tg.txt"

    print "**************** Start SubJob Submission ************** "
    subjob_start = time.time()
    jobs_ec2 = []
    jobs_nimbus = []
    jobs_tg = []
    number_started_jobs= 0

    ec2_done=0
    tg_done=0
    nimbus_done=0
    while True:
        try:
            # check how much time/work is left
            runtime = (time.time() - start)/60 # in min 
            time_left = MAX_RUNTIME - runtime
            jobs_done=ec2_done + tg_done + nimbus_done
            jobs_left=NUMBER_JOBS-jobs_done
            jobs_per_minute = all_done/runtime

            # if half of the time is used up and not half of the jobs have been executed
            # start cloud pilots
            if jobs_left > (NUMBER_JOBS/2) and time_left < MAX_RUNTIME/2:
                bj_ec2, bj_nimbus = start_cloud_pilots()

            print "Pilot Job/BigJob URL: " + bj_tg.pilot_url + " State: " + str(bj_tg.get_state_detail()) + " Time since launch: " + str(time.time()-start)
            if(bj_ec2!=None and bj_nimbus!=None):
                print "Pilot Job/BigJob URL: " + bj_ec2.pilot_url + " State: " + str(bj_ec2.get_state_detail()) + " Time since launch: " + str(time.time()-start)
                print "Pilot Job/BigJob URL: " + bj_nimbus.pilot_url + " State: " + str(bj_nimbus.get_state_detail()) + " Time since launch: " + str(time.time()-start)
            


            if number_started_jobs < NUMBER_JOBS:
                if bj_nimbus!=None and str(bj_nimbus.get_state_detail())=="Running":
                    print "Nimbus: Free nodes: " + str(bj_nimbus.get_free_nodes()) 
                    if int(bj_nimbus.get_free_nodes()) >= int(jd_nimbus.number_of_processes):
                        print " Start job: " +str(number_started_jobs + 1) + " on " + str(jd_nimbus.number_of_processes)
                        sj_nimbus = bigjob_cloud.subjob(bigjob=bj_nimbus)
                        jd_nimbus.output = "stdout_nimbus.txt."+str(number_started_jobs+1)
                        jd_nimbus.error = "stderr_nimbus.txt."+str(number_started_jobs+1)
                        sj_nimbus.submit_job(jd_nimbus)
                        jobs_nimbus.append(sj_nimbus)
                        number_started_jobs = number_started_jobs + 1

                if bj_ec2!=None = str(bj_ec2.get_state_detail())=="Running":
                    print "EC2: Free nodes: " + str(bj_ec2.get_free_nodes())
                    if int(bj_ec2.get_free_nodes()) >= int(jd_ec2.number_of_processes):
                        print " Start job no.: " +str(number_started_jobs + 1)
                        sj_ec2 = bigjob_cloud.subjob(bigjob=bj_ec2)
                        jd_ec2.output = "stdout_ec2.txt."+str(number_started_jobs+1)
                        jd_ec2.error = "stderr_ec2.txt."+str(number_started_jobs+1)
                        sj_ec2.submit_job(jd_ec2)
                        jobs_ec2.append(sj_ec2)
                        number_started_jobs = number_started_jobs + 1

                if str(bj_tg.get_state_detail())=="Running": 
                    print "TG: Free nodes: " + str(bj_tg.get_free_nodes())
                    if int(bj_tg.get_free_nodes()) >= int(jd.number_of_processes):
                        print "Start job no.: " +str(number_started_jobs + 1)
                        sj_tg = bigjob.subjob(advert_host)
                        jd.output = "stdout_tg.txt."+str(number_started_jobs+1)
                        jd.error = "stderr_tg.txt."+str(number_started_jobs+1)
                        sj_tg.submit_job(bj_tg.pilot_url, jd)
                        jobs_tg.append(sj_tg)
                        number_started_jobs = number_started_jobs + 1
                
            print str(number_started_jobs)+"/"+str(NUMBER_JOBS) + " started. EC2: " + str(len(jobs_ec2))\
                   + " Nimbus: " + str(len(jobs_nimbus)) + " TG: " + str(len(jobs_tg))
            # EC 2 jobs    
            ec2_done=check_all_jobs(jobs_ec2)
            print "EC2: "+ str(ec2_done) + "/"+str(len(jobs_ec2)) + " done. Time since SubJob Start: " + str(time.time()-subjob_start) + " s"
            
            # Nibmus jobs    
            nimbus_done=check_all_jobs(jobs_nimbus)
            print "Nimbus: "+ str(nimbus_done) + "/"+str(len(jobs_nimbus)) + " done. Time since SubJob Start: " + str(time.time()-subjob_start) + " s"

            # TG jobs    
            tg_done=check_all_jobs(jobs_tg)
            print "TG: "+ str(tg_done) + "/"+str(len(jobs_tg)) + " Time since SubJob Start: " + str(time.time()-subjob_start) + " s"
                    
            if ((ec2_done+nimbus_done+tg_done)==NUMBER_JOBS):
                print "******** Completed Scenario of " + str(NUMBER_JOBS) + " Runtime " + str(time.time()-subjob_start) + " s"
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
