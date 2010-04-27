import saga
import os

if __name__ == "__main__":

 try:
  jd = saga.job.description()
  jd.executable = "/bin/date"
  jd.number_of_processes = "1"
  jd.spmd_variation = "single"
 # jd.arguments = ["NPT.conf"]
  jd.arguments = [""]
  jd.working_directory ="/work/athota1/new_bigjob/bigjob/"
 # jd.job_project = ["loni_jhabig09"]
  jd.output = "stdout.txt"
#  jd.wall_time_limit = "15"
  jd.error = "stderr.txt"
  js = saga.job.service("fork://localhost")
  job = js.create_job(jd)
  print "job submitted"
  job.run()
 except saga.exception, e:
  print e.get_all_messages()

