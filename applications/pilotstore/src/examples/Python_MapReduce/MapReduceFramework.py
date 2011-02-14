""" 
Author : Pradeep K Mantha 
Reviewer : Andre Luckow, Shantenu Jha

Description: The Map Reduce frame..... <<<<<< need to complete >>>>> 

Prerequisites: Bigjob should already be working.

Usage: Provide the configuration parameters in MapReduce.conf and execute it using python MapReduceFramework.py
"""

import sys
import saga
import os
import os.path
import time
import uuid
import getopt
import many_job

APPLICATION_NAME="Map_Reduce"
DATABASE_HOST=''

def has_finished(state):
    state = state.lower()
    if state=="done" or state=="failed" or state=="canceled":
        return True
    else:
        return False


class MapReduce:

    def __init__(self):

        # cache filename
        self.__input_file = ''
        # number of equal sized chunks
        self.__numchunks = 5
        # Size of each chunk
        self.__chunksize = 10
        # Optional postfix string for the chunk filename
        self.__postfix = ''
        # Program name
        self.__progname = "MapReduce.py"
        # Output
        self.__output = ''
        # Temp
        self.__tmp = ''
        # Url
        self.__url = ''
        #chunk_list
        self.__chunk_list = []
        # number of map jobs count
        self.__nbr_map_jobs = 0
        # number of reduce jobs count
        self.__nbr_reduce_jobs = 0
        # number of partition jobs
        self.__nbr_partition_jobs = 0
        # resource list
        self.__resource_list = []
        self.__reduce_resource_list = []
        # partition list
        self.__partition_list = []
        self.__partition_file_names = []
        self.__sorted_partition_file_names = []
        self.__mjs=''
        self.__mjs1=''
        self.__mjs2=''
        
    def conf_check(self):
    
        par_conf={"advert":0, "bigjob":0, "chunk_size":0, "input":0,"temp":0, "output":0,"url":0, "map_script":0,"reduce_script":0,"nbr_reduces":0,"partition_script":0}
        #par_conf={"advert":0, "bigjob":0, "chunk_size":0, "input":0,"temp":0, "output":0,"url":0, "map_script":0,"reduce_script":0,"number_cores":0, "allocation":0,"queue":0, "working_directory":0,"walltime":0}
        config_file="MapReduce.conf"
        if os.path.exists(config_file):
            if os.path.isfile(config_file):
                confile=open(config_file,"rb")
                conf={}
                
                for line in confile:
                    s=line.split("=")
                    s[1]=s[1].strip('\n')
                    conf[s[0]]=s[1]   
                confile.close()
                for key in par_conf.keys():
                    if key not in conf:
                        print key + " is not provided in the configuration file"
                        sys.exit(0)
                
                for par,value in conf.iteritems():
                    if par == "advert":
                        self.advert_host = value
                        print "\n >>> DATABASE_HOST is " , self.advert_host
                    if par == "chunk_size":
                        self.__chunksize = int(value)
                    if par == "input":
                        self.__input_file = value
                    if par == "temp":
                        self.__tmp = value
                    if par == "output":
                        self.__output = value
                    if par == "url":
                        self.__url = value
                    if par == "output_url":
                        self.__output_url = value
                    if par == "output_path":
                        self.__output_path=value
                    if par == "map_script":
                        self.__map_script = value
                    if par == "partition_script":
                        self.__partition_script = value
                    if par == "reduce_script":
                        self.__reduce_script = value
                    if par == "nbr_reduces":
                        self.__nbr_reduces = int(value)
                    if par == "bigjob":
                        sys.path.append(value)
                        self.__bigjob = value
                        self.__re_agent = value + "/bigjob_agent_launcher.sh"
                        self.__resource_list.append( {"gram_url" : self.__url, "number_cores" : "8", "allocation" : "loni_jhabig10", 
                           "queue" : "workq", "re_agent": self.__re_agent, "working_directory":"/work/pmantha/output/", "walltime":10 })
                        self.__reduce_resource_list.append( {"gram_url" : self.__output_url, "number_cores" : "8", "allocation" : "loni_jhabig10", 
                           "queue" : "workq", "re_agent": self.__re_agent, "working_directory":"/work/pmantha/output/", "walltime":10 }) 
                    

    def split(self):
        """ Split the file and save chunks to separate files """

        print ' >>> Splitting file', self.__input_file + "\n"
        
        try:
            f = saga.filesystem.file(self.__input_file)
        except:
            pass
        
        bname = (os.path.split(str(f.get_url())))[1]

        # Get the file size
        fsize = f.get_size()

        # Get size of each chunk
        self.__numchunks = int(float(fsize)/float(self.__chunksize))
        
        # print " saga file name " + str(f.get_url())
        print " >>> saga file size " + str(f.get_size()) + " >>> Number of chunks " + str(self.__numchunks) + "\n"
        # print ' Number of chunks', self.__numchunks 

    
        chunksz = self.__chunksize
        total_bytes = 0

        for x in range(self.__numchunks):
            chunkfilename = bname + '-' + str(x+1) + self.__postfix
            self.__chunk_list.append(self.__tmp + "/" + chunkfilename)

            # if reading the last section, calculate correct
            # chunk size.
            if x == self.__numchunks - 1:
                chunksz = fsize - total_bytes

            try:
                print 'Writing file',chunkfilename
                data = f.read(chunksz)
                total_bytes += len(data)
                """ Advert entry has to be created here instead of local chunks """
                chunkf = saga.filesystem.file(self.__tmp + "/" + chunkfilename, saga.filesystem.Write)
                chunkf.write(data)
                chunkf.close()
            except (OSError, IOError), e:
                print e
                continue
            except EOFError, e:
                print e
                break
        
        print " \n ***************************** Input File Split is done in temp location... " + self.__tmp + "************************** \n"
    
    def map_job_submit(self):
    ##########################################################################################
        print " >>> Starting BigJob ..................... \n"
        print " >>> Create manyjob service with advert service at ... " , self.advert_host + "\n"
        
        self.__mjs = many_job.many_job_service(self.__resource_list, None)
        
        jobs = []
        job_start_times = {}
        job_states = {}
        for u in self.__chunk_list:          
            k = u.replace('//','/').split('/')
            uname = (os.path.split(u))[1]
            temp_abs_path = "/" + "/".join(k[2:len(k)-1]) + "/" + uname
            print " >>> chunk path/name to be submitted to map subjob  " + temp_abs_path + " >>> " + uname
            # create job description
            try:
                jd = saga.job.description()
                jd.executable = self.__map_script
                jd.number_of_processes = "1"
                jd.spmd_variation = "single"                
                jd.arguments = [temp_abs_path]
                jd.working_directory = "/work/pmantha/output"
                jd.output =  "/work/pmantha/output/stdout-" + uname + ".txt"
                jd.error = "/work/pmantha/output/stderr-" + uname + ".txt"
                self.__partition_list.append(jd.output)
                subjob = self.__mjs.create_job(jd)
                subjob.run()
                print "Submited sub-job " + uname + "."
                jobs.append(subjob)
                job_start_times[subjob]=time.time()
                job_states[subjob] = subjob.get_state()
                self.__nbr_map_jobs = self.__nbr_map_jobs + 1
            except:
                #traceback.print_exc(file=sys.stdout)
                print " Map Job failed. Cancelling bigjob......"
                self.__mjs.cancel()
                sys.exit(0)
                try:
                    self.__mjs.cancel()
                except:
                    pass  
            
        print "************************ All Jobs submitted ************************"

        print " No of map subjobs created - " + str( self.__nbr_map_jobs)
        # Wait for task completion of map tasks - synchronization      
        
        while 1: 
            finish_counter=0
            result_map = {}
            for i in range(0, self.__nbr_map_jobs):
                old_state = job_states[jobs[i]]
                state = jobs[i].get_state()
                if result_map.has_key(state) == False:
                    result_map[state]=0
                result_map[state] = result_map[state]+1
                #print "counter: " + str(i) + " job: " + str(jobs[i]) + " state: " + state
                if old_state != state:
                    print "Job " + str(jobs[i]) + " changed from: " + old_state + " to " + state
                if old_state != state and has_finished(state)==True:
                    print "Job: " + str(jobs[i]) + " Runtime: " + str(time.time()-job_start_times[jobs[i]]) + " s."
                if has_finished(state)==True:
                    finish_counter = finish_counter + 1
                job_states[jobs[i]]=state

            print "Current states: " + str(result_map) 
            time.sleep(5)
            print " finished jobs - " + str(finish_counter) + " nbr of map jobs " +  str(self.__nbr_map_jobs)
            if finish_counter == self.__nbr_map_jobs:
                print " finished jobs - " + str(finish_counter) + " nbr of map jobs " +  str(self.__nbr_map_jobs)
                break
            

    def partition_job_submit(self):
    ##########################################################################################        
        jobs = []
        job_start_times = {}
        job_states = {}
        self.__mjs1 = many_job.many_job_service(self.__resource_list, None)
        print " length of partion list is " + str(len(self.__partition_list))   
        
        for u in self.__partition_list:
            uname = (os.path.split(u))[1]
            bname = (os.path.split(u))[0]
            print " >>> chunk [path/name] to be submitted to partition subjob  " + u + " >>> " + str(self.__nbr_reduces)
            for i in range(0,int(self.__nbr_reduces)):
                    self.__sorted_partition_file_names.append(u + "sorted-part-" + str(i))
            # create job description
            try:
                jd1 = saga.job.description()
                jd1.executable = self.__partition_script
                jd1.number_of_processes = "1"
                jd1.spmd_variation = "single"                
                jd1.arguments = [str(u), str(self.__nbr_reduces) ]
                jd1.working_directory = "/work/pmantha/output/"
                #jd1.output =  "/work/pmantha/output/partition_file_list"
                jd1.error = "/work/pmantha/output/stdparterr-" + uname + ".txt"
                subjob = self.__mjs1.create_job(jd1)
                subjob.run()
                print "Submited Partition sub-job " + uname + "."
                jobs.append(subjob)
                job_start_times[subjob]=time.time()
                job_states[subjob] = subjob.get_state()
                self.__nbr_partition_jobs = self.__nbr_partition_jobs + 1
            except:
                #traceback.print_exc(file=sys.stdout)
                print " Partition Job failed. Cancelling bigjob......"
                self.__mjs1.cancel()
                sys.exit(0)
                try:
                    self.__mjs1.cancel()
                except:
                    pass                      
            
        print "************************ All Partition Jobs submitted ************************"

        #print " No of Partition subjobs created - " + str( self.__nbr_map_jobs)
        # Wait for task completion of map tasks - synchronization      
        
        while 1: 
            finish_counter=0
            result_map = {}
            for i in range(0, self.__nbr_partition_jobs):
                old_state = job_states[jobs[i]]
                state = jobs[i].get_state()
                if result_map.has_key(state) == False:
                    result_map[state]=0
                result_map[state] = result_map[state]+1
                #print "counter: " + str(i) + " job: " + str(jobs[i]) + " state: " + state
                if old_state != state:
                    print "Job " + str(jobs[i]) + " changed from: " + old_state + " to " + state
                if old_state != state and has_finished(state)==True:
                    print "Job: " + str(jobs[i]) + " Runtime: " + str(time.time()-job_start_times[jobs[i]]) + " s."
                if has_finished(state)==True:
                    finish_counter = finish_counter + 1
                job_states[jobs[i]]=state

            print "Current states: " + str(result_map) 
            time.sleep(5)
            print " finished jobs - " + str(finish_counter) + " nbr of Partition jobs " +  str(self.__nbr_partition_jobs)
            if finish_counter == self.__nbr_map_jobs:
                print " finished jobs - " + str(finish_counter) + " nbr of Partition jobs " +  str(self.__nbr_partition_jobs)
                break
            
        self.__mjs1.cancel()

    def reduce_job_submit(self):
    ##########################################################################################        
        jobs = []
        job_start_times = {}
        job_states = {}
        part_list_argument = []
        self.__mjs2 = many_job.many_job_service(self.__reduce_resource_list, None)
                
        k = self.__tmp.replace('//','/').split('/')
        src = k[1]        
        k = self.__output_url.replace('//','/').split('/')
        dest = k[1]
        dest_url = "gridftp://"+dest+self.__output_path
        
        print " moving files .... to output location.... " + dest_url
        
        for u in self.__sorted_partition_file_names:   
            src_url = "file://"+src+ u
            print " >>> Partition to be moved ...  " + u + " >>>  from " + src_url + " to " + dest_url
            part_file =  saga.filesystem.file(src_url)
            part_file.copy(dest_url) 

        for i in range(0,self.__nbr_reduces):
            part_list_string =""
            for u in self.__sorted_partition_file_names:   
                file_name=(os.path.split(u))[1]
                if u.endswith(str(i)):
                    part_list_string = part_list_string + " " + self.__output_path + "/" + file_name     
            part_list_argument.append(part_list_string)
        
        for i in part_list_argument:
            print " The argument list is " + i
            
            # create job description
            try:
                jd2 = saga.job.description()
                jd2.executable = self.__reduce_script
                jd2.number_of_processes = "1"
                jd2.spmd_variation = "single"                
                jd2.arguments = [i]
                jd2.working_directory = "/home/pmantha"
                #jd2.output =  "/work/pmantha/output/reduce-out-" + uname
                jd2.error = "/home/pmantha/reduce-err"  
                subjob = self.__mjs2.create_job(jd2)
                subjob.run()
                print "Submited Reduce sub-job " + i + "."
                jobs.append(subjob)
                job_start_times[subjob]=time.time()
                job_states[subjob] = subjob.get_state()
                self.__nbr_reduce_jobs = self.__nbr_reduce_jobs + 1
            except:
                #traceback.print_exc(file=sys.stdout)
                print " Reduce Job failed. Cancelling bigjob......"
                self.__mjs2.cancel()
                sys.exit(0)
                try:
                    self.__mjs2.cancel()
                except:
                    pass                      
            
        print "************************ All Reduce Jobs submitted ************************"

        print " No of Reduce subjobs created - " + str( self.__nbr_reduce_jobs)
        # Wait for task completion of map tasks - synchronization      
        
        while 1: 
            finish_counter=0
            result_map = {}
            for i in range(0, self.__nbr_reduce_jobs):
                old_state = job_states[jobs[i]]
                state = jobs[i].get_state()
                if result_map.has_key(state) == False:
                    result_map[state]=0
                result_map[state] = result_map[state]+1
                #print "counter: " + str(i) + " job: " + str(jobs[i]) + " state: " + state
                if old_state != state:
                    print "Job " + str(jobs[i]) + " changed from: " + old_state + " to " + state
                if old_state != state and has_finished(state)==True:
                    print "Job: " + str(jobs[i]) + " Runtime: " + str(time.time()-job_start_times[jobs[i]]) + " s."
                if has_finished(state)==True:
                    finish_counter = finish_counter + 1
                job_states[jobs[i]]=state

            print "Current states: " + str(result_map) 
            time.sleep(5)
            print " finished jobs - " + str(finish_counter) + " nbr of reduce jobs " +  str(self.__nbr_reduce_jobs)
            if finish_counter == self.__nbr_reduce_jobs:
                print " finished jobs - " + str(finish_counter) + " nbr of reduce jobs " +  str(self.__nbr_reduce_jobs)
                break
            
        self.__mjs2.cancel() 
        

    ############################################################################################

def main():
    print " **************************  Map Reduce Framework Started ************************* "
    starttime = time.time()
    fsp = MapReduce()
    fsp.conf_check()    
    print " \n >>> All Configuration parameters provided \n"
    fsp.split()
    fsp.map_job_submit()
    runtime = time.time()-starttime
    print " >>> Map Phase completed and it took ... " + str(runtime) + "\n\n"
    starttime = time.time()
    fsp.partition_job_submit()
    runtime = time.time()-starttime
    print " >>> Partition Phase completed and it took ... " + str(runtime) + "\n\n"
    starttime = time.time()
    fsp.reduce_job_submit()
    runtime = time.time()-starttime
    print " >>> Reduce Phase completed and it took ... " + str(runtime) + "\n\n"  
    sys.exit(0)
    
if __name__=="__main__":
    main()
