#!/usr/bin/env python2.3
#
#  async-re.py
#  
#
#  Created by athota1 on 08/04/10.
#  Copyright (c) 2010 __MyCompanyName__. All rights reserved.
#

""" Example application demonstrating job submission via bigjob 
    advert_job implementation of BigJob is used
"""

import saga
import os
import bigjob
import time
import pdb

#Configure here:
BIGJOB_SIZE = 8
NUMBER_EXCHANGES = 3
NUMBER_BIGJOBS = 2
NUMBER_REPLICAS = 4
HOST = "eric1.loni.org"
REMOTE1 = "louie1.loni.org"
REMOTE2 = "oliver1.loni.org"
advert_host = "fortytwo.cct.lsu.edu"
#dirs for replicas
WORK_DIR = "/work/athota1/new_bigjob/bigjob/"
WALLTIME = 60

def copy_with_saga(i):
    source_url = saga.url('file://' + WORK_DIR + 'NPT-' + str(i) + '.conf')
    dest_url = saga.url('gridftp://' + REMOTE1 + WORK_DIR)

    sagafile = saga.filesystem.file(source_url)
    try:
        sagafile.copy(dest_url)
    except saga.exception, e:
        print "\n(ERROR) remote file copy from %s to %s failed"%(HOST, REMOTE1)

    return None
             
def prepare_NAMD_config(r, i):
# config prep when re-launching replicas   
   ifile = open("NPT-" + str(i) + ".conf")   # should be changed if a different name is going to be used
   lines = ifile.readlines()
   for line in lines:
      if line.find("desired_temp") >= 0 and line.find("set") >= 0:
         lines[lines.index(line)] = "set desired_temp %s \n"%(str(temps[r]))
         print "new temperatures being set, re-launching#" + str(i) + "whose new temp=" + str(temps[r])
   ifile.close()
   ofile = open("NPT-" + str(i) + ".conf","w")
   for line in lines:
     ofile.write(line)
   ofile.close()

def NAMD_config():
#initial prep of config,for the first launch of replicas
  ifile = open("NPT-" + str(i) + ".conf")   # should be changed if a different name is going to be used
  lines = ifile.readlines()
  for line in lines:
     if line.find("desired_temp") >= 0 and line.find("set") >= 0:
      # if(i==0):
        lines[lines.index(line)] = "set desired_temp %s \n"%(str(temps[i]))
        print "initial temperature has been set for replica id " + str(i)+ "with" + str(temps[i])
      # else:
       #   lines[lines.index(line)] = "set desired_temp %s \n"%(str(temps[i]))
  ifile.close()
  ofile = open("NPT-" + str(i) + ".conf","w")
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
    nodes = BIGJOB_SIZE # number nodes for agent
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
        lrms_url = "gram://" + REMOTE1 + "/jobmanager-pbs"
      bjs[i].start_pilot_job(lrms_url,
                            bigjob_agent,
                            nodes,
                            None,
                            None,
                            workingdirectory, 
                            userproxy,
                            WALLTIME)
      print "Start Pilot Job/BigJob: " + bigjob_agent + " at: " + lrms_url
      print "Pilot Job/BigJob URL: " + bjs[i].pilot_url + " State: " + str(bjs[i].get_state())

    ##########################################################################################
    # Submit SubJob through BigJob
    i=0
    jds=[]
    jd = saga.job.description()
    jd.executable = "namd2"
    jd.number_of_processes = "4"
    jd.spmd_variation = "mpi"
   # jd.arguments = ["NPT.conf"]
    jd.working_directory = os.getcwd() 
    sjs=[]
    for i in range(0, NUMBER_REPLICAS):
      os.system("cp NPT.conf NPT-" + str(i) + ".conf")
      jd.arguments = ["NPT-" + str(i) + ".conf"]
      jd.output = "stdout-" + str(i) + ".txt"
      jd.error = "stderr-" + str(i) + ".txt"  	
      jds.append(jd)
      sj = bigjob.subjob(advert_host)
      sjs.append(sj)
      #prepare config and scp other files to remote machine
      NAMD_config()
      if not i%2:
        j = 0   
        sjs[i].submit_job(bjs[j].pilot_url, jds[i],str(i))
      else: 
        j = 1
        #os.system("gsiscp NPT-" + str(i) + ".conf %s:%s"%(REMOTE1, WORK_DIR))
        copy_with_saga(i)
        sjs[i].submit_job(bjs[j].pilot_url, jds[i],str(i))
        
    count=0
    while (count < NUMBER_EXCHANGES):
      print "exchange count=" + str(count)
#################################################################################            
      i = 0
      state=[]
      energy=[]
      for i in range(0,NUMBER_REPLICAS):
       states = str(sjs[i].get_state())
       energies = str(sjs[i].get_energy())
       state.append(states)
       energy.append(energies)
       print "current state= " + str(state[i]) + " where: replica# is" +str(i) + ", also current energy: " + str(energy[i])
       time.sleep(2)
#################################################################################             
      for i in range(0, NUMBER_REPLICAS):
        if(state[i]=="Done"):
          j=i #exclude the replica itself when looking for partners
#############################################      
          f=0
          list=[]
          for f in range(0, NUMBER_REPLICAS):
            print "found a replica in Done state, looking for other replicas in Done state"
            if((state[f]=="Done") and (f!=j)):
              list.append(f)
              print str(f) + "-- replica is in Done state"
            elif(f==j): 
              print "checking the same replica"
            else:
              print "no other replicas in Done state"
#################################################################################              
          if len(list)!=0:
            print "possible replicas for exchange found"
            k = 0
            for k in list:
              if (float(energy[k]) < 1):
                print "replica chosen for exchange is" + str(k)
                print "replica for which selection was made" + str(i)
                print "assigning the new temepratures and re-starting the replicas"
                prepare_NAMD_config(k, i) 
                if not i%2:
                  j=0
                  sjs[i].submit_job(bjs[j].pilot_url, jds[i], str(i))
                else:
                  j=1                  
                  #os.system("gsiscp NPT-" + str(i) + ".conf %s:%s"%(REMOTE1, WORK_DIR))  
                  copy_with_saga(i)
                  sjs[i].submit_job(bjs[j].pilot_url, jds[i], str(i))
                                  
                prepare_NAMD_config(i, k)
                if not k%2:
                  j=0
                  sjs[k].submit_job(bjs[j].pilot_url, jds[k], str(k))
                else:
                  j=1
                  #os.system("gsiscp NPT-" + str(k) + ".conf %s:%s"%(REMOTE1, WORK_DIR))
                  copy_with_saga(k)
                  sjs[k].submit_job(bjs[j].pilot_url, jds[k], str(k))
                  count = count + 1
                break
              else:
                print str(len(list))+ " = length of list, compared replica not selected, comparing other replicas"
            break    
        else: 
          pass  
            
#################################################################################          
          
      print "count=" + str(count)

   # Cleanup - stop BigJob
    for i in range(0, NUMBER_BIGJOBS):
     bjs[i].cancel()

