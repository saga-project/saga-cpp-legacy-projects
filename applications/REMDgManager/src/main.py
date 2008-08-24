""" The main script for REMDgManager that are able to run fault-recovery REMD via SAGA-CPR/Migol

Usage : (For Test_RE)   python main.py  
                     or python main.py --type=Test_RE
        (For REMD) python main.py --type=REMD --configfile=remd_manager.config

jhkim at cct dot lsu dot edu

(Note on the current stage)
1. Currently, the following simple scenario is assumed.
a. each replica is submitted to each local scheduler via CPR/Migol  (will be changed with the major revision)
b. There is a wrapper in HPC to register checkpoint files to migol (all file are registered as checkpoint files)


TODO:
    - encapsulate data und method into 1 central REMD-Manager class 

"""

import sys, os, os.path, random, time
import optparse
import logging
import saga
import re
import pdb
import math
import threading
import traceback
import advert_job

""" Config parameters (will be moved to config file in the future) """
CPR = False
SCP = False

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
        self.numberofprocesses = '1'
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

        # advert host
        self.advert_host ="localhost"
        # map <host, glidin-job>
        self.advert_job = None

#####################################
#  REMD specific functions
########################################################
def create_new_namd_conf_file(RE_info, irep):
    # make new conf file here (JK) will be implemented
    # assume namd conf file ending with ".conf"
    
    
    
    return None
    
def get_energy(replica_ID, RE_info):
    # not implemented yet. it is dummy.
    
    en = 0.4
    
    return en


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
    
    does_exchange = "YES"    # at the moment always
    
    return does_exchange


#####################################
#  Elementary Functions
########################################################

""" create job description for respective replica """
def set_saga_job_description(replica_ID, RE_info):
    
    if CPR==True:
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

""" state files with SAGA/File & GridFTP """
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

""" state files with SCP
    This function assumes that public/key authentication for SCP is enabled """
def file_stage_in_with_scp(input_file_list_with_path, remote_machine_ip, remote_dir):
    #pdb.set_trace()    
    for ifile in input_file_list_with_path:
        print "stage file: " + ifile
        dest_url_str = remote_machine_ip + ":"
    
        ifile_basename = os.path.basename(ifile)
        if not os.path.isfile(ifile):
            error_msg = "Input file %s does not exist in %s"%(ifile_basename, os.path.dirname(ifile))
            logging.error(error_msg)
        else:
            try:
                source_url_str = ifile
                dest_url_str = dest_url_str + os.path.join(remote_dir, ifile_basename)
                command = "scp " + source_url_str + " " + dest_url_str
                print "Execute: " + command
                os.popen(command)
                logging.info("Now Input file %s is staged into %s"%(ifile_basename,dest_url_str))
            except saga.exception, e:
                error_msg = "Input file %s failed to be staged in"%(ifile_basename)
                logging.error(error_msg)
                
    return None    

""" submit job via job api"""
def submit_job(dest_url_string, jd):
    error_string = ""
    js = saga.job.service(saga.url(dest_url_string))
    new_job = js.create_job(jd)
    new_job.run()
    return error_string, new_job

""" submit job via cpr api"""
def submit_job_cpr(dest_url_string, jd):
    error_string = ""
    start = time.time()
    js = saga.cpr.service(saga.url(dest_url_string))
    jd_start = jd
    jd_restart = jd
    new_cpr_job = js.create_job(jd_start, jd_restart)
    new_cpr_job.run()
    print "job state: " + str(new_cpr_job.get_state());
    print "spawning time " + "%d"%(time.time()-start) +" s"    
    return error_string, new_cpr_job



#########################################################
#  Initialize
#########################################################
def initialize(config_filename):
    
    RE_info = RE_INFO()
    
    conf_file = open(config_filename)
    lines = conf_file.readlines()
    conf_file.close()
    # REMD config file should have the following format. In brief, : is needed between the keyword and variable(s) where 
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
            elif key == "advert_host" :
                RE_info.advert_host = value[0] 
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
    if(SCP==True):
        file_stage_in_with_scp(RE_info.stage_in_files, remote_machine_ip, remote_dir)
    else:
        file_stage_in_with_saga(RE_info.stage_in_files, remote_machine_ip, remote_dir) 
   
    start_file_transfer=time.time()
    file_stage_in_with_saga(RE_info.stage_in_files, remote_machine_ip, remote_dir) 
    print "Time for staging " + "%d"%len(RE_info.stage_in_files) + " files: " + str(time.time()-start_file_transfer) + " s"

def start_job(RE_info, irep):
    """ start job:
        if glidin job has been started via advert job start protocol
        otherwise with SAGA CPR/Job """
    jd = set_saga_job_description(irep, RE_info)
    host = RE_info.remote_hosts[irep]
    scheduler = RE_info.remote_host_local_schedulers[irep]
    print "start job at: " + host   
    
    if (RE_info.advert_job.glidin_jobs.has_key(host)): # start via advert service
        dest_url_string = "advert://" + host + "/" + "jobmanager-" + scheduler  
        error, new_job =  RE_info.advert_job.submit_job(dest_url_string, jd)    
    else: # normal SAGA CPR/Job start
        if (CPR==True):
            dest_url_string = "migol://" + host + "/" + "jobmanager-" + scheduler     # just for the time being
            error, new_job = submit_job_cpr(dest_url_string, jd)
        else:
            dest_url_string = "gram://" + host + "/" + "jobmanager-" + scheduler    # just for the time being
            error, new_job = submit_job(dest_url_string, jd)
    RE_info.replica.insert(irep,new_job)
    print "Replica " + "%d"%irep + " started." 
    
      
