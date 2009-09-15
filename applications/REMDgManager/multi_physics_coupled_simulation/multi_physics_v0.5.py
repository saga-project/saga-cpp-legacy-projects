""" CFD-MD multi-physics Distrbuted Adaptive Job management via bigjob 

You can choose one of the folowing scenarios 
iscenario = 1 : One bigjob and two subjobs
iscenario = 2 : Two bigjob and two subjobs in one machine
iscenario = 3 : Two bigjob and two subjobs in two machines


Key Assumptions
a. Two applications, CFD and MD are running as two subjobs in one bigjob or in two bigjobs separately
b. Two bigjob(s) are only submitted in the begining and not submitted again (for logner job, this should be changed...)
c. DAAMS_MP_MANAGER_DIR and DAAMS_MP_AGENT_DIR : directories containing Manager (this script) and Agent (advert_launcher.sh, advert_launcher.py, advert_launcher.conf). For the current implementation, both directories are same in the same machine
d. URL_MAIN_MACHINE and URL_REMOTE_MACHINE : machine url for the main machine having manager and agent
e. MD_CFD_DIR : directory containing CFD and MD and input files
f. remote machine used in the scenario 3 should have the same names for the above directories.
g. at this moment, machines beloinging to LONI are only used at this moment:
qb1.loni.org, louie1.loni.org, poseidon1.loni.org, oliver1, eric1.loni.org. 

Prerequisite
a. advert server (postgresql) 
b. saga and globus for manager and agent (in the local scheduler environment)
c. only tested with pbs

Augst 8th 2009
(Note on this version)
a. scenario 2 is mostly supported if the assumptions written above are carefully considered
b. Now most of parameter setting can be made witht the global variables shown in the beginning. 
c. scenario 3 is now available, but need more testing, in particular, with respect to file exchange issues
d. if a bigjob in the remote machine is up earlier than one in the main machine, subjob is not submittted to the bigjob of the remote machine in the first iteration

"""

import saga
import os
import advert_job
import time
import pdb
import commands
import glob

advert_host = "fortytwo.cct.lsu.edu"

# For most of cases, you configure here for your run
CFD_MD_DIR = "/work/jhkim/TestKim/"
#CFD_MD_DIR = "/work/athota1/TestKim/"

DAAMS_MP_MANAGER_DIR = os.getcwd()
DAAMS_MP_AGENT_DIR = "/work/jhkim/bigjob_test"

URL_MAIN_MACHINE= "louie1.loni.org"
URL_REMOTE_MACHINE = "poseidon1.loni.org"

RE_AGENT = DAAMS_MP_AGENT_DIR + "/advert_launcher.sh" # path to agent
PROJECT = "loni_stopgap2" #allocation
QUEUE = "workq" # queue (PBS)

RUNNING_TIME = [500, 300]  # min
NUMS_OF_CPUS_PER_BIGJOB = [16 , 32] # total num of cpus for each big job

def get_restart_job_description(nprocs_cfd, nprocs_md):
    cfd_jd = saga.job.description()
    md_jd = saga.job.description()

    # cfd job description
    cfd_jd.executable = CFD_MD_DIR + "CFDMDCouple"
    cfd_jd.number_of_processes = str(nprocs_cfd)
    cfd_jd.spmd_variation = "mpi"
    cfd_jd.arguments = ["-mesh Couette121.grd -domain MD -irestart Yes"]
        
    cfd_jd.working_directory = CFD_MD_DIR
    cfd_jd.output = "output_cfd.txt"
    cfd_jd.error = "error_cfd.txt"

    # md job desription
    md_jd.executable = CFD_MD_DIR + "lmp_linux"
    md_jd.number_of_processes = str(nprocs_md)
    md_jd.spmd_variation = "mpi"
    md_jd.arguments = ["-in in.flow.couette_3D_BJ_restart"]
    md_jd.working_directory = CFD_MD_DIR
    md_jd.output = "output_md.txt"
    md_jd.error = "error_md.txt"

    return cfd_jd, md_jd


