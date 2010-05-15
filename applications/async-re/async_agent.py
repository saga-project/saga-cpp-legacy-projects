import saga
import os
import subprocess
import logging
#if __name__ == "__main__":
# try:
command = "mpirun" + " -np " + "4" + " -machinefile " + "/work/athota1/machinefile" + " " + "/work/athota1/new_bigjob/bigjob/namd2" + " /work/athota1/new_bigjob/bigjob/NPT.conf"
  
  # print "execute: " + command + " in " + "workingdirectory" + " from: " + str(socket.gethostname()) + " (Shell: " +")"
LOG_FILENAME = '/work/athota1/logging_example.out'
logging.basicConfig(filename=LOG_FILENAME,level=logging.DEBUG)
# bash works fine for launching on QB but fails for Abe :-(
logging.debug('parsing')
#subprocess.Popen('echo "hello" > /work/athota1/jump.txt', shell=True, executable="/bin/bash")
logging.debug('not parsed')
subprocess.Popen(args=command, executable="/bin/bash", stderr=/work/athota1/stderr,stdout=/work/athota1/stdout,cwd=os.getcwd(),shell=True)
# except e: 
#   print str(e)
