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
    nodes = 4 # number nodes for agent
    workingdirectory=os.getcwd() +"/agent"  # working directory for agent
    userproxy = None # userproxy (not supported yet due to context issue w/ SAGA)

    # start pilot job (bigjob_agent)
 #   print "Start Pilot Job/BigJob: " + bigjob_agent + " at: " + lrms_url
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
    jd.executable = "/bin/date"
    jd.number_of_processes = "1"
    jd.spmd_variation = "single"
    jd.arguments = [""]
    jd.working_directory = os.getcwd() 
   # jd.output = "stdout-" + str(i) + ".txt"
   # jd.error = "stderr-" + str(i) + ".txt"
    sjs=[]
    for i in range(0, 2):
        jd.output = "stdout-" + str(i) + ".txt"
    	jd.error = "stderr-" + str(i) + ".txt"
        	
	jds.append(jd)
   	sj = bigjob.subjob(advert_host)
   	sjs.append(sj)

	sjs[i].submit_job(bjs[i].pilot_url, jds[i])
  
    # busy wait for completion
    states = []
    state = str(sjs[i].get_state())
   # states.append(state)
    i = 0
    states.append(state)
    while 1:
      states[0] = str(sjs[0].get_state())
      print "state: " + states[0] + str(0)
      states[1] = str(sjs[1].get_state())
      if(states[0]=="Failed" or states[0]=="Done") and (states[1]=="Failed" or states[1]=="Done"):
         break
	 time.sleep(10)
    ##########################################################################################
    # Cleanup - stop BigJob
    bjs[0].cancel()
    bjs[1].cancel()

