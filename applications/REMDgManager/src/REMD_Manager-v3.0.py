""" The main script for REMDgManager (w/o migol) : REMDManager-v1.0.py 
2008/8/26

Usage : (For Test_RE)   python REMDManager-v1.0.py  
                     or python REMDManager-v1.0.py --type=Test_RE
        (For REMD) python REMDManager-v1.0.py --type=REMD --configfile=remd_manager.config

jhkim at cct dot lsu dot edu

(Note on the current stage)
1. Currently, the following simple scenario is assumed.
a. each replica is submitted to each local scheduler via CPR/Migol  (will be changed with the major revision) (again not for this version)
b. remote directories should exist at each HPC (see remd_manager.config)
c. There is a wrapper in HPC to register checkpoint files to migol (all file are registered as checkpoint files) (again not for this version)
d. This version does not have migol  part.
e. remd-temp.out will contain temperature trajectories of each replica 
f. NPT.cof is modified before being sent to HPC
g. "output.txt" is the output file name with which energy is pulled out for the replica exchange step
h. get_energy(), do_exchange(), prepare_NAMD_config() might be changed later since implementaion is not optimized yet
: energy search in output.txt is not efficient as the size of output.txt grows, prepare_NAMD_config() needs more for assigning other initial files 

"""

import sys
import os
import random
import time
import optparse
import logging
import saga
import re
import math
import threading
import traceback
import advert_job
import pdb
import ConfigParser

class ReManager():
    """ 
    This class holds information about the application and replicas remotely running via SAGA-CPR/MIGOL
    """    
    def __init__(self, config_filename):
        self.stage_in_files = []
        self.exchange_count = 0
        self.totalcputime = '40'
        self.arguments = []
        
        # lists for variables of each replica (Note that these variable should have n variables where n is self.replica_count
        self.replica_count = 0
        self.temperatures = []
        
        # instant variable for replica exchange
        self.replica_saga_jobs = []   # saga jobs
        self.istep = 0
        self.iEX = 0
        
        # advert host
        self.advert_host ="localhost"
        # map <host, glidin-job>
        self.advert_glidin_jobs = {}
        self.userproxy = []        
        self.number_glideins_per_host = 1
        self.read_config(config_filename)
        # init random seed
        random.seed(time.time()/10.)
        
    def init_cpr(self):
        # enable monitoring through Migol
        if(self.cpr==True):
            self.js = saga.cpr.service()
        
    def read_config(self, conf_file):
        # read config file
        config = ConfigParser.ConfigParser()
        print ("read configfile: " + conf_file)
        config.read(conf_file)
        # RE configuration
        default_dict = config.defaults()
        self.re_agent = default_dict["re_agent"]
        self.arguments = default_dict["arguments"]
        self.number_of_mpi_processes = config.getint("DEFAULT", "number_of_mpi_processes") 
        self.exchange_count = default_dict["exchange_count"]
        self.advert_host = default_dict["advert_host"]
        self.total_number_replica = default_dict["total_number_replica"]
        
        """ Config parameters (will be moved to config file in the future) """
        self.cpr  = self.re_agent = config.getboolean("DEFAULT", "cpr")
        self.scp  = self.re_agent = config.getboolean("DEFAULT", "scp")
        self.glide_in  = self.re_agent = config.getboolean("DEFAULT", "glide_in")
        self.adaptive_sampling  = self.re_agent = config.getboolean("DEFAULT", "adaptive_sampling") 
        
        self.temperatures = default_dict["temperature"].split()
        self.stage_in_file = default_dict["stage_in_file"].split()

        # Resources       
        self.resourceMap = {}
        for section in config.sections():
            print section
            optionMap = {}
            for option in config.options(section):
                print " ", option, "=", config.get(section, option)
                optionMap[option] = config.get(section, option)            
            self.resourceMap[section] = optionMap
        
        print str(self.resourceMap)
    
  
    #####################################
    #  Elementary Functions
    ########################################################
    def set_saga_job_description(self, machine, replica_id):
        
        if self.cpr == True and self.glide_in == False: #if glidein use normal jd
            jd = saga.cpr.description()
        else:    
            jd = saga.job.description()
            
        jd.spmdvariation = "mpi" # launch MPI directly
        jd.numberofprocesses = self.number_of_mpi_processes
        jd.arguments = self.arguments
        jd.executable = machine["executable"]
        jd.queue = machine["queue"] + "@" + machine["allocation"]
        jd.workingdirectory = machine["working_dir_root"] + "/" + replica_id
        jd.output = "output.txt"    #this is requried for Migol
        jd.error = "error.txt"
        return jd


    def file_stage_in_with_saga(self, input_file_list_with_path, remote_machine_ip, remote_dir):
