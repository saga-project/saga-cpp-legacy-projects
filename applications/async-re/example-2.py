""" Example application demonstrating job submission via bigjob 
    advert_job implementation of BigJob is used
"""

import saga
import os
import bigjob
import time
import pdb

advert_host = "fortytwo.cct.lsu.edu"

""" Test Job Submission via Advert """
if __name__ == "__main__":

    ##########################################################################################
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
        lrms_url = "gram://eric1.loni.org/jobmanager-pbs"
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
      jd.output = "rep-" + str(i) + "/stdout-" + str(i) + ".txt"
      jd.error = "rep-" + str(i) + "/stderr-" + str(i) + ".txt"  	
      jds.append(jd)
      sj = bigjob.subjob(advert_host)
      sjs.append(sj)
      sjs[i].submit_job(bjs[i].pilot_url, jds[i],str(i))
    
    # busy wait for completion
    while 1:
      state0 = str(sjs[0].get_state())
      energy0 = str(sjs[0].get_energy())
      print "energy0: " + energy0
      print "state0: " + state0 + str(0)
      state1 = str(sjs[1].get_state())
      energy1 = str(sjs[1].get_energy())
      print "state1: " + state1 + str(1)
      print "energy1: " + energy1
      if(state0=="Failed" or state0=="Done") and (state1=="Failed" or state1=="Done"):
         break
      time.sleep(10)
    ##########################################################################################
    
    def prepare_NAMD_config():
# The idea behind this is that we can simply modify NPT.conf before submit a job to set temp and other variables
        ifile = open("NPT.conf")   # should be changed if a different name is going to be used
        lines = ifile.readlines()
        for line in lines:
            if line.find("desired_temp") >= 0 and line.find("set") >= 0:
               # items = line.split()
               # temp = items[2]
                if(sjs[i]==sjs[0]):
                 #   print "\n (DEBUG) temperature is changing to " + str(self.temperatures[replica_id]) + " from " + temp + " for rep" + str(replica_id)
                    lines[lines.index(line)] = "set desired_temp %s \n"%(str(energy1))
                else:
                    lines[lines.index(line)] = "set desired_temp %s \n"%(str(energy0))    
        ifile.close()
        ofile = open("NPT.conf","w")
        for line in lines:
            ofile.write(line)
        ofile.close()
    
    for i in range(0,2):
      prepare_NAMD_config()
      sjs[i].submit_job(bjs[i].pilot_url, jds[i], str(i))
    
    while 1:
      state0 = str(sjs[0].get_state())
      energy0 = str(sjs[0].get_energy())
      print "energy0: " + energy0
      print "state0: " + state0 + str(0)
      state1 = str(sjs[1].get_state())
      energy1 = str(sjs[1].get_energy())
      print "state1: " + state1 + str(1)
      print "energy1: " + energy1
      if(state0=="Failed" or state0=="Done") and (state1=="Failed" or state1=="Done"):
         break
      time.sleep(10)

# Cleanup - stop BigJob
    bjs[0].cancel()
    bjs[1].cancel()