def start_glidin_jobs(RE_info):
    """start glidin jobs (advert_job.py) at every unique machine specified in RE_info"""  
    unique_hosts = set(RE_info.remote_hosts)    
    if RE_info.advert_job == None:
        RE_info.advert_job = advert_job.advert_job(RE_info.advert_host)
    for i in unique_hosts:
        nodes = RE_info.remote_hosts.count(i) 
        lrms = RE_info.remote_host_local_schedulers[RE_info.remote_hosts.index(i)]
        project = RE_info.projects[RE_info.remote_hosts.index(i)]
        queue = RE_info.queues[RE_info.remote_hosts.index(i)]
        workingdirectory = RE_info.workingdirectories[RE_info.remote_hosts.index(i)]
        if(CPR==True):
            lrms_url = "migol://"    
        else:
            lrms_url = "gram://"
        lrms_url = lrms_url + i + "/" + "jobmanager-" + lrms      
        print "Glidin URL: " + lrms_url    
        print "hosts: " + i + " number of replica_processes: " + "%d"%nodes
        print "Project: " + project + " Queue: " + queue + " Working Dir: " +workingdirectory
        
        # start job
        RE_info.advert_job.start_glidin_job(lrms_url, 
                                            nodes,
                                            queue,
                                            project,
                                            workingdirectory)
        
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
    start_glidin_jobs(RE_info)


    #file transfer
    start_file_transfer=time.time()
    transfer_thread_list = []
    for irep in range(0, numReplica):
       transfer_thread=threading.Thread(target=transfer_files(RE_info, irep))
       transfer_thread.start()
       transfer_thread_list.append(transfer_thread)
       
    for ithread in range(0, len(transfer_thread_list)):
       transfer_thread_list[ithread].join()
                               
    print "Time for staging " + "%d"%len(RE_info.stage_in_files) + " files to " \
     + "%d"%(numReplica) + " replicas: " + str(time.time()-start_file_transfer) + " s"

    # job submit   
    RE_info.replica = []
    
    start_time = time.time()
    for irep in range(0,numReplica):
        start_job(RE_info, irep)
    end_time = time.time()        
    print "Time for spawning " + "%d"%(irep+1) + " replica: " + str(end_time-start_time) + " s"
    
    namd_conf_filename = os.getcwd() + "/" + "NPT.conf"        # assumed this argument is the configuration file name

    # determine the total number of required exchanges  
    total_num_exchanges = numReplica * numEX
    num_exchanges = 0
    while num_exchanges < total_num_exchanges: 
        #polling pairwise neighbors for their state
        irep=0
        while irep < numReplica-1:
            print "poll state replica " + "%d"%irep + " and " + "%d"%(irep+1) 
	    try:
            	job1 = RE_info.replica[irep]
            	state1 = job1.get_state()
            	job2 = RE_info.replica[irep+1]
            	state2 = job2.get_state()
	    except:
                traceback.print_exc(file=sys.stdout) 
            print "state replica job " + "%d"%irep + ": " + str(state1) +", replica job "+ "%d"%(irep+1)+": " + str(state2)
            # check whether both replicas are done
            if (str(state1) == "Done" and str(state2)=="Done"):
		#pdb.set_trace()
                print "Replica " + "%d"%(irep) + " and " + "%d"%(irep + 1 ) + " are done."
                # get energy from results of run and
                # exchange parameter
                does_exchange = exchange_replicas(RE_info, irep, irep+1)
                
                if does_exchange in ("YES"):
                # restart
     #           transfer_files(RE_info, irep)
                    remote_machine_ip = RE_info.remote_hosts[irep]
                    remote_dir = RE_info.workingdirectories[irep]
                
                    create_new_namd_conf_file(RE_info, irep)
                    file_stage_in_with_saga([namd_conf_filename], remote_machine_ip, remote_dir)
                    start_job(RE_info, irep)
     #           transfer_files(RE_info, irep+1)
                    remote_machine_ip = RE_info.remote_hosts[irep+1]
                    remote_dir = RE_info.workingdirectories[irep+1]
                
                    create_new_namd_conf_file(RE_info, irep+1)
                    file_stage_in_with_saga([namd_conf_filename], remote_machine_ip, remote_dir)
                    start_job(RE_info, irep+1)
                    num_exchanges = num_exchanges + 2 # 2 processes exchanged their replicas
                    print "Restarted Replica " + "%d"%(irep) + " and " + "%d"%(irep + 1 ) + ", Number Exchanges: " + "%d"%num_exchanges                 
                else:
                    print "Replica " + "%d"%(irep) + " and " + "%d"%(irep + 1 ) + " are not changed, Number Exchanges: " + "%d"%num_exchanges
            irep = irep + 2
        time.sleep(10)
 

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
    js=None
    if(CPR==True):
        js = saga.cpr.service()
    
    # AL: I disabled this option temporarly since it is not working
    #     I also added a usage message
    #if options.type in (None,"test_RE"):
        #run_test_RE(options.numreplica,20)   #sample test for Replica Exchange with localhost
    if options.type != None and options.type in ("REMD"):
        run_REMDg(options.configfile) 
    else:
        print "Usage : \n python " + sys.argv[0] + " --type=<REMD> --configfile=<configfile> \n"
        print "Example: \n python " + sys.argv[0] + " --type=REMD --configfile=remd_manager.config"
        sys.exit(1)      
        
    print "REMDgManager Total Runtime: " + str(time.time()-start) + " s"
    
