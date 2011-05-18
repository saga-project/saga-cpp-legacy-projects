""" Example application demonstrating job submission via bigjob 
    cloud implementation to three different cloud backends
"""

import sys
sys.path.append("..")
sys.path.append("../cloud")
sys.path.append("../condor")

# load all required modules
import saga
import os
import bigjob
import bigjob_cloud
import bigjob_condor
import time
import pdb
import threading

""" Test application 
    Start n number of jobs on whatever resources become available
"""

# Condor Configuration
""" Directory with condor_submit wrapper and soft links to original condor_rm, condor_q"""
CONDOR_BIN = "/home/luckow/saga/condor_bin"
X509_USER_PROXY = "/home/luckow/.globus/userproxy.pem"  # or alternatively os.environ.get("X509_USER_PROXY")

resources_list = (
                  { "gram_url" : "poseidon1.loni.org/jobmanager-pbs", "queue" : "checkpt", "project" : "loni_loniadmin1", "number_nodes" : 2, "walltime" : 20},
                  { "gram_url" : "oliver1.loni.org/jobmanager-pbs", "queue" : "checkpt", "project" : "loni_loniadmin1", "number_nodes" : 2, "walltime" : 20},
#                 { "gram_url" : "louie1.loni.org/jobmanager-pbs", "queue" : "checkpt", "project" : "loni_loniadmin1", "number_nodes" : 1, "walltime" : 10},
                )


NUMBER_JOBS_NIMBUS_=2
NUMBER_JOBS_EC2=2
NUMBER_JOBS_GRID_=2
NUMBER_JOBS_CONDOR=2

#BJ Grid
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