#        userproxy=None
#        try: 
#            userproxy = RE_info.userproxy[RE_info.remote_hosts.index(remote_machine_ip)] 
#        except:
#            try:
#                userproxy = RE_info.userproxy[RE_info.gridftp_hosts.index(remote_machine_ip)]
#            except:
#                pass
#        if userproxy != None or userproxy=="":
#             os.environ["X509_USER_PROXY"]=userproxy
#             print "use proxy: " + userproxy
#        else:
#             print "use standard proxy"
        for ifile in input_file_list_with_path:
    
            if remote_machine_ip.find('localhost') >= 0:
                dest_url_str = 'file://'
            else:
                dest_url_str = 'gridftp://'+remote_machine_ip + "/"
            source_url_str = 'file://'
            print "stage file: " + ifile + " to " + dest_url_str
    
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
    def file_stage_in_with_scp(self, input_file_list_with_path, remote_machine_ip, remote_dir):
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
                    #os.popen(command)
                    logging.info("Now Input file %s is staged into %s"%(ifile_basename,dest_url_str))
                except saga.exception, e:
                    error_msg = "Input file %s failed to be staged in"%(ifile_basename)
                    logging.error(error_msg)
        
        return None
                
                    

    def file_stage_out_with_saga(self, file_list, local_dir, remote_machine_ip, remote_dir):
        for ifile in file_list:
            try:
                source_url_str = "gsiftp://"+remote_machine_ip + "/" + os.path.join(remote_dir, ifile)
                dest_url_str = "file://" + local_dir + "/" + ifile
    # for the time being, use globus-url-copy            
    #            source_url = saga.url(source_url_str)
    #            dest_url = saga.url(dest_url_str)
     
    #            sagafile = saga.file.file(source_url)
    #            sagafile.copy(dest_url)
                print "(DEBUG) Now I am pulling the output.txt file at %s to %s"%(source_url_str, dest_url_str)
                cmd = "globus-url-copy %s %s"%(source_url_str, dest_url_str) 
                os.system(cmd)    
            except saga.exception, e:
                error_msg = "File stage out failed: "+ source_url_str
    
        return None
    
    
    def submit_job(self, dest_url_string, jd):
        error_string = ""
        js = saga.job.service(saga.url(dest_url_string))
        new_job = js.create_job(jd)
        new_job.run()
        return error_string, new_job
    
    def submit_job_cpr(self, dest_url_string, jd, checkpt_files):
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


    def prepare_NAMD_config(self, irep):
        # The idea behind this is that we can simply modify NPT.conf before submit a job to set temp and other variables
        ifile = open("NPT.conf")   # should be changed if a different name is going to be used
        lines = ifile.readlines()
        for line in lines:
            if line.find("desired_temp") >= 0 and line.find("set") >= 0:
                items = line.split()
                temp = items[2]
                if eval(temp) != self.temperatures[irep]:
                    print "\n (DEBUG) temperature is changing to " + str(self.temperatures[irep]) + " from " + temp + " for rep" + str(irep)
                    lines[lines.index(line)] = "set desired_temp %s \n"%(str(self.temperatures[irep]))
        ifile.close() 
        ofile = open("NPT.conf","w")
        for line in lines:    
            ofile.write(line)
        ofile.close()
          


    def get_energy(self, machine, replica_id):
    #I know This is not the best one!  namd output is staged out and take the energy out from the file
     
        file_list = ["output.txt"]  
        local_dir = os.getcwd()
        remote_machine_ip = machine["host"]
        if machine.has_key("gridftp_url"):
           remote_machine_ip = machine["gridftp_url"]
        remote_dir = machine["working_dir_root"] + "/" + replica_id
    
        self.file_stage_out_with_saga(file_list, local_dir, remote_machine_ip, remote_dir)
       
        enfile = open("output.txt", "r")
        lines = enfile.readlines()
        for line in lines:
            items = line.split()
            if len(items) > 0:
                if items[0] in ("ENERGY:"):
                    en = items[11]  
        print "(DEBUG) energy : " + str(en) + " from replica " + str(replica_id) 
        
        return eval(en)

    def do_exchange(self, energy, irep, jrep):
        iflag = False
        en_a = energy[irep]
        en_b = energy[jrep]
        
        factor = 0.0019872  # from R = 1.9872 cal/mol
        delta = (1./self.temperatures[irep]/factor - 1./self.temperatures[irep+1]/factor)*(en_b-en_a)
        if delta < 0:
            iflag = True
        else :
            if math.exp(-delta) > random.random() :
                iflag = True
    
            tmpNum = self.temperatures[jrep]
            self.temperatures[jrep] = self.temperatures[irep]
            self.temperatures[irep] = tmpNum
    
        print "(DEBUG) delta = %f"%delta + " en_a = %f"%en_a + " from rep " + str(irep) + " en_b = %f"%en_b +" from rep " + str(jrep)


    def submit_job_advert(self, dest_url_string, jd):
        """ submit job via advert service and glidin job"""
        error_string = ""
        new_advert_job = advert_job.advert_job(self.advert_host)
        new_advert_job = new_advert_job.submit_job(dest_url_string, jd)
        return error_string, new_advert_job
      
    def start_glidin_jobs(self):
        """start glidin jobs (advert_job.py) at every unique machine specified in RE_info"""  
        for resource in self.resourceMap.keys():
            i = self.resourceMap[resource]
            host = i["host"]
            num_glidein = int(i["number_glide_in"])
            nodes = int(i["total_number_nodes"])
            lrms = i["scheduler"]
            project = i["allocation"]
            queue = i["queue"]
            workingdirectory = i["working_dirs"].split()[0]
            userproxy=None
            try:
                userproxy = i["userproxy"]
            except:
                pass
            
            if(self.cpr==True):
                lrms_url = "migol://"    
            else:
                lrms_url = "gram://"
                
            lrms_url = lrms_url + host + "/" + "jobmanager-" + lrms      
            nodes_per_glidein = nodes
            
            if (num_glidein != None and num_glidein != 0):
                nodes_per_glidein = nodes/num_glidein # must be divisible 
                print "Glidin URL: " + lrms_url    
                print "hosts: " + str(i) + " number of replica_processes (total): " + str(nodes)
                print "number glide-ins: " + str(num_glidein) + " node per glidein: " + str(nodes_per_glidein)
                print "Project: " + project + " Queue: " + queue + " Working Dir: " +workingdirectory
            
                # start job
                for ng in range(0, num_glidein):
                    advert_glidin_job = advert_job.advert_glidin_job(self.advert_host)
                    advert_glidin_job.start_glidin_job(lrms_url, 
                                                   nodes_per_glidein,
                                                   queue,
                                                   project,
                                                   workingdirectory, userproxy)
                    if i.has_key("glide_in_jobs") == False:
                        i["glide_in_jobs"] = []
                    i["glide_in_jobs"] .append(advert_glidin_job);
                    print "Started: " + str(advert_glidin_job)  + " Glide-In Job Number: " + str(ng)

    def stop_glidin_jobs(self):
        for resource in self.resourceMap.keys():
            i = self.resourceMap[resource]   
            num_glidein = int(i["number_glide_in"])
            glide_in_jobs = i["glide_in_jobs"]
            for j in range(0, num_glidein):     
                job = glide_in_jobs[j]
                job.cancel()
    
    def get_machine_info(self, machine):
        host = machine["host"]
        nodes = int(machine["total_number_nodes"])
        num_nodes_per_glidein = nodes
        num_glidein = int(machine["number_glide_in"])
        return host, nodes, num_nodes_per_glidein, num_glidein
    
    def check_glidein_states(self, current_replica_id_glidein_dict, start_glidin):
        """ check for Glide-In states
            returns dictionary: <replica_id, glidein_url> 
            only keys from replicas with active glidein job are set."""
        # query glidin job states and cache them into a dict.
        replica_id_glidein_dict = {}
        if self.glide_in == True:
            print "Number unique hosts: " + str(len(self.resourceMap.keys()))
            replica_id = 0
            for resource in self.resourceMap.keys():
                machine = self.resourceMap[resource]
                host, nodes, num_nodes_per_glidein, num_glidein = get_machine_info(machine)
                if num_glidein > 1:
                    num_nodes_per_glidein = num_rep_per_host/num_glidein
                glidin_jobs = machine["glide_in_jobs"]     
                print "Host: " + host + " Number Glide-Ins: " + str(len(glidin_jobs)) \
                        + " Number Nodes per GlideIn: " + str(num_nodes_per_glidein)
                for j in range(0, len(glidin_jobs)):
                    state = glidin_jobs[j].get_state_detail()
                    glidin_url = glidin_jobs[j].glidin_url 
                    print "glidein: " + glidin_url + " state: " + state
                    if state.lower()== "running":
                        # distribute replicas to glideins by setting glideinurl to replicaid
                        for r in range(0, num_nodes_per_glidein):
                            #replica_id = (i*num_rep_per_host) + (j*num_nodes_per_glidein) + r
                            if current_replica_id_glidein_dict.has_key(replica_id)==False:
                                print "Glide-In: " + glidin_url + " got active after: " + str(time.time()-start_glidin) + " s"
                            print "set replica id: " + str(replica_id) + " glidein: " + glidin_url + " state: running"
                            replica_id_glidein_dict[replica_id]=glidin_url
                            replica_id = replica_id + 1
                    else:
                        replica_id = replica_id + num_nodes_per_glidein
        return replica_id_glidein_dict

    def stage_files(self, file_list, machine, replica_id):
        """ stage passed file list to specified remote machine
            create directory in working dir for replica_id """ 
        remote_machine_ip = machine["host"]
        if machine.has_key("gridftp_url"):
           remote_machine_ip = machine["gridftp_url"]
        remote_dir = machine["working_dir_root"] + "/" + replica_id
        # prepare parameter 
        self.prepare_NAMD_config(irep) 
        if self.scp == True:
           self.file_stage_in_with_scp(file_list, remote_machine_ip, remote_dir)
        else:
           self.file_stage_in_with_saga(file_list, remote_machine_ip, remote_dir) 
        
        print "(INFO) Replica %d : Input files are staged into %s  "%(replic_id, remote_machine_ip)

    #########################################################
    #  run_REMDg
    #########################################################
    def run_REMDg(self):
        
        """ Main loop running replica-exchange """
        start = time.time()
        numEX = self.exchange_count    
        ofilename = "remd-temp.out"
        if self.glide_in == True:
            print "Start with glidein"
            self.start_glidin_jobs()
        else:
            print "Start without glidein not supported"
            return
    
        iEX = 0
        total_number_of_namd_jobs = 0
        current_replica_id_glidein_dict={}
        replica_id = 0
        while 1:
            print "\n"
            # reset replica number
            numReplica = self.total_number_replica
            current_replica_id_glidein_dict = check_glidein_states(current_replica_id_glidein_dict, start)
            if len(current_replica_id_glidein_dict.keys())==numReplica:
                    all_glideins_ready = True
                    print "All Glide-Ins got active after: " + str(time.time()-start) + " s"
            # iterate through all resources
            for resource in self.resourceMap.keys():
                machine = self.resourceMap[resource]
                host, nodes, num_nodes_per_glidein, num_glidein = get_machine_info(machine)
                if num_glidein > 1:
                    num_nodes_per_glidein = num_rep_per_host/num_glidein
                glidin_jobs = machine["glide_in_jobs"]     
                print "Host: " + host + " Number Glide-Ins: " + str(len(glidin_jobs)) \
                        + " Number Nodes per GlideIn: " + str(num_nodes_per_glidein)
                # interate through all Glide-In jobs
                for j in range(0, len(glidin_jobs)):
                    state = glidin_jobs[j].get_state_detail()  
                    glidin_url = glidin_jobs[j].glidin_url 
                    print "glidein: " + glidin_url + " state: " + state
                    if state.lower()== "running":
                        ############## NPT staging ######################################
                        if init == True:
                            self.stage_files(self.stage_in_file, machine, replica_id)
                        else:
                            self.stage_files([os.getcwd() + "/NPT.conf"], machine, replica_id)
                        ################ replica job spawning ###########################  
                        # job submit   
                        self.replica = []
                        start_time = time.time()
                        print "check host: " + str(host)
                        jd = self.set_saga_job_description(machine, replica_id)
                        dest_url_string = "gram://" + host + "/" + "jobmanager-" + RE_info.remote_host_local_schedulers[irep]     # just for the time being
                        error_msg, new_job = self.submit_job_advert(glidin_url, jd)
                        self.replica.append(new_job)
                        replica_id = replica_id + 1
                        print "(INFO) Replica " + "%d"%irep + " started (Num of Exchange Done = %d)"%(iEX)
               
    
            end_time = time.time()        
            # contains number of started replicas
            numReplica = len(RE_info.replica)
            if numReplica == 0: # no replica process started
                time.sleep(10)
                continue # next attempt to start replica processes
    
            print "started " + "%d"%numReplica + " of " + "%d"%RE_info.replica_count + " in this round." 
            print "Time for spawning " + "%d"%numReplica + " replica: " + str(end_time-start_time) + " s"
            ####################################### Wating for job termination ###############################
            # job monitoring step
            energy = [0 for i in range(0, numReplica)]
            flagJobDone = [ False for i in range(0, numReplica)]
            numJobDone = 0
    
            print "\n\n" 
            while 1:    
                print "\n##################### Replica State Check at: " + time.asctime(time.localtime(time.time())) + " ########################"
                for irep in range(0, numReplica):
                    running_job = self.replica[irep]
                    try: 
                        state = running_job.get_state()
                    except:
                        pass
                    print "job: " + str(running_job) + " received state: " + str(state)
                    if (str(state) == "Done") and (flagJobDone[irep] is False) :   
                        print "(INFO) Replica " + "%d"%irep + " done"
                        energy[irep] = self.get_energy(irep)##todo get energy from right host
                        flagJobDone[irep] = True
                        numJobDone = numJobDone + 1
                        total_number_of_namd_jobs = total_number_of_namd_jobs + 1
                    elif(str(state)=="Failed"):
                        self.stop_glidin_jobs()
                        sys.exit(1)
                
                if numJobDone == numReplica:
                        break
                time.sleep(30)
    
            ####################################### Replica Exchange ##################################    
            # replica exchange step        
            print "\n(INFO) Now exchange step...."
            for irep in range(0, numReplica-1):
                en_a = energy[irep]
                en_b = energy[irep+1]
                self.do_exchange(energy, irep, irep+1)
    
            iEX = iEX +1
            output_str = "%5d-th EX :"%iEX
            for irep in range(0, numReplica):
                output_str = output_str + "  %5d"%self.temperatures[irep]
            
            print "\n\nExchange result : "
            print output_str + "\n\n"
            
            ofile = open(ofilename,'a')
            for irep in range(0, numReplica):
                ofile.write(" %d"%(self.temperatures[irep]))
            ofile.write(" \n")            
            ofile.close()
    
            if iEX == numEX:
                break
    
            ########################## delete old jobs #####################
            if GlideIn == True:    
                for i in self.replica:
                    i.delete_job()
                    
        self.print_config()
        print "REMD Runtime: " + str(time.time()-start) + " s; Glide-In: " + str(self.glide_in) \
                + "; number replica: " + str(self.total_number_replica) + "; CPR: " + str(self.cpr) \
                + "; number namd jobs: " + str(total_number_of_namd_jobs)
        # stop gliding job        
        self.stop_glidin_jobs()
        
    def print_config(self):
        for section in resourceMap.keys():
            print section
            for option in section.items():
                print " ", option[0] , "=", option[1]
    
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
    
    # AL: I disabled this option temporarly since it is not working
    #     I also added a usage message
    #if options.type in (None,"test_RE"):
        #run_test_RE(options.numreplica,20)   #sample test for Replica Exchange with localhost
    if options.type != None and options.type in ("REMD"):
        re_manager = ReManager(options.configfile)
        re_manager.run_REMDg() 
    else:
        print "Usage : \n python " + sys.argv[0] + " --type=<REMD> --configfile=<configfile> \n"
        print "Example: \n python " + sys.argv[0] + " --type=REMD --configfile=remd_manager.config"
        sys.exit(1)      
        
    #print "REMDgManager Total Runtime: " + str(time.time()-start) + " s"