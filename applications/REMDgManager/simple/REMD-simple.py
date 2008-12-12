""" The main script for REMDgManager (w/o migol) : REMDManager-v1.0.py 
2008/8/26

Usage : 
        python REMD-simple.py  --configfile=re_manager_v3.conf 

jhkim at cct dot lsu dot edu
luckow at cs dot uni-potsdam dot de

(Note on the current stage)
1. Currently, the following simple scenario is assumed.
a. each replica is submitted to each local scheduler via CPR/Migol  (will be changed with the major revision) (again not for this version)
b. remote directories should exist at each HPC (see remd_manager.config)
c. There is a wrapper in HPC to register checkpoint files to migol (all file are registered as checkpoint files) (again not for this version)
d. This version does not have migol  part.
e. remd-temp.out will contain temperature trajectories of each replica 
f. NPT.conf is modified before being sent to HPC
g. "output.txt" is the output file name with which energy is pulled out for the replica exchange step
h. get_energy(), do_exchange(), prepare_NAMD_config() might be changed later since implementaion is not optimized yet
: energy search in output.txt is not efficient as the size of output.txt grows, prepare_NAMD_config() needs more for assigning other initial files 

"""

import sys
import os
import random
import time
import optparse
import saga
import re
import math
import threading
import traceback
import pdb
import ConfigParser