def get_start_job_description(nprocs_cfd, nprocs_md):
    cfd_jd = saga.job.description()
    md_jd = saga.job.description()

    # cfd job description
    cfd_jd.executable = CFD_MD_DIR + "CFDMDCouple"
    cfd_jd.number_of_processes = str(nprocs_cfd)
    cfd_jd.spmd_variation = "mpi"
    cfd_jd.arguments = ["-mesh Couette121.grd -domain MD"]

    cfd_jd.working_directory = CFD_MD_DIR 
    cfd_jd.output = "output_cfd.txt"
    cfd_jd.error = "error_cfd.txt"

    #md job description
    md_jd.executable = CFD_MD_DIR + "lmp_linux"
    md_jd.number_of_processes = str(nprocs_md)
    md_jd.spmd_variation = "mpi"
    md_jd.arguments = ["-in in.flow.couette_3D_BJ"]
    
    md_jd.working_directory =  CFD_MD_DIR
    md_jd.output = "output_md.txt"
    md_jd.error = "error_md.txt"

    return cfd_jd, md_jd

def shutdown_bigjobs(bigjob_list):
    for bigjob in bigjob_list:
        bigjob.cancel()
    
def set_nprocs_for_new_iteration(old_nprocs_cfd, old_nprocs_md, tot_num_of_bigjobs, nums_of_cpus_per_bigjob):
    # at this moment, all cpus in a bigjob are assigned for one subjob
    if tot_num_of_bigjobs == 1:
        nprocs_cfd = old_nprocs_cfd
        nprocs_md = old_nprocs_md
    elif tot_num_of_bigjobs == 2:
        nprocs_cfd = nums_of_cpus_per_bigjob[0]
        nprocs_md =  nums_of_cpus_per_bigjob[1]


    return nprocs_cfd, nprocs_md


def set_nprocs_from_loadbalancing(nprocs_per_node, old_nprocs_cfd, old_nprocs_md, perf_1, perf_2, num_of_available_bigjobs ):
    # this load balancing is only for the scenario 1, meaning one bigjob case

    # start load balancing calculation
    lb_start=time.time()

    # unit_ratio: the unit of processing group (same as the number of processors in a node - now specified as 4 for convenience of test)
    nodepes = nprocs_per_node[0]


# 1: How can I get total number of processors? Shall I use 'nodes'? Or, can I do jd1.num_PE+jd2.num_PE? It seems it would be a character variable
# Answer gained!!!!!
    proc_1=int(old_nprocs_cfd)
    proc_2=int(old_nprocs_md)
    nprocs=proc_1+proc_2

    if (nprocs<3*nodepes):
        nodepes = 1

    # processor ratio
    optcfd=min(nprocs-2*nodepes,max(nprocs*perf_1*proc_1/(perf_1*proc_1+perf_2*proc_2),nodepes))

    cfdproc1=int(optcfd/nodepes)
    cfdproc2=cfdproc1+1
    cfdproc1=cfdproc1*nodepes
    cfdproc2=cfdproc2*nodepes

    mdproc1 = nprocs-cfdproc1
    mdproc2 = nprocs-cfdproc2

    # compare for best performance
    tchk1=max(proc_1*perf_1/cfdproc1,proc_2*perf_2/mdproc1)
    tchk2=max(proc_1*perf_1/cfdproc2,proc_2*perf_2/mdproc2)

    if (tchk1 < tchk2):
        cfdproc = cfdproc1
        mdproc  = mdproc1
    else:
        cfdproc = cfdproc2
        mdproc  = mdproc2


    # check finish time
    lb_end=time.time()
    lbtime=lb_end-lb_start

    return cfdproc, mdproc, lbtime

def check_file_exist(filename_with_path, machine_url):

    if URL_MAIN_MACHINE == machine_url:
        if os.path.exists(filename_with_path) == True:
            ifile = open(filename_with_path)
            lines = ifile.readlines()
            ifile.close()
            items = lines[0].split()
            if items == []:
                return "not exist"
            else:
                return "exist"
           
        else:
            return "not exist"
        
    else:
        tmpfile = DAAMS_MP_MANAGER_DIR + "/tmp.dat"
        if os.path.exists(tmpfile):
            os.remove(tmpfile)
        res = remote_filecopy_with_saga(filename_with_path, machine_url, DAAMS_MP_MANAGER_DIR +"/tmp.dat", URL_MAIN_MACHINE)
        if res is not "Success":
            return "not exist"

        ifile = open(DAAMS_MP_MANAGER_DIR+ "/tmp.dat")
        lines = ifile.readlines()
        ifile.close()
        if lines == []: return "not exist"
        else:
            items = lines[0].split()
            if items == []:
                return "not exist"
            else:
                return "exist"
        


