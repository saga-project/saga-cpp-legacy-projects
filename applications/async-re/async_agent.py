import saga
import os

    

jd = saga.job.description()
#jd.executable = "/bin/date"
jd.executable = "/work/athota1/new_bigjob/bigjob/namd2"
jd.number_of_processes = "1"
jd.spmd_variation = "single"
jd.arguments = [""]
jd.working_directory = os.getcwd()
jd.output = "stdout.txt"
jd.error = "stderr.txt"
js = saga.job.service("localhost")# is this right?
job = js.create_job(jd)
print "job submitted"
job.run()
"""
get_state()
publish state to advert service 
if done/free look for exchange partners
the replica which initiates the exchange, that is, the one in pending state, will increment the exchange count, while the replica in "free" state will not increment the count"""
