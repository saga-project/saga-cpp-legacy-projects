""" The main script for REMDgManager that are able to run fault-recovery REMD via SAGA-CPR/Migol

Usage : (For Test_RE)   python main.py  
                     or python main.py --type=Test_RE
        (For REMD) python main.py --type=REMD --configfile=remd_manager.config

jhkim at cct dot lsu dot edu

(Note on the current stage)
1. Currently, the following simple scenario is assumed.
a. each replica is submitted to each local scheduler via CPR/Migol  (will be changed with the major revision)
b. There is a wrapper in HPC to register checkpoint files to migol (all file are registered as checkpoint files)
"""

import sys, os, os.path, random, time
import optparse
import logging
import saga
import re
import pdb
import math

JOB_STATE_UNKNOWN = -1
JOB_STATE_NEW=1
JOB_STATE_RUNNING=2
JOB_STATE_DONE=3
JOB_STATE_CANCELED=4
JOB_STATE_FAILED=5
JOB_STATE_SUSPENDED=6

########################################################
#  Global variable 
########################################################
class RE_INFO (object):
    """ 
    This class holds information about the application and replicas remotely running via SAGA-CPR/MIGOL
    
    """
    
    def __init__(self):
        # general info
        self.app_name = "NAMD"
        self.stage_in_files = []
        self.numberofprocesses = '0'
        self.exchange_count = 0
        self.totalcputime = '40'
        self.arguments = []
        
        # lists for variables of each replica (Note that these variable should have n variables where n is self.replica_count
        self.replica_count = 0
        self.remote_hosts = []
        self.remote_host_local_schedulers = []
        self.projects = []
        self.queues = []
        self.workingdirectories = []
        self.executables = []
        self.temperatures = []
        
        # instant variable for replica exchange
        self.replica_saga_jobs = []   # saga jobs
        self.istep = 0
        self.iEX = 0
        
        
#####################################
#  Elementary Functions
########################################################

def set_saga_job_description(replica_ID, RE_info, iflag):
    
    if iflag=="cpr":
        jd = saga.cpr.description()
    else :    
        jd = saga.job.description()
    
    jd.numberofprocesses = RE_info.numberofprocesses
    jd.spmdvariation = "single"
    #jd.totalcputime = RE_info.totalcputime
    jd.arguments = RE_info.arguments   
    
    jd.executable = RE_info.executables[replica_ID]
    jd.queue = RE_info.projects[replica_ID] + "@" + RE_info.queues[replica_ID]
    jd.workingdirectory = RE_info.workingdirectories[replica_ID]
    
    jd.output = "output.txt"    #this is requried for Migol
    jd.error = "error.txt"
    
    return jd


def file_stage_in_with_saga(input_file_list_with_path, remote_machine_ip, remote_dir):
    #pdb.set_trace()    
    for ifile in input_file_list_with_path:
	print "stage file: " + ifile
        if remote_machine_ip.find('localhost') >= 0:
            dest_url_str = 'file://'
        else:
            dest_url_str = 'gridftp://'+remote_machine_ip + "/"
        source_url_str = 'file://'

        ifile_basename = os.path.basename(ifile)
        if not os.path.isfile(ifile):
            error_msg = "Input file %s does not exist in %s"%(ifile_basename, os.path.dirname(ifile))
            logging.error(error_msg)
        else:
                
            try:
                source_url_str = source_url_str+ifile
                dest_url_str = dest_url_str + os.path.join(remote_dir, ifile_basename)
                source_url = saga.url(source_url_str)
                dest_url = saga.url(dest_url_str)

                sagafile = saga.file.file(source_url)
                sagafile.copy(dest_url)
                logging.info("Now Input file %s is staged into %s"%(ifile_basename,dest_url_str))
            except saga.exception, e:
                error_msg = "Input file %s failed to be staged in"%(ifile_basename)
                logging.error(error_msg)
                
    return None
    
    
def submit_job(dest_url_string, jd):
    error_string = ""
    
    js = saga.job.service(saga.url(dest_url_string))

    new_job = js.create_job(jd)
    
    new_job.run()
        
    return error_string, new_job

