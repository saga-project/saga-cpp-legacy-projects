""" Example application demonstrating job submission via bigjob 
    advert_job implementation of BigJob is used
"""

import saga
import os
import bigjob
import time
import pdb

advert_host = "fortytwo.cct.lsu.edu"

def prepare_NAMD_config():
# The idea behind this is that we can simply modify NPT.conf before submit a job to set temp and other variables
   ifile = open("NPT.conf")   # should be changed if a different name is going to be used
   lines = ifile.readlines()
   for line in lines:
      if line.find("desired_temp") >= 0 and line.find("set") >= 0:
         if(i==0):
            lines[lines.index(line)] = "set desired_temp %s \n"%(str(energy1))
         else:
            lines[lines.index(line)] = "set desired_temp %s \n"%(str(energy0))
   ifile.close()
   ofile = open("NPT.conf","w")
   for line in lines:
     ofile.write(line)
   ofile.close()

def NAMD_config():
# The idea behind this is that we can simply modify NPT.conf before submit a job to set temp and other variables
  ifile = open("NPT.conf")   # should be changed if a different name is going to be used
  lines = ifile.readlines()
  for line in lines:
     if line.find("desired_temp") >= 0 and line.find("set") >= 0:
       if(i==0):
          lines[lines.index(line)] = "set desired_temp %s \n"%(str(temps[i]))
       else:
          lines[lines.index(line)] = "set desired_temp %s \n"%(str(temps[i]))
  ifile.close()
  ofile = open("NPT.conf","w")
  for line in lines:
    ofile.write(line)
  ofile.close()

def update_states_energies():#gets the states and energies
      for i in number of replicas:
        state[i] = str(sjs[i].get_state())
        energy[i] = str(sjs[i].get_energy())

def search():#searches for other replicas which are in DONE state
   i=0
   while(i< number of replicas) and (i!=x[i]):
      if(state[i]==DONE):
         list.append(i)

def select():#by metropolis scheme select a replica
   for i in list[]:
      if(energy[i] is in required range):
         choose i
   return i

def find_partners_exchange():#once replicas/sjs are submitted, continuosly updates states and energies. as and when a replica is DONE, tries to find a partner for that replica. and then choose a partner and restart that replica. 
   exchange count=0
   while 1:
     i = 0
     for i in number of replicas:
       update_states_energies()
         if(state[i]==DONE):
           x[i]=i #exclude the replica itself when looking for partners
           search()
           if(list[]!=NULL):
             print "partners found"
             select() 
             scp the temps to the required dirs
             restart sjs[i]
             count=count+1

""" Test Job Submission via Advert """
if __name__ == "__main__":

    #range of temperatures
    temps=[]
    t=300
    for i in range(0,2):
      temp = t
      t = t+10
      temps.append(temp)

##################################################################################  
  # Start BigJob
    # Parameter for BigJob
    bigjob_agent = os.getcwd() + "/bigjob_agent_launcher.sh" # path to agent
    #bigjob_agent = "/bin/echo"
    nodes = 16 # number nodes for agent
    workingdirectory=os.getcwd() +"/agent"  # working directory for agent
    userproxy = None # userproxy (not supported yet due to context issue w/ SAGA)

    bjs=[]
    i=0
    for i in range(0,2):
      bj = bigjob.bigjob(advert_host)
      bjs.append(bj)
      if(i==0):
        lrms_url = "gram://eric1.loni.org/jobmanager-pbs"
      else:
        lrms_url = "gram://qb1.loni.org/jobmanager-pbs"
      bjs[i].start_pilot_job(lrms_url,
                            bigjob_agent,
                            nodes,
                            None,
                            None,
                            workingdirectory, 
                            userproxy,
                            None)
      print "Start Pilot Job/BigJob: " + bigjob_agent + " at: " + lrms_url
      print "Pilot Job/BigJob URL: " + bjs[i].pilot_url + " State: " + str(bjs[i].get_state())

    ##########################################################################################
    # Submit SubJob through BigJob
    i=0
    jds=[]
    jd = saga.job.description()
    jd.executable = "namd2"
    jd.number_of_processes = "16"
    jd.spmd_variation = "mpi"
    jd.arguments = ["NPT.conf"]
    jd.working_directory = os.getcwd() 
    sjs=[]
    for i in range(0, 2):
    #  jd.arguments = ["NPT" + str(i) + ".conf"]
      jd.output = "rep-" + str(i) + "/stdout-" + str(i) + ".txt"
      jd.error = "rep-" + str(i) + "/stderr-" + str(i) + ".txt"  	
      jds.append(jd)
      sj = bigjob.subjob(advert_host)
      sjs.append(sj)
      #prepare config and scp other files to remote machine
      NAMD_config()
      if(i==1):
        os.system("gsiscp NPT.conf qb1.loni.org:/work/athota1/new_bigjob/") 
      sjs[i].submit_job(bjs[i].pilot_url, jds[i],str(i))
    
    # busy wait for completion
    for i in range(0,2):
      prepare_NAMD_config()
      if(i==1):
         os.system("gsiscp NPT.conf qb1.loni.org:/work/athota1/new_bigjob/bigjob")
      sjs[i].submit_job(bjs[i].pilot_url, jds[i], str(i))
    


    bjs[0].cancel()
    bjs[1].cancel()
