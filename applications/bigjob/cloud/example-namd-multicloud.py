""" Example application demonstrating job submission via bigjob 
    cloud implementation to three different cloud backends
"""

import saga
import os
import bigjob_cloud
import time
import pdb


NUMBER_JOBS=1

""" Test Job Submission of NAMD via Cloud BigJob """
if __name__ == "__main__":

    ##########################################################################################
    # Start BigJob
    # Parameter for BigJob
    nodes = 1 # number nodes for agent
    current_directory=os.getcwd() +"/agent"  # working directory for agent

    # start pilot job (bigjob_agent)
    print "Start Pilot Job/BigJob in the EUCA cloud. "
#    bj_euca = bigjob_cloud.bigjob_cloud()
#    bj_euca.start_pilot_job(number_nodes=nodes, 
#                       working_directory=current_directory,
#                       walltime=300,
#                       cloud_type="EUCA",
#                       image_name="emi-62360E71")
#    print "Pilot Job/BigJob URL: " + bj_euca.pilot_url + " State: " + str(bj_euca.get_state())

    print "Start Pilot Job/BigJob in the EC2 cloud. "
    bj_ec2 = bigjob_cloud.bigjob_cloud()
    bj_ec2.start_pilot_job(number_nodes=nodes, 
                       working_directory=current_directory,
                       walltime=300,
                       cloud_type="EC2",
                       image_name="ami-836e8dea")
    print "Pilot Job/BigJob URL: " + bj_ec2.pilot_url + " State: " + str(bj_ec2.get_state())
    
    print "Start Pilot Job/BigJob in the Nimbus cloud. "
    bj_nimbus = bigjob_cloud.bigjob_cloud()
    bj_nimbus.start_pilot_job(number_nodes=nodes, 
                       working_directory=current_directory,
                       walltime=300,
                       cloud_type="NIMBUS",
                       image_name="gentoo_saga-1.3.3_namd-2.7b1.gz")
    print "Pilot Job/BigJob URL: " + bj_nimbus.pilot_url + " State: " + str(bj_nimbus.get_state())



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
    
    jd_nimubs = saga.job.description()
    jd_nimubs.executable = "/usr/local/NAMD_2.7b1_Linux-x86/charmrun"
    #jd.executable = "/bin/date"
    jd_nimubs.number_of_processes = "1"
    jd_nimubs.spmd_variation = "single"
    jd_nimubs.arguments = ["++remote-shell", "ssh", "++nodelist", "/root/machinefile", "+p2", "/usr/local/NAMD_2.7b1_Linux-x86/namd2", "/root/run/NPT.conf"]
    #jd.working_directory = "/root/run/"
    jd_nimubs.output = "stdout_euca_nimbus.txt"
    jd_nimubs.error = "stderr_euca_nimbus.txt"
    
    
    
    jobs_euca = []
    jobs_ec2 = []
    jobs_nimbus = []
    
    
    for i in range (0, NUMBER_JOBS):
#        print "Start job no.: " + str(i)  + " in EUCA cloud"
#        sj_euca = bigjob_cloud.subjob(bigjob=bj_euca)
#        sj_euca.submit_job(jd_euca)
#        jobs_euca.append(sj_euca)
        
        print "Start job no.: " + str(i) + " in EC2 cloud"
        sj_ec2 = bigjob_cloud.subjob(bigjob=bj_ec2)
        sj_ec2.submit_job(jd_ec2)
        jobs_ec2.append(sj_ec2)
        
        print "Start job no.: " + str(i) + " in NIMBUS cloud"
        sj_nimbus = bigjob_cloud.subjob(bigjob=bj_nimbus)
        sj_nimbus.submit_job(jd_nimbus)
        jobs_nimbus.append(sj_nimbus)
    
    # busy wait for completion
    while 1:
        try:
            
            all_done = 0
            
            # EUCA jobs
            number_done = 0
#            for i in jobs_euca:
#                state = str(i.get_state())
#                print "EUCA job: " + str(i) + " state: " + str(state)
#                if(state=="Failed" or state=="Done"):
#                    number_done = number_done + 1                    
#                
#            if (number_done == len(jobs_euca)):
#                all_done = all_done + 1
            
            # EC 2 jobs    
            for i in jobs_ec2:
                state = str(i.get_state())
                print "EC2 job: " + str(i) + " state: " + str(state)
                if(state=="Failed" or state=="Done"):
                    number_done = number_done + 1                    
                
            if (number_done == len(jobs_ec2)):
                all_done = all_done + 1
    
            # Nimbus Jobs
            for i in jobs_nimbus:
                state = str(i.get_state())
                print "Nimbus job: " + str(i) + " state: " + str(state)
                if(state=="Failed" or state=="Done"):
                    number_done = number_done + 1                    
                
            if (number_done == len(jobs_nimbus)):
                all_done = all_done + 1
            
            if (all_done == 3):
                break
                 
            time.sleep(10)
        except KeyboardInterrupt:
            break

    ##########################################################################################
    # Cleanup - stop BigJob
    bj_euca.cancel()
    bj_ec2.cancel()
    bj_nimbus.cancel()