class ReManager():

    """ 
    This class holds information about the application and replicas remotely running via SAGA-CPR/MIGOL
    """    
    def __init__(self, config_filename):
        self.stage_in_files = []
        self.exchange_count = 0
        self.arguments = []
        
        # lists for variables of each replica (Note that these variable should have n variables where n is self.replica_count
        self.replica_count = 0
        self.temperatures = []
        
        # instant variable for replica exchange
        self.replica_jobs = []   # saga jobs
        self.replica_job_machine_dic ={}
        
        # file staging
        # contains ids of staged files
        # <host, [replica_id1, ...]
        self.host_file_dict = {}

        self.read_config(config_filename)
        # init random seed
        random.seed(time.time()/10.)
        
    def read_config(self, conf_file):
        # read config file
        config = ConfigParser.ConfigParser()
        print ("read configfile: " + conf_file)
        config.read(conf_file)
        # RE configuration
        default_dict = config.defaults()
        self.arguments = default_dict["arguments"].split()
        self.number_of_mpi_processes = 1
        try:
            self.number_of_mpi_processes = config.getint("DEFAULT", "number_of_mpi_processes") 
        except:
            pass
        self.exchange_count = config.getint("DEFAULT", "exchange_count")
        self.total_number_replica = config.getint("DEFAULT", "total_number_replica")
        self.temperatures = default_dict["temperature"].split()
        self.stage_in_file = default_dict["stage_in_file"].split()

        # Resources       
        self.resourceMap = {}
        self.min_number_cores_in_glidein=None
        for section in config.sections():
            print section
            optionMap = {}
            for option in config.options(section):
                print " ", option, "=", config.get(section, option)
                optionMap[option] = config.get(section, option)            
            self.resourceMap[section] = optionMap
        print "minimal number of nodes per Glidein: " + str(self.min_number_cores_in_glidein)
        print str(self.resourceMap)
    
  
    #####################################
    #  Elementary Functions
    ########################################################
    def set_saga_job_description(self, machine, replica_id):
        jd = saga.job.description()
        jd.spmd_variation = "mpi" # launch MPI directly
        jd.number_of_processes = str(self.number_of_mpi_processes)
        jd.arguments = self.arguments
        jd.executable = machine["executable"]
        try:
            jd.queue = machine["queue"]
        except:
            pass
        try:
            jd.job_project = [machine["allocation"]] 
        except:
            pass
        jd.working_directory = machine["working_dir_root"] + "/" + str(replica_id)
        jd.output = "output.txt"  
        jd.error = "error.txt"
        return jd


    def file_stage_in_with_saga(self, input_file_list_with_path, remote_url_prefix, remote_dir):
        cwd = os.getcwd()
        for ifile in input_file_list_with_path:
            # destination url
            dest_url = saga.url(remote_url_prefix + "/")
            ifile_basename = os.path.basename(ifile)

            try:
                dest_dir = saga.url(remote_url_prefix)
                dest_dir.path = remote_dir
                saga.file.directory(dest_dir, saga.file.Create |  saga.file.ReadWrite)
            except:
                print "Could not create: " + dest_dir.get_string()

            dest_url.path = os.path.join(remote_dir, ifile_basename)

            # source url
            source_url = saga.url('file://' + os.path.join(cwd, ifile))

            if not os.path.isfile(ifile):
                error_msg = "Input file %s does not exist in %s"%(ifile_basename, os.path.dirname(ifile))
                print(error_msg)
            else:
                try:
                    print "stage file: " + source_url.get_string() + " to " + dest_url.get_string()
                    sagafile = saga.file.file(source_url)
                    sagafile.copy(dest_url)
                except saga.exception, e:
                    error_msg = "Input file %s failed to be staged in"%(ifile_basename)
                    print(error_msg)
                    
        return None
    
    def file_stage_out_with_saga(self, file_list, local_dir, remote_url_prefix, remote_dir):
        for ifile in file_list:
            try:
                source_url = saga.url(remote_url_prefix)
                source_url.path= os.path.join(remote_dir, ifile)
                dest_url = saga.url("file://" + local_dir + "/")
                dest_url.path = ifile
                print "(DEBUG) Staging out output.txt file at %s to %s"%(source_url.get_string(), dest_url.get_string())
                sagafile = saga.file.file(source_url)
                sagafile.copy(dest_url)
            except saga.exception, e:
                error_msg = "File stage out failed: "+ source_url.get_string()
    
        return None
    
    
    def submit_job(self, dest_url_string, jd):
        # submit job to GRAM via SAGA adaptor
        js = saga.job.service(saga.url(dest_url_string))
        new_job = js.create_job(jd)
        new_job.run()
        return new_job

    def prepare_NAMD_config(self, replica_id):
        # The idea behind this is that we can simply modify NPT.conf before submit a job to set temp and other variables
        ifile = open("NPT.conf")   # should be changed if a different name is going to be used
        lines = ifile.readlines()
        for line in lines:
            if line.find("desired_temp") >= 0 and line.find("set") >= 0:
                items = line.split()
                temp = items[2]
                if eval(temp) != self.temperatures[replica_id]:
                    print "\n (DEBUG) temperature is changing to " + str(self.temperatures[replica_id]) + " from " + temp + " for rep" + str(replica_id)
                    lines[lines.index(line)] = "set desired_temp %s \n"%(str(self.temperatures[replica_id]))
        ifile.close() 
        ofile = open("NPT.conf","w")
        for line in lines:    
            ofile.write(line)
        ofile.close()

    def get_energy(self, machine, replica_id):
        #I know This is not the best one!  namd output is staged out and take the energy out from the file
        file_list = ["output.txt"]  
        local_dir = os.getcwd()
        remote_url_prefix = machine["file_url"]
        remote_dir = machine["working_dir_root"] + "/" + str(replica_id)
    
        self.file_stage_out_with_saga(file_list, local_dir, remote_url_prefix, remote_dir)
       
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
        delta = (1./int(self.temperatures[irep])/factor - 1./int(self.temperatures[irep+1])/factor)*(en_b-en_a)
        if delta < 0:
            iflag = True
        else :
            if math.exp(-delta) > random.random() :
                iflag = True
    
        if iflag is True:
            tmpNum = self.temperatures[jrep]
            self.temperatures[jrep] = self.temperatures[irep]
            self.temperatures[irep] = tmpNum
    
        print "(DEBUG) delta = %f"%delta + " en_a = %f"%en_a + " from rep " + str(irep) + " en_b = %f"%en_b +" from rep " + str(jrep)

    def stage_files(self, file_list, machine, replica_id):
        """ stage passed file list to specified remote machine
            create directory in working dir for replica_id """ 
        remote_url_prefix = machine["file_url"]
        remote_dir = machine["working_dir_root"] + "/" + str(replica_id)
        # prepare parameter 
        self.prepare_NAMD_config(replica_id) 
        self.file_stage_in_with_saga(file_list, remote_url_prefix, remote_dir) 
        print "(INFO) Replica %d : Input files are staged into %s  "%(replica_id, remote_url_prefix)

    #########################################################
    #  run_REMDg
    #########################################################
    def run_REMDg(self):
        
        """ Main loop running replica-exchange """
        start = time.time()
        numEX = self.exchange_count    
        ofilename = "remd-temp.out"
        iEX = 0
        total_number_of_namd_jobs = 0
        while 1:
            print "\n"
            # reset replica number
            numReplica = self.total_number_replica
            self.replica_jobs = []

            # Job spawning
            print "############# spawn jobs ################"
            start_time = time.time()
            replica_id = 0
            for resource in self.resourceMap.keys():
                machine = self.resourceMap[resource]
                host = saga.url(machine["rm_url"]).host
                nodes = int(machine["number_nodes"])
                num_jobs = nodes / self.number_of_mpi_processes
                print "Host: " + host + " Nodes: " + str(nodes) + " Num jobs: " + str(num_jobs)
                for i in range(0, num_jobs):
                    if replica_id >= self.total_number_replica:
                        print "All replica started. Not starting more jobs."
                        break;
                    ############## File staging ######################################
                    if self.host_file_dict.has_key(host)==False:
                            self.host_file_dict[host]=[]
                    files_staged_replica_id = self.host_file_dict[host]
                    if files_staged_replica_id.count(replica_id) == 0 : # stage all file
                        self.stage_files(self.stage_in_file, machine, replica_id)
                        self.host_file_dict[host].append(replica_id)
                    else: # stage only configuration
                        self.stage_files([os.getcwd() + "/NPT.conf"], machine, replica_id)
                    ################ replica job spawning ###########################  
                    print "check host: " + str(host)
                    jd = self.set_saga_job_description(machine, replica_id)
                    dest_url_string = machine["rm_url"]     # just for the time being
                    new_job = self.submit_job(dest_url_string, jd)
                    self.replica_jobs.insert(replica_id, new_job)
                    self.replica_job_machine_dic[replica_id] = machine
                    replica_id = replica_id + 1
                    print "(INFO) Replica " + "%d"%replica_id + " started (Num of Exchange Done = %d)"%(iEX)

            end_time = time.time()        
            # contains number of started replicas
            numReplica = len(self.replica_jobs)
    
            print "started " + "%d"%numReplica + " of " + str(self.total_number_replica) + " in this round." 
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
                    running_job = self.replica_jobs[irep]
                    try: 
                        state = running_job.get_state()
                    except:
                        pass
                    print "replica_id: " + str(irep) + " job: " + str(running_job) + " received state: " + str(state)
                    if (str(state) == "Done") and (flagJobDone[irep] is False) :   
                        print "(INFO) Replica " + "%d"%irep + " done"
                        machine = self.replica_job_machine_dic[irep]
                        energy[irep] = self.get_energy(machine, irep)##todo get energy from right host
                        flagJobDone[irep] = True
                        numJobDone = numJobDone + 1
                        total_number_of_namd_jobs = total_number_of_namd_jobs + 1
                    elif(str(state)=="Failed"):
                        sys.exit(1)
                
                if numJobDone == numReplica:
                        break
                time.sleep(15)
    
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
                output_str = output_str + "  %s"%self.temperatures[irep]
            
            print "\n\nExchange result : "
            print output_str + "\n\n"
            
            ofile = open(ofilename,'a')
            for irep in range(0, numReplica):
                ofile.write(" %s"%(self.temperatures[irep]))
            ofile.write(" \n")            
            ofile.close()
    
            if iEX == numEX:
                break
    
            ########################## delete old jobs #####################
            for i in self.replica_jobs:
                i.cancel()
                    
        self.print_config()
        print "REMD Runtime: " + str(time.time()-start) + " s; " \
                + "; number replica: " + str(self.total_number_replica) \
                + "; number namd jobs: " + str(total_number_of_namd_jobs)
        
    def print_config(self):
        for section in self.resourceMap.keys():
            print section
            optionMap = self.resourceMap[section]
            for option in optionMap.items():
                print " ", option[0] , "=", option[1]
    
    def __del__(self):
        for i in self.replica_jobs:
            i.cancel()

    
#########################################################
#  main
#########################################################
if __name__ == "__main__" :
    start = time.time()
    op = optparse.OptionParser()
    op.add_option('--configfile','-c')
    options, arguments = op.parse_args()
    
    if options.configfile != None:
        re_manager = ReManager(options.configfile)
        re_manager.run_REMDg() 
    else:
        print "Usage : \n python " + sys.argv[0] + " --configfile=<configfile> \n"
        print "Example: \n python " + sys.argv[0] + " --configfile=remd_manager.config"
        sys.exit(1)      
        
    #print "REMDgManager Total Runtime: " + str(time.time()-start) + " s"