def submit_job_cpr(dest_url_string, jd, checkpt_files):
    error_string = ""
    start = time.time()
    js = saga.cpr.service(saga.url(dest_url_string))
    jd_start = jd
    jd_restart = jd
    
    #here checkpoint files are registered (not done yet)
#    check_point = saga.cpr.checkpoint("REMD_MANAGER_CHECKPOINT")
    
#    for ifile in checkpt_files:
#        check_point.add_files(ifile)
    
    new_cpr_job = js.create_job(jd_start, jd_restart)
    new_cpr_job.run()
    print "job state: " + str(new_cpr_job.get_state());
    print "spawning time " + "%d"%(time.time()-start) +" s"    
    return error_string, new_cpr_job


def get_energy(replica_ID, RE_info):
    # not implemented yet. it is dummy.
    
    en = 0.4
    
    return en

#########################################################
#  Initialize
#########################################################
def initialize(config_filename):
    
    RE_info = RE_INFO()
    
    conf_file = open(config_filename)
    lines = conf_file.readlines()
    conf_file.close()
    # config file should have the following format. In brief, : is needed between the keyword and variable(s) where 
    # multi variables are separated by spaces 
    # exeutables :  /usr/local/namd  /usr/local/namd 
    # arguments : NPT.conf 
    
    for line in lines:
        items = line.split()
        if line.find(':'):
            key = items[0]
            value = items[2:]
            
            # list of variables for each replica
            if key == 'remote_host':
                for ihost in value:
                    RE_info.remote_hosts.append(ihost)
            
            elif key == 'remote_host_local_scheduler':
                for isched in value:
                    RE_info.remote_host_local_schedulers.append(isched)
            
            elif key == 'workingdirectory':   # this is the list of working directories for each replica in order with remote_hosts
                for idir in value:
                    RE_info.workingdirectories.append(idir)
            
            elif key == 'executable':
                for ival in value:
                    RE_info.executables.append(ival)
                
            elif key == 'queue':
                for ival in value:
                    RE_info.queues.append(ival)
    
            elif key == 'project':     
                for ival in value:
                    RE_info.projects.append(ival)      
 
                    # variables common to all replicas
            elif key == 'arguments':
                # support quoted arguments like this:
                # "/usr/local/packages/namd-2.6-mvapich-1.0-intel10.1/namd2 NPT.conf" "mpi"
                args = line.split(":")[1]
                p = re.compile(r'\" \"|\"')
                args_parts = p.split(args)
                for ival in args_parts:
                    if (ival.strip() !=""):
                        RE_info.arguments.append(ival.strip())  
                #for ival in value:
                #    RE_info.arguments.append(ival)      
 
            elif key == 'totalcputime':
                RE_info.totalcputime = value[0]    
                
            elif key == 'numberofprocesses':
                RE_info.numberofprocesses = value[0] 
            

            elif key == 'replica_count':
                RE_info.replica_count = eval(value[0])
                
            elif key == 'exchange_count' :
                RE_info.exchange_count = eval(value[0])    
                
            elif key == "stage_in_file" :
                cwd = os.getcwd()
                for ifile in value:
                    ifile_with_path = cwd + "/" + ifile    # of course unix machine
                    RE_info.stage_in_files.append(ifile_with_path)      
            elif key == "temperature" :
                for itemp in value:
                    RE_info.temperatures.append(eval(itemp))        
            
            else :
                logging.info("this line %s in %s does not have configure variables"%(line, config_filename))    
            
                
        else :
            logging.info("this line %s in %s does not have configure variables"%(line, config_filename))
                         
    random.seed(time.time()/10.)
    
    return RE_info


# transfer files
def transfer_files(RE_info, irep):
    remote_machine_ip = RE_info.remote_hosts[irep]
    remote_dir = RE_info.workingdirectories[irep]
           #pdb.set_trace()
    file_stage_in_with_saga(RE_info.stage_in_files, remote_machine_ip, remote_dir) 