def remote_filecopy_with_saga(filename_with_local_path_from, machine_url_from, filename_with_local_path_to, machine_url_to):
    source_url = saga.url('file://' + machine_url_from + filename_with_local_path_from)
    dest_url = saga.url('gridftp://' + machine_url_to + filename_with_local_path_to)

    sagafile = saga.file.file(source_url)
    try:
        sagafile.copy(dest_url)
        print "\n(DEBUG)  remote file copy from %s of %s to %s of %s is attempted"%(filename_with_local_path_from, machine_url_from, filename_with_local_path_to, machine_url_to)
        
    except saga.exception, e:
        print "\n(WARNING) remote file copy from %s of %s to %s of %s is failed"%(filename_with_local_path_from, machine_url_from, filename_with_local_path_to, machine_url_to)
        return "Failed"
    print "\n(DEBUG) No error while remote file copy from %s of %s to %s of %s"%(filename_with_local_path_from, machine_url_from, filename_with_local_path_to, machine_url_to)
    return "Success"

def file_stage_in_for_remote_md_start():
    #find the file BJ.Restart.xxxx with the highest number attached    
    fl = glob.glob(CFD_MD_DIR + "/*.*")
    high_number = 0
    for ifl in fl:
        if ifl.find("BJ_Restart") >= 0 :
            number = int(ifl.split(".")[1])
            if number > high_number:
                high_number = number

    BJrestart_filename = "BJ_Restart." + str(high_number)  
    print "(INFO) now %s is transferred to %s from %s for remote md start"%(BJrestart_filename, URL_REMOTE_MACHINE, URL_MAIN_MACHINE)   
 
    filename_list = ["in.flow.couette_3D_BJ_restart",BJrestart_filename]

    for ifilename in filename_list:
        while 1:
            res = remote_filecopy_with_saga(CFD_MD_DIR+"/"+ ifilename, URL_MAIN_MACHINE, CFD_MD_DIR+"/"+ ifilename, URL_REMOTE_MACHINE)
            if res == "Success":
                break
           

def file_transfer_for_coupling(filename_from_cfd, filename_from_md, ifile_number):

    # the following means always the main machine runs CFD while the remote machine runs MD 
    cfd_url = URL_MAIN_MACHINE 
    md_url = URL_REMOTE_MACHINE

    if ifile_number in range(1,10):
        file_extension = "00"+str(ifile_number)
    elif ifile_number in range (10, 100):
        file_extension = "0"+str(ifile_number)
    elif ifile_numbter in range (100, 1000):
        file_extension = str(ifile_number)
    else:
        print "\n(WARNING) ifile_number is larger than 999: %d"%ifile_number            

   
    filename = CFD_MD_DIR + filename_from_cfd + "."+file_extension
    
    while 1:            
        isthere = check_file_exist(filename, cfd_url)
        if isthere == "exist":
            while 1: 
                res = remote_filecopy_with_saga(filename, cfd_url, filename, md_url)
                if res == "Success":
                    break
            break
         

    print "\n(INFO) CFDtoMD file  %s is moved from %s to %s"%(filename, cfd_url, md_url)

    filename = CFD_MD_DIR + filename_from_md + "."+file_extension
               
    while 1:
        isthere = check_file_exist(filename, md_url)
        if isthere == "exist":
            while 1:
                res = remote_filecopy_with_saga(filename, md_url, filename, cfd_url)
                if res == "Success":
                    break
            break


    print "\n(INFO) MDtoCFD file % is moved from %s to %s"%(filename, md_url, cfd_url)


if __name__ == "__main__":

    num_of_restart = 20  # number of iteration for restarts

    while 1:
        iscenario = raw_input("\n\n\nChoose your option \n1. One big job and two subjobs  \n2. Two big jobs in one machine \n3. Two big jobs in two machines  \n -->  ")
        iscenario = int(iscenario)
        if iscenario in (1,2,3):
            print "\n(INFO) You chosed iscenario = %d and now lauching multi-physics DAAMS"%iscenario
            break
        
    
    # choose loadbalancing option   
    option_loadbalancing = True 
    #option_loadbalancing = False 
    if iscenario == 2 or iscenario == 3:
        option_loadbalancing = False  # tentatively loadbalancing is not applied when more than two bigjobs are chosen
    print "\n(INFO) You choose option_loadbalancing = %s"%(str(option_loadbalancing))

    # match nprocs per node with machine with url
    lrms_urls = []
#    nprocs_per_node = [8]
    lrms_urls.append("gram://"+ URL_MAIN_MACHINE + "/jobmanager-pbs") # resource url
    if URL_MAIN_MACHINE is not "qb1.loni.org":
        nprocs_per_node = [4]
    else:
        nprocs_per_node = [8]
    
    if iscenario == 2:
        lrms_urls.append("gram://" + URL_MAIN_MACHINE + "/jobmanager-pbs") # resource url
        nprocs_per_node.append(nprocs_per_node[0])
    elif iscenario == 3:
        lrms_urls.append("gram://" + URL_REMOTE_MACHINE + "/jobmanager-pbs") # resource url
        if URL_MAIN_MACHINE is not "qb1.loni.org":
            nprocs_per_node.append(4)
        else:
            nprocs_per_node.append(8)

    if iscenario == 1:
        tot_num_of_bigjobs = 1
    else :
        tot_num_of_bigjobs = 2 

    bigjob_list = []   # list of Bijobs
    bigjob_timestamp = []   # string holding timestamps

    # time stamp for start
    total_start = time.time()
    bigjob_timestamp.append("Start: %s  %s "%(time.ctime(total_start), total_start))

    for ibj in range(tot_num_of_bigjobs):
        print "\n(INFO) Start Glide-In %d at: "%(ibj+1) + lrms_urls[ibj]
       
        BigJob_start = time.time()
      
        re_agent = RE_AGENT
        queue = QUEUE
        project = PROJECT
        workingdirectory = DAAMS_MP_AGENT_DIR 
        wall_time = RUNNING_TIME[ibj] 
        userproxy = None # userproxy (not supported yet due to contex issue w/SAGA) 

        # time stamp when a big job is submitted
        curr_time=time.time()
        bigjob_timestamp.append("%d-th big job is submitted :%s  %s "%((ibj+1),time.ctime(curr_time),curr_time))

        advert_glidin_job = advert_job.advert_glidin_job(advert_host)
        advert_glidin_job.start_glidin_job(lrms_urls[ibj],
                                        re_agent,
                                        NUMS_OF_CPUS_PER_BIGJOB[ibj],
                                        queue,
                                        project,
                                        workingdirectory, 
                                        userproxy,
                                        wall_time)
        print "\n(INFO)%d-th BigJob URL: "%(ibj+1) + advert_glidin_job.glidin_url + ", and the number of proecesses asked is %d"%NUMS_OF_CPUS_PER_BIGJOB[ibj]

        bigjob_list.append(advert_glidin_job)    

    # output (time checker) file open
    foutput=open(CFD_MD_DIR + "Tottime.dat","w")
 
    BigJobtime = time.time() - BigJob_start 
    foutput.write("BigJobLaunch : %s"%(BigJobtime))
    foutput.write("	\n ")

    foutput.write("LoadBalancing     SubjobLaunch    Runtime    CFDProc     MDProc     CFDtime     MDtime\n")
  
    if iscenario == 3:
        iCFDMD_file_number = 1
    ###### subjob submission/running/monitoring/resubmission ###########################
    for iter_restart in range(num_of_restart):

        #step 1:  count the number of running bigjob
        while 1:
            num_of_available_bigjobs = 0
            if iter_restart == 0:
                active_bigjob_flag = [False for i in range(tot_num_of_bigjobs)]

            for i in range(tot_num_of_bigjobs):
                state = bigjob_list[i].get_state_detail() 
                if state.lower() == "running":
                    if  active_bigjob_flag[i] == False :
                        curr_time = time.time() 
                        # time stamp when the change of the bigjob state to "running" is observed
                        bigjob_timestamp.append("Found that %d-th bigjob started:%s  %s "%(i,time.ctime(curr_time),curr_time))
                    
                        active_bigjob_flag[i] = True
                        print "\n(INFO) Now %d-th bigjob becomes now available for this iteration"%i
                    else:
                        print "\n(INFO) %d-th bigjob keeps running for this iteration"%i
                else:
                    if active_bigjob_flag[i] == True:
                        active_bigjob_flag[i] = False
                        print "\n(INFO) Now previously active %d-th bigjob becomes NO MORE available for this iteration"%i

            num_of_available_bigjobs = active_bigjob_flag.count(True)
          
 
            # I want to avoid the remote machine starts earlier, which means the main machine should be used first in any event
            if iscenario == 3 :
                if num_of_available_bigjobs == 1 :
                    if active_bigjob_flag[1] == True and active_bigjob_flag[0] == False :
                        num_of_available_bigjobs = 0    
                        active_bigjob_flag[1] = False
                        print "\n(INFO) Bigjob in the remote machine is up, but not used until bigjob in the main machine is up. Please be patient"

                elif num_of_available_bigjobs == 2 :
                    if iter_restart == 0:
                        num_of_available_bigjobs = 1
                        active_bigjob_flag[1] = False

                        print "\n(INFO) Bigjob in the remote machine is up, but not used for this first iteration."
            if num_of_available_bigjobs is not 0:
                print "\n\n(INFO) Now %d bigjob(s) will be used for this iteration \n"%num_of_available_bigjobs 
                break
               
       
        # step 2: start subjobs for MD and CFD 
        
        Subtask_start=time.time()

        if iter_restart == 0:  # initial run
            # hopefully it does not cause a problem. Two applications will have a half of the activated bigjob coming first
            # for initial iteration, we only use the first bigjob
            if num_of_available_bigjobs == 2:
                active_bigjob_flag[1] = False
                num_of_available_bigjobs == 1
            

            nprocs_cfd = NUMS_OF_CPUS_PER_BIGJOB[active_bigjob_flag.index(True)]/2
            nprocs_md = nprocs_cfd

            cfd_jd, md_jd = get_start_job_description(nprocs_cfd, nprocs_md) 
            
            foutput.write(str(0.0))
            foutput.write("     ")
        else:
            if option_loadbalancing == True:
                old_nprocs_cfd = nprocs_cfd
                old_nprocs_md = nprocs_md
                nprocs_cfd, nprocs_md, lbtime = set_nprocs_from_loadbalancing(nprocs_per_node, old_nprocs_cfd, old_nprocs_md, perf_1, perf_2, num_of_available_bigjobs ) 
                print "\n(INFO) after LB: " + str(nprocs_cfd) + " and " + str(nprocs_md) + "  from %d and %d"%(old_nprocs_cfd, old_nprocs_md)
                
                foutput.write(str(lbtime))
                foutput.write("     ")
                
                if iscenario in (2,3):
                    print "\n(INFO) At this moment, load balancing is not available with this scenario %d"%iscenario
            else :
                old_nprocs_cfd = nprocs_cfd
                old_nprocs_md = nprocs_md
                nprocs_cfd, nprocs_md = set_nprocs_for_new_iteration(old_nprocs_cfd, old_nprocs_md, num_of_available_bigjobs, NUMS_OF_CPUS_PER_BIGJOB) 

                if old_nprocs_cfd is not nprocs_cfd:   # means the remote job needs to start
                    if iscenario == 3:   # means the remote job needs to start
                        file_stage_in_for_remote_md_start()
                        time.sleep(120)  # good for waiting until the restart file is copied?    
                    print "\n(INFO) # of cpus for sub job changed for %d-th iteration : \n"%iter_restart + str(nprocs_cfd) + " and " + str(nprocs_md) + " from %d and %d"%(old_nprocs_cfd, old_nprocs_md) 

                foutput.write(str(0.0))
                foutput.write("      ")
  
            cfd_jd, md_jd = get_restart_job_description(nprocs_cfd, nprocs_md) 
            

        if num_of_available_bigjobs == 1:    #one bigjob two applications

            id = active_bigjob_flag.index(True)            
            advert_glidin_job = bigjob_list[id]
            subjob_start = time.time()
        
            advert_job_cfd = advert_job.advert_job(advert_host)
            advert_job_cfd.submit_job(advert_glidin_job.glidin_url, cfd_jd)

            advert_job_md = advert_job.advert_job(advert_host)
            advert_job_md.submit_job(advert_glidin_job.glidin_url, md_jd)

        elif num_of_available_bigjobs == 2:   #  two bigjobs, and thus one bigjob for one application   
            advert_glidin_job = bigjob_list[0]

            advert_job_cfd = advert_job.advert_job(advert_host)
            advert_job_cfd.submit_job(advert_glidin_job.glidin_url, cfd_jd)

            advert_glidin_job = bigjob_list[1]

            advert_job_md = advert_job.advert_job(advert_host)
            advert_job_md.submit_job(advert_glidin_job.glidin_url, md_jd)

        Subtask_end = time.time()
        Subtasktime=Subtask_end-Subtask_start
        foutput.write(str(Subtasktime))
        foutput.write("	 ")

        curr_time = time.time() 
        run_start = curr_time
        # time stamp when two subjobs are submitted
        bigjob_timestamp.append("Two subjobs are submitted : %s  %s "%(time.ctime(curr_time),curr_time))
        

        #### step 3 : subjob monitoring
        init_time = time.time()       
        while 1:
            state1 = str(advert_job_cfd.get_state())
            state2 = str(advert_job_md.get_state())
            curr_time = time.time()
            if int(curr_time-init_time) % 600 == 0:
                print "(INFO) status : %d BigJob up, the CFD subjob :"%num_of_available_bigjobs + state1 + " with %d processes"%nprocs_cfd + " and the MD subjob : " + state2 + " with %d processes"%nprocs_md
          
            if state1.lower() == "unknown" or state2.lower() == "unknown":
                run_qwait = time.time()
            curr_time = time.time()
            el_time = curr_time - init_time
            if iscenario == 3 and num_of_available_bigjobs == 2 and int(el_time)% 600 == 0 : 
                file_transfer_for_coupling("CFDtoMD.dat", "MDtoCFD.dat", iCFDMD_file_number ) 
                iCFDMD_file_number = iCFDMD_file_number + 1        

            if(state1=="Failed" or state1=="Done") and (state2=="Failed" or state2=="Done"):
                if state1.lower == "failed":
                    print "cfd is failed"
       
                if state2.lower == "failed":
                    print "md is failed"
                break

        print "\n(INFO)  Now %d-th iteration of %d is finished"%(iter_restart, num_of_restart)
       
        curr_time = time.time() 
        # time stamp when two subjobs are finished
        bigjob_timestamp.append("Two subjobs are finished:%s  %s "%(time.ctime(curr_time),curr_time))

        # check finish time
        run_end=time.time()
            # writing in the file
        runtime=run_end-run_qwait
        foutput.write(str(runtime))
        foutput.write("	 ")


        foutput.write(str(nprocs_cfd))
        foutput.write("	")
        foutput.write(str(nprocs_md))
        foutput.write("	")

        # file open and close
        f1=open(CFD_MD_DIR + "CFDtime.dat")

        if iscenario == 3 and num_of_available_bigjobs == 2 and iter_restart > 0:
            remote_filecopy_with_saga(CFD_MD_DIR + "MDtime.dat", URL_REMOTE_MACHINE, CFD_MD_DIR + "MDtime.dat", URL_MAIN_MACHINE)
        
        f2=open(CFD_MD_DIR + "MDtime.dat")
        perf_1=float(f1.readline())
        perf_2=float(f2.readline())
        print "\n(INFO) perf: " + str(perf_1) + " and " + str(perf_2)
        f1.close()
        f2.close()

        # time writing
        foutput.write(str(perf_1))
        foutput.write("	 ")
        foutput.write(str(perf_2))
        foutput.write("\n")
      
        daams_mp_logfile = open("mp.log","a")
        daams_mp_logfile.write("%d iteration result \n"%iter_restart)
        for timestamp in bigjob_timestamp:
            daams_mp_logfile.write(timestamp + "\n")

        daams_mp_logfile.write("CFD time : %f   MD time : %f \n"%(perf_1, perf_2))
        daams_mp_logfile.close()

    foutput.write("\n")
    total_end=time.time()
    totaltime=total_end-total_start
    foutput.write("Total Computation Time\n")
    foutput.write(str(totaltime))
    foutput.close()

    print "\n\n\n Overall results \n"
    for st in bigjob_timestamp:
        print "(INFO) %s"%st



    #Cleanup at the end
    shutdown_bigjobs(bigjob_lsit)    
