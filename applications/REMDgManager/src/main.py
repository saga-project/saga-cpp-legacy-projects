""" The main script for REMDgManager that are able to run fault-recovery via SAGA-CPR/Migol

Usage : (For Test_RE)   python main.py  
                     or python main.py --type=Test_RE
        (For REMD) not yet

jhkim at cct dot lsu dot edu
"""

import sys, os
import optparse
import logging
import saga


########################################################
#  Global variable 
########################################################
class ApplicationManagerInfo (object):
    """ 
    This calss holds information about application manager environment and jobs
    
    """
    am_type = ""
    am_package_name = ""
    
    def __init__(self, am_type, am_package_name):
        self.jobdescription = {'executable':"", \
                      'arguments':[], \
                      'environment':[],\
                      'workingdirectory':"" ,\
                      'interactive':"" ,\
                      'input':"" , \
                      'output':"" , \
                      'error':"" , \
                      'filetransfer':"" , \
                      'cleanup':"" , \
                      'jobstarttime':"" , \
                      'totalcputime':"" , \
                      'totalphysicalmemory':"" , \
                      'cpuarchitecture':"" , \
                      'operatingsystemtype':"" , \
                      'candidaetehosts' : []  , \
                      'queue':"" , \
                      'jobcontact':"" , \
                      'spmdvariation':"", \
                      'totalcpucount':"" , \
                      'numberofprocesses':"", \
                      'processesperhost':"", \
                      'threadsperprocess':""    # up to this line from python-binding
                      }
        
        self.am_type = am_type                  
        self.am_package_name = am_package_name
#####################################
#  Elementary Functions
########################################################


def file_stagein(localfile_url_string, remotefile_url_string):
    error_string = ""    

    ifile = saga.file(saga.url(localfile_url_string))
    
    try:
        ifile.copy(saga.url(remotefile_url_string))
    except saga_exception, err :
        error_string = "SAGA exception when " + localfile_url_string + " is sent to " + remotefile_url_string + " with " + str(err)
    
    return error_string

def file_stageout(remotefile_url_string, localfile_url_string):
    error_string = ""
    
    ofile = saga.file(saga.url(remotefile_url_string))
    
    try:
        ifile.copy(saga.url(localfile_url_string))
    except saga_exception, err :
        error_string = "SAGA exception when " + remote_url_string + " is moved into " + localfile_url_string + " with " + str(err)
    
    return error_string   

def set_saga_job_description(saga_jd, am):
    
    
    
    
    
    return saga_jd
    
    
def submit_job(target_url_string, AM):
    error_string = ""
    
    js = saga.job.service(target_url_string)
    jd = saga.job.description()
    jd = set_sagajob_description(jd, AM)
    new_job = js.create_job(jd)
    
    new_job.run()
        
    return error_string, new_job

def submit_job_cpr(target_url_string, AM):
    error_string = ""
    
    js = saga.job.service(target_url_string)
    jd_start = saga.job.description()
    jd_start = set_sagajob_description(jd, AM)
    jd_restart = saga.job.description()
    jd_restart = set_sagajob_description(jd, AM)
    
    #here checkpoint files are registered
    check_point = saga.cpr.checkpoint(target_url_string)
    check_point = add_files_cpr(check_point. AM)
    
    new_cpr_job = js.create_job(jd_start, jd_restart)
    
    new_cpr_job.run()
        
    return error_string, new_cpr_job

def get_job_state(launched_job):
    
    lj_state = str(launched_job.get_state())
    
    return lj_state


#########################################################
#  Initialize
#########################################################
def initialize():
    
    return error_string

#########################################################
#  proceed_nstep
#########################################################
def proceed_nstep():
    
    return error_string


#########################################################
#  gather_checkFault_replicaEx
#########################################################
def gather_checkFault_replicaEx():
    
    return error_string



#########################################################
#  run_REMDg
#########################################################
def run_REMDg(arguments):
    AM = ApplicationManagerInfo("REMD","NAMD")
    intialize()    

#########################################################
#  run_test_RE
#########################################################    
def run_test_RE(nReplica, nRand):
    
    # initialization
    app_name = "test_app.py"
    numReplica = nReplica
    numRand = nRand        
    AM = ApplicationManagerInfo("TEST_RE",app_name)
    curr_working_dir = os.getcwd()
    ofilenamestring = "test-%d.out"
    ofilenamelist = [ofilenamestring%i for i in range (0,numReplica)]
    replicaListing = [i for i in range(1, numReplica+1)]
    numEX = 5
    iEX = 0
    
    
    while 1:
        # job scheduling and job submission step
        job_list = []
        for ireplica in range(0,numReplica):
            js = saga.job.service(saga.url("fork://localhost%s"%curr_working_dir))
            jd = saga.job.description()
            jd.executable = app_name
            jd.arguments=[str(replicaListing[ireplica]),ofilenamelist[ireplica], str(numRand)]
            new_job = js.create_job(jd)
            job_list.append(new_job)
            new_job.run()
            print "Replica " + "%d"%replicaListing[ireplica] + " started (Run = %d)"%(iEX+1)

        # job monitoring step
        rnumber = [0 for i in range(0, numReplica)]
        flagJobDone = [ False for i in range(0, numReplica)]
        numJobDone = 0
        while 1:    
            for ireplica in range(0, numReplica):
                running_job = job_list[ireplica]
                state = running_job.get_state()
                if (str(state) == "Done") and (flagJobDone[ireplica] is False) :   
                    print "Replica " + "%d"%(ireplica+1) + " done"
                    ifile = open(ofilenamelist[ireplica])
                    lines = ifile.readlines()
                    ifile.close()
                    lastline = lines[-1]
                    
                    items = lastline.split()
                    rnumber[ireplica] = eval(items[1])
                    flagJobDone[ireplica] = True
                    numJobDone = numJobDone + 1
                else :
                    pass
            
            if numJobDone == numReplica:
                break
            
        # replica exchange step        
        for ireplica in range(0, numReplica-1):
            ran_a = rnumber[ireplica]
            ran_b = rnumber[ireplica+1]
            if ran_a < ran_b :
                tmpNum = replicaListing[ireplica+1]
                replicaListing[ireplica+1] = replicaListing[ireplica]
                replicaListing[ireplica] = tmpNum
            else :
                pass
    
        iEX = iEX +1
        output_str = "%5d-th EX :"%iEX
        for ireplica in range(0, numReplica):
            output_str = output_str + "  %3d"%replicaListing[ireplica]
        
        print "\n\nExchange result : "
        print output_str + "\n\n"
                    
        if iEX == numEX:
            break

    
#########################################################
#  main
#########################################################

if __name__ == "__main__" :

    op = optparse.OptionParser()
    op.add_option('--name','-n')
    op.add_option('--type','-t')
    options, arguments = op.parse_args()
    
    if options.type in (None,"test_RE", 5, 50):
        run_test_RE(5,20)   #sample test for Replica Exchange with localhost
    elif options.type in ("REMD"):
        run_REMDg(options.appname) 
    
    