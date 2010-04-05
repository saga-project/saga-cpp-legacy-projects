""" Example application demonstrating job submission via bigjob 
    advert_job implementation of BigJob is used
"""

import saga
import os
import bigjob
import time
import pdb

#Configure here:
NUMBER_EXCHANGES = 2
NUMBER_BIGJOBS = 2
NUMBER_REPLICAS = 2
HOST = "eric1.loni.org"
REMOTE1 = "qb1.loni.org"
REMOTE2 = "oliver1.loni.org"
advert_host = "fortytwo.cct.lsu.edu"
#create dirs for replicas
HOST_DIR = "/work/athota1/new_bigjob/bigjob/agent"
REMOTE_DIR = "/work/athota1/new_bigjob/bigjob/agent"

def states_energies():
  state[]
  energy[]
  for i in range(0,NUMBER_REPLICAS):
     states = str(sjs[i].get_state())
     energies = str(sjs[i].get_energy())
     state.append(states)
     energy.append(energies)

#def update_states_energies():#gets the states and energies
 #  for i in number of replicas:
  #    state[i] = str(sjs[i].get_state())
   #   energy[i] = str(sjs[i].get_energy())

def search():#searches for other replicas which are in DONE state
   i=0
   list[]
   for i in range(0, NUMBER_REPLICAS):   
       if((state[i]==DONE) and (i!=j)):
         list.append(i)

def find_partners_exchange():#once replicas/sjs are submitted, continuosly updates states and energies. as and when a replica is DONE, tries to find a partner for that replica. and then choose a partner and restart that replica. 
   count=0
   while (count < NUMBER_EXCHANGES):
     i = 0
     states_energies()
     for i in range(0, NUMBER_REPLICAS):
         if(state[i]==DONE):
           j=i #exclude the replica itself when looking for partners
           search()
           if(len(list[])!=0):
             print "possible exchange partners found"
             for k in list[]:
                if(energy[k] < 1):
                break
             prepare_NAMD_config(k)
#will use SAGA
             os.system(gsiscp NPT.conf #to corresponding machines/dir
#should submit to the corresponding bigjob
             sjs[i].submit_job(bjs[i].pilot_url, jds[k], str(k))
             prepare_NAMD_config(i)
             os.system(gsiscp NPT.conf #to corresponding machine/dir)
             sjs[i].submit_job(bjs[i].pilot_url, jds[i], str(i))
             count=count+1

def prepare_NAMD_config(r):
# config prep when re-launching replicas   
   ifile = open("NPT.conf")   # should be changed if a different name is going to be used
   lines = ifile.readlines()
   for line in lines:
      if line.find("desired_temp") >= 0 and line.find("set") >= 0:
         lines[lines.index(line)] = "set desired_temp %s \n"%(str(temps[r]))
   ifile.close()
   ofile = open("NPT.conf","w")
   for line in lines:
     ofile.write(line)
   ofile.close()

def NAMD_config():
#initial prep of config,for the first launch of replicas
  ifile = open("NPT.conf")   # should be changed if a different name is going to be used
  lines = ifile.readlines()
  for line in lines:
     if line.find("desired_temp") >= 0 and line.find("set") >= 0:
      # if(i==0):
        lines[lines.index(line)] = "set desired_temp %s \n"%(str(temps[i]))
      # else:
       #   lines[lines.index(line)] = "set desired_temp %s \n"%(str(temps[i]))
  ifile.close()
  ofile = open("NPT.conf","w")
  for line in lines:
    ofile.write(line)
  ofile.close()


""" Test Job Submission via Advert """
if __name__ == "__main__":

    #range of temperatures
    temps=[]
    t=300
    for i in range(0,NUMBER_REPLICAS):
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
    for i in range(0,NUMBER_BIGJOBS):
      bj = bigjob.bigjob(advert_host)
      bjs.append(bj)
      if(i==0):
        lrms_url = "gram://" + HOST + "/jobmanager-pbs" 
      else:
        lrms_url = "gram://" REMOTE1 + "/jobmanager-pbs"
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
    for i in range(0, NUMBER_REPLICAS):
    #  jd.arguments = ["NPT" + str(i) + ".conf"]
      jd.output = "stdout-" + str(i) + ".txt"
      jd.error = "stderr-" + str(i) + ".txt"  	
      jds.append(jd)
      sj = bigjob.subjob(advert_host)
      sjs.append(sj)
      #prepare config and scp other files to remote machine
      NAMD_config()
#should copy file to the corresponding machine, should create a system tying each replica to a machine 
      os.system("gsiscp NPT.conf qb1.loni.org:/work/athota1/new_bigjob/") 
      sjs[i].submit_job(bjs[i].pilot_url, jds[i],str(i))
    
    find_partners_exchange() 

# Cleanup - stop BigJob
    bjs[0].cancel()
    bjs[1].cancel()
