import saga
import os
import subprocess
#if __name__ == "__main__":
# try:
   #command = "mpirun" + " -np " + "4" + " -machinefile " + "/work/athota1/machinefile" + " " + "/work/athota1/new_bigjob/bigjob/namd2" + " /work/athota1/new_bigjob/bigjob/NPT.conf"
  
  # print "execute: " + command + " in " + "workingdirectory" + " from: " + str(socket.gethostname()) + " (Shell: " +")"
# bash works fine for launching on QB but fails for Abe :-(
subprocess.Popen('echo "hello"', shell=True, executable="/bin/bash")
#subprocess.Popen(args=command, executable="/bin/bash", stderr="stderr",stdout="stdout",cwd=os.getcwd(),shell=True)
# except e: 
#   print str(e)
