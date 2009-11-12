""" Example application demonstrating job submission via bigjob 
    advert_job implementation of BigJob is not used
"""

import bigjob_condor
import saga
import os
import time

""" Directory with condor_submit wrapper and soft links to original condor_rm, condor_q"""
CONDOR_BIN = "/home/latin/saga/condor_bin"
X509_USER_PROXY = "/home/latin/.globus/userproxy.pem"	# or alternatively os.environ.get("X509_USER_PROXY")

resources_list = (
		  { "gram_url" : "poseidon1.loni.org/jobmanager-pbs", "queue" : "checkpt", "project" : "loni_loniadmin1", "number_nodes" : 2, "walltime" : 10},
#		  { "gram_url" : "oliver1.loni.org/jobmanager-pbs", "queue" : "checkpt", "project" : "loni_loniadmin1", "number_nodes" : 1, "walltime" : 10},
#		  { "gram_url" : "louie1.loni.org/jobmanager-pbs", "queue" : "checkpt", "project" : "loni_loniadmin1", "number_nodes" : 1, "walltime" : 10},
		)	

NUMBER_JOBS = 2

""" Test Job Submission of NAMD via Condor BigJob """
if __name__ == "__main__":

	##########################################################################################
	# Start BigJob
	# Parameter for BigJob

	# Create a local Condor pool (glidein master_condor on remote resources via Condor-G/GRAM2)
	print "Create a local Condor pool"

	bigjobs = []
	for i in resources_list:
		bj = bigjob_condor.bigjob_condor()
		bj.start_pilot_job(lrms_url=i["gram_url"],
			queue=i["queue"],
			project=i["project"],
			number_nodes=i["number_nodes"],
			walltime=i["walltime"],
			userproxy=X509_USER_PROXY)
    
		print "Glidein Condor-G Job URL: " + bj.pilot_url + " State: " + str(bj.get_state())
		bigjobs.append(bj)

	##########################################################################################
	# Submit SubJob through BigJob (to the local Condor pool)
	# NAMD command: mpirun -np $v -machinefile machines `which namd2` NPT.conf
	# working directory: $TG_CLUSTER_SCRATCH/saga/bigjob/data

	jd = saga.job.description()
	jd.executable = "/home/latin/saga/condor_namd.sh"
	jd.arguments = ["NPT.conf"]
	jd.working_directory = "/work/lukas/saga/bigjob/data"
	jd.output = "condor_namd.$(CLUSTER).$(PROCESS).$(NODE).out"
	jd.error = "condor_namd.$(CLUSTER).$(PROCESS).$(NODE).err"

	attr = open(CONDOR_BIN + "/condor_attr", "w")
	attr.write("universe = parallel\n")
	attr.write("machine_count = 2\n")		# number of nodes (not cores)
	attr.write("+WantParallelSchedulingGroups = True\n")
	attr.close()

	jobs = []
	for i in range (0, NUMBER_JOBS):
		print "Start job no.: " + str(i)
		sj = bigjob_condor.subjob(bigjob=bj)
		sj.submit_job(jd)
		jobs.append(sj)

	# busy wait for completion
	sj = jobs.pop()
	while 1:
		try:
			state = str(sj.get_state())
			print "state: " + state
			if state=="Failed" or state=="Done" or state=="Canceled" or state=="Suspended":
				if len(jobs) > 0:
					sj = jobs.pop()
					continue
				else:
					break
			time.sleep(10)
		except KeyboardInterrupt:
			break

	#######################################################################
	# Cleaning - stop BigJob - release the local Condor pool
	for i in bigjobs:
		i.cancel()