""" Test Job Submission of NAMD via Cloud BigJob """
if __name__ == "__main__":

    ##########################################################################################
    # Cloud BigJob
    # Parameter for BigJob
    current_directory=os.getcwd() +"/agent"  # working directory for agent
    start=time.time()

    print "Start Pilot Job/BigJob in the EC2 cloud. "
    bj_ec2 = bigjob_cloud.bigjob_cloud()
    #bj_ec2.start_pilot_job(number_nodes=8, 
    #                   working_directory=current_directory,
    #                   walltime=300,
    #                   cloud_type="EC2",
    #                   image_name="ami-644caf0d")
 
    print "Start Pilot Job/BigJob in the Nimbus cloud. "
    bj_nimbus = bigjob_cloud.bigjob_cloud()
    bj_nimbus.start_pilot_job(number_nodes=8, 
                       working_directory=current_directory,
                       walltime=60,
                       cloud_type="NIMBUS",
                       image_name="gentoo_saga-1.3.3_namd-2.7b1.gz")

    ##########################################################################################
    # Condor BigJob
    # Parameter for BigJob
    bj_condor = bigjob_condor.bigjob_condor()
    for i in resources_list:
        pj = bj_condor.start_pilot_job(lrms_url=i["gram_url"],
                        queue=i["queue"],
                        project=i["project"],
                        number_nodes=i["number_nodes"],
                        walltime=i["walltime"],
                        working_directory="/tmp/luckow",
                        userproxy=X509_USER_PROXY)
        print "Glidein Condor-G Job URL: " + bj_condor.pilot_url + " State: " + str(bj_condor.get_state(pj))


    # ###################################################################################
    # TG/LONI Pilot Job
    # Parameter for BigJob
    re_agent = "/home/luckow/src/bigjob/bigjob_agent_launcher.sh" # path to agent
    nodes = 16  # number nodes for agent
    lrms_url = "gram://poseidon1.loni.org/jobmanager-pbs" # resource url
    #lrms_url = "gram://qb1.loni.org/jobmanager-fork" # resource url
    project = "" #allocation
    queue = "checkpt" # queue (PBS)
    workingdirectory="/home/luckow/"  # working directory
    userproxy = None # userproxy (not supported yet due to context issue w/ SAGA)

    print "Start Pilot Job/BigJob in TG at: " + lrms_url
    bj_tg = bigjob.bigjob(advert_host)
    #bj_tg.start_pilot_job(lrms_url,
    #                       re_agent,
    #                       nodes,
    #                       queue,
    #                       project,
    #                       workingdirectory,
    #                       userproxy,
    #                       "20")

    ##########################################################################################
    # Submit SubJob through BigJob
    # NAMD command: $NAMD_HOME/charmrun ++verbose ++remote-shell ssh ++nodelist nodefile +p4 /usr/local/namd2/namd2 NPT.conf
    # working directory: $HOME/run       
    jd_ec2 = saga.job.description()
    jd_ec2.executable = "/usr/local/NAMD_2.7b1_Linux-x86/charmrun"
    #jd.executable = "/bin/date"
    jd_ec2.number_of_processes = "8"
    jd_ec2.spmd_variation = "single"
    jd_ec2.arguments = ["++remote-shell", "ssh", "++nodelist", "/root/machinefile", "+p16", "/usr/local/NAMD_2.7b1_Linux-x86/namd2", "/root/run/NPT.conf"]
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


    #JD Condor
    jd_condor = saga.job.description()
    jd_condor.executable = "/home/luckow/src/bigjob/condor/condor_namd.sh"
    jd_condor.arguments = ["NPT.conf"]
    jd_condor.working_directory = "/work/lukas/saga/bigjob/data"
    jd_condor.output = "namd.$(CLUSTER).$(PROCESS).$(NODE).out"
    jd_condor.error = "namd.$(CLUSTER).$(PROCESS).$(NODE).err"

    attr = open(CONDOR_BIN + "/condor_attr", "w")
    attr.write("universe = parallel\n")
    attr.write("machine_count = 2\n")                       # number of nodes (not cores)
    attr.write("+WantParallelSchedulingGroups = True\n")    # to avoid running on nodes from different clusters
    attr.close()

    # submit sub-job through big-job
    jd = saga.job.description()
    jd.executable = "/usr/local/packages/namd-2.6-mvapich-1.0-intel10.1/namd2"
    jd.number_of_processes = "8"
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
    jobs_condor = []
    number_started_jobs= 0
    while True:
        try:
            print "*********************************************************"
            print "Pilot Job/BigJob URL: " + bj_ec2.pilot_url + " State: " + str(bj_ec2.get_state_detail()) + " Time since launch: " + str(time.time()-start)
            print "Pilot Job/BigJob URL: " + bj_nimbus.pilot_url + " State: " + str(bj_nimbus.get_state_detail()) + " Time since launch: " + str(time.time()-start)
            print "Pilot Job/BigJob URL: " + bj_tg.pilot_url + " State: " + str(bj_tg.get_state_detail()) + " Time since launch: " + str(time.time()-start)
            print "Pilot Job/BigJob URL: " + bj_condor.pilot_url + " State: " + str(bj_condor.get_state_detail()) + " Time since launch: " + str(time.time()-start)

            if number_started_jobs < NUMBER_JOBS:
                if str(bj_nimbus.get_state_detail())=="Running":
                    print "Nimbus: Free nodes: " + str(bj_nimbus.get_free_nodes()) 
                    if int(bj_nimbus.get_free_nodes()) >= int(jd_nimbus.number_of_processes):
                        print " Start job: " +str(number_started_jobs + 1)
                        sj_nimbus = bigjob_cloud.subjob(bigjob=bj_nimbus)
                        jd_nimbus.output = "stdout_nimbus.txt."+str(number_started_jobs+1)
                        jd_nimbus.error = "stderr_nimbus.txt."+str(number_started_jobs+1)
                        sj_nimbus.submit_job(jd_nimbus)
                        jobs_nimbus.append(sj_nimbus)
                        number_started_jobs = number_started_jobs + 1

                if str(bj_ec2.get_state_detail())=="Running":
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

                if str(bj_condor.get_state_detail())=="Running":
                    print "Condor: Free nodes: " + str(bj_condor.get_free_nodes())
                    if int(bj_condor.get_free_nodes()) >= int(jd_condor.number_of_processes):
                        print "Start job no.: " +str(number_started_jobs + 1)
                        sj_condor = bigjob_condor.subjob(bigjob=bj_condor)
                        jd_condor.output = "stdout_condor.txt."+str(number_started_jobs+1)
                        jd_condor.error = "stderr_condor.txt."+str(number_started_jobs+1)
                        sj_condor.submit_job(bj_condor.pilot_url, jd_condor)
                        jobs_condor.append(sj_condor)
                        number_started_jobs = number_started_jobs + 1


            print str(number_started_jobs)+"/"+str(NUMBER_JOBS) + " started. EC2: " + str(len(jobs_ec2))\
                   + " Nimbus: " + str(len(jobs_nimbus)) + " TG: " + str(len(jobs_tg)) + " Condor: " + str(len(jobs_condor))
            # EC 2 jobs    
            ec2_done=check_all_jobs(jobs_ec2)
            print "EC2: "+ str(ec2_done) + "/"+str(len(jobs_ec2)) + " done. Time since SubJob Start: " + str(time.time()-subjob_start) + " s"
            
            # Nibmus jobs    
            nimbus_done=check_all_jobs(jobs_nimbus)
            print "Nimbus: "+ str(nimbus_done) + "/"+str(len(jobs_nimbus)) + " done. Time since SubJob Start: " + str(time.time()-subjob_start) + " s"

            # TG jobs    
            tg_done=check_all_jobs(jobs_tg)
            print "TG: "+ str(tg_done) + "/"+str(len(jobs_tg)) + " Time since SubJob Start: " + str(time.time()-subjob_start) + " s"

            # Condor jobs    
            condor_done=check_all_jobs(jobs_condor)
            print "Condor: "+ str(condor_done) + "/"+str(len(jobs_condor)) + " Time since SubJob Start: " + str(time.time()-subjob_start) + " s"
                    
            if ((ec2_done+nimbus_done+tg_done+condor_done)==NUMBER_JOBS):
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
    bj_condor.cancel()
    print "Runtime: " + str(time.time()-start)