# start job
def start_job(RE_info, irep):
    jd = set_saga_job_description(irep, RE_info, "cpr")
    #dest_url_string = "migol://" + RE_info.remote_hosts[irep] + "/" + "jobmanager-" + RE_info.remote_host_local_schedulers[irep]     # just for the time being
    dest_url_string = "gram://" + RE_info.remote_hosts[irep] + "/" + "jobmanager-" + RE_info.remote_host_local_schedulers[irep]     # just for the time being
    checkpt_files = []     # will be done by migol not here  (JK  08/05/08)
    #error, new_job = submit_job_cpr(dest_url_string, jd, checkpt_files)
    error, new_job = submit_job(dest_url_string, jd)
    RE_info.replica.append(new_job)
    print "Replica " + "%d"%irep + " started." 
    
# swap parameters for new runs
# mockup at the moment    
def exchange_replicas(RE_info, irep1, irep2):
    en_a = get_energy(irep1, RE_info)
    en_b = get_energy(irep2, RE_info)
    print "Replica Exchange - Temperature of replica " + "%d"%irep1 + ": " + "%d"%RE_info.temperatures[irep1]
    if math.exp(-en_a/RE_info.temperatures[irep1] + en_b/RE_info.temperatures[irep2]) > random.random() :
                tmpNum = RE_info.temperatures[irep2]
                RE_info.temperatures[irep2] = RE_info.temperatures[irep1]
                RE_info.temperatures[irep1] = tmpNum
    else :
         pass    
    
#########################################################
#  run_REMDg
#########################################################
def run_REMDg(configfile_name):

    # initialize of all replica and its related variables
    RE_info = initialize(configfile_name)
    
    numEX = RE_info.exchange_count    
    numReplica = RE_info.replica_count
    
    ofilenamestring = "replica-temp-%d.out"
    ofilenamelist = [ofilenamestring%i for i in range (0,numReplica)]

    #iEX = 0

    #file transfer
    start_file_transfer=time.time()
    for irep in range(0, numReplica):
       transfer_files(RE_info, irep)
    print "Time for staging " + "%d"%len(RE_info.stage_in_files) + " files to " \
     + "%d"%(numReplica) + " replicas: " + str(time.time()-start_file_transfer) + " s"

    # job submit   
    RE_info.replica = []
    
    start_time = time.time()
    for irep in range(0,numReplica):
        start_job(RE_info, irep)
    end_time = time.time()        
    print "Time for spawning " + "%d"%(irep+1) + " replica: " + str(end_time-start_time) + " s"
    
    # determine the total number of required exchanges  
    total_num_exchanges = numReplica * numEX
    num_exchanges = 0
    while num_exchanges < total_num_exchanges: 
        #polling pairwise neighbors for their state
        irep=0
        while irep < numReplica-1:
            job1 = RE_info.replica[irep]
            state1 = job1.get_state()
            job2 = RE_info.replica[irep+1]
            state2 = job1.get_state()
            # check whether both replicas are done
            if (str(state1) == "Done" and str(state2)=="Done"):
                print "Replica " + "%d"%(irep) + " and " + "%d"%(irep + 1 ) + " are done."
                # get energy from results of run and
                # exchange parameter
                exchange_replicas(RE_info, irep, irep+1)
                # restart
                transfer_files(RE_info, irep)
                start_job(RE_info, irep)
                transfer_files(RE_info, irep+1)
                start_job(RE_info, irep+1)
                num_exchanges = num_exchanges + 2 # 2 processes exchanged their replicas
                print "Restarted Replica " + "%d"%(irep) + " and " + "%d"%(irep + 1 ) + "."                
            irep = irep + 2
 

#########################################################
#  run_test_RE
#########################################################    
def run_test_RE(nReplica, nRand):
    
    # initialization
    app_name = "test_app.py"
    numReplica = nReplica
    numRand = nRand        
    
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
    
        iEX = iEX + 1
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
    start = time.time()
    op = optparse.OptionParser()
    op.add_option('--type','-t')
    op.add_option('--configfile','-c')
    op.add_option('--numreplica','-n',default='2')
    options, arguments = op.parse_args()

    # enable monitoring through Migol
    #js = saga.cpr.service()
    
    if options.type in (None,"test_RE"):
        run_test_RE(options.numreplica,20)   #sample test for Replica Exchange with localhost
    elif options.type in ("REMD"):
        run_REMDg(options.configfile) 
    print "REMDgManager Total Runtime: " + str(time.time()-start) + " s"
    
