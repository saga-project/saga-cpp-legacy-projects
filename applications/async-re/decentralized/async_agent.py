import saga
import os
import subprocess
import logging
import sys

num = sys.argv[2]
command = "mpirun" + " -np " + "16" + " -machinefile " + "/work/athota1/machinefile" + " " + "/work/athota1/new_bigjob/decentralized/agent/" +str(num) + "/namd2" + " /work/athota1/new_bigjob/decentralized/"+str(num)+"/NPT.conf"

  # print "execute: " + command + " in " + "workingdirectory" + " from: " + str(socket.gethostname()) + " (Shell: " +")"
LOG_FILENAME = '/work/athota1/new_bigjob/decentralized/logging_example-'+str(num)+'.out'
logging.basicConfig(filename=LOG_FILENAME,level=logging.DEBUG)
# bash works fine for launching on QB but fails for Abe :-(
logging.debug('parsing')
stdout = open("/work/athota1/new_bigjob/decentralized/agent/"+str(num)+"/"+"stdout-"+str(num), "w")
stderr = open("/work/athota1/new_bigjob/decentralized/agent/"+str(num)+"/"+"stderr-"+str(num), "w")
#subprocess.Popen('echo "hello" > /work/athota1/jump.txt', shell=True, executable="/bin/bash")
logging.debug('not parsed')
#subprocess.Popen(args=command, executable="/bin/bash", stderr=stderr,stdout=stdout,cwd=os.getcwd(),shell=True)
# except e: 
#   print str(e)

agent_url = saga.url("advert://fortytwo.cct.lsu.edu/"+"BigJob/BigJob" + "-" + sys.argv[1] + "/"+str(num)+"/")
agent_dir = saga.advert.directory(agent_url, saga.advert.Create | saga.advert.CreateParents | saga.advert.ReadWrite)
tring = agent_url.get_string()
logging.debug(tring)
agent_dir.set_attribute("state", str(saga.job.Running))
subprocess.Popen(args=command, executable="/bin/bash", stderr=stderr,stdout=stdout,cwd=os.getcwd(),shell=True)
#agent_dir.set_attribute("state", str(saga.job.Done))
#print str(app_dir.get_attribute("state"))
