#!/usr/bin/env python
""" 
Description: MapReduce is a programming model and an associated implementation for processing and generating large data sets.
The Below implementation is a SAGA Bigjob based python mapreduce. The SAGA bigjob is used to submit map & reduce jobs using the
url provided in the configuration file. This framework process multiple files which are mentioned in the input directory.
The files are chunked physically in the temp directory and map_partition jobs are submitted for each input chunk file.
After the process of each input chunk, based on the number of reduces mentioned in the configuration file intermediate files are 
created and 'moved' from the  temp directory to the output directory. Again reduce jobs are submmited for each intermediate 
sorted partition file. The output files are moved to the output directory mentioned in configuration file.

Currently the below framework uses one manyjob service( which is a variation of bigjob ) where multiple resources can be 
requested. Currently that is not implemented..

The files can be moved between the directories using gridftp, file or any other SAGA supported file urls. 

The implementation has to be tested 
1. against HDFS 
2. to check the scalability over multiple machines.

"""

import sys
import saga
import os
import os.path
import time
import uuid
import getopt
import many_job
import pdb

APPLICATION_NAME="Map_Reduce"
DATABASE_HOST=''

def has_finished(state):
    state = state.lower()
    if state=="done" or state=="failed" or state=="canceled":
        return True
    else:
        return False

def wait_for_all_jobs(jobs, job_start_times, job_states, poll_intervall=5):
    """ waits for all jobs that are in list to terminate """
    while 1: 
        finish_counter=0
        result_map = {}
        number_of_jobs = len(jobs)
        for i in range(0, number_of_jobs):
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
        time.sleep(poll_intervall)
        if finish_counter == number_of_jobs:
            break

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
        self.allocation=''
        self.walltime=0
        self.ppn=''
        self.optchunk=''
        
    def conf_check(self):
    
        par_conf={"advert":0, "bigjob":0, "chunk_size":0, "input":0,"temp":0, "output":0, "map_script":0,"reduce_script":0,"nbr_reduces":0}
        #par_conf={"advert":0, "bigjob":0, "chunk_size":0, "input":0,"temp":0, "output":0,"url":0, "map_script":0,"reduce_script":0,"number_cores":0, "allocation":0,"queue":0, "working_directory":0,"walltime":0}
        
        options, remainder = getopt.getopt(sys.argv[1:], 'c:' )
        config_file = ''
        
        for opt, arg in options:
            if opt in ('-c'):
                config_file = arg

        if config_file == '':
            print " Configuration file not provided.. usage : python MapReduceFramework.py -c <config_file> "
            sys.exit(0)

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
                        self.__chunksize = int(value) * 1024 * 1024
                    if par == "input":
                        self.__input_file = value
                    if par == "temp":
                        self.__tmp = value
                    if par == "output":
                        self.__output = value
                    if par == "output_url":
                        self.__output_url = value
                    if par == "map_script":
                        self.__map_script = value
                    if par == "reduce_script":
                        self.__reduce_script = value
                    if par == "ftp_protocol":
			self.ftp_protocol = value
                    if par == "nbr_reduces":
                        self.__nbr_reduces = int(value)
                    if par == "queue":
			self.queue = value
		    if par == "nbr_workers_jobs":
			self.nbr_workers_jobs = value
                    if par == "ppn":
                        self.ppn = value
                    if par == "walltime":
                       self.walltime = int(value)
                    if par == "allocation":
                       self.allocation = value          
                    if par == "bigjob":
                        sys.path.append(value)
                        self.__bigjob = value
                        self.__re_agent = value + "/bigjob_agent_launcher.sh"
		    if par == "optimum_chunk":
			self.optchunk = value
                    
                for i in range(0,1):                  
                    self.__resource_list.append( {"processes_per_node":self.ppn, "resource_url" : self.__output_url, "number_nodes" : str(self.nbr_workers_jobs),  "allocation" :self.allocation, "queue" : self.queue , "bigjob_agent": self.__re_agent, "working_directory": self.__bigjob+"/agent", "walltime":self.walltime })
                                
		self.__tmp_path = self.__tmp
                self.__tmp = "file:/" + "/".join ((self.__url.split("/"))[1:3]) + "/" + self.__tmp
                print self.__tmp + " \n"
		     
                

    def split(self):
        """ Split the file and save chunks to separate files """
        list_files=[]
        is_dir = 0
        k = saga.filesystem.file(self.__input_file)
        if ( k.is_dir() ):
            dirList =  saga.filesystem.directory(self.__input_file)
            print " Total number of input files in directory " + self.__input_file + " is " + str(len(dirList.list()))
            list_files =  dirList.list()
            is_dir = 1
        else:
	    fname = self.__input_file
            list_files.append(fname)
        
        for fname in list_files:
            dname = os.path.split(self.__input_file)[1]
            bname = os.path.split(self.__input_file)[0]
            if is_dir :
            	fname = bname + "/" + dname + "/" + str(fname)
            print ' >>> Splitting file - '  + str(fname) + "\n"
            try:
                f = saga.filesystem.file(fname)
            except:
                print " File not opened "
                sys.exit(0)
                pass
        
            bname = (os.path.split(str(f.get_url())))[1]

            # Get the file size
            fsize = f.get_size()

            # Get size of each chunk
            self.__numchunks = int(float(fsize)/float(self.__chunksize))
            if int(float(fsize)%float(self.__chunksize)) > 0:
                 self.__numchunks = self.__numchunks + 1
            if self.__numchunks == 0:
                if fsize > 0:
                    self.__numchunks = 1
             
        
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
		    k = self.__tmp + "/" + chunkfilename
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
        jobs = []
        job_start_times = {}
        job_states = {}

        print " >>> Create manyjob service with advert service at ... " , self.advert_host + "\n"
        
        self.__mjs = many_job.many_job_service(self.__resource_list, self.advert_host)
        
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
                jd.arguments = [temp_abs_path , str(self.__nbr_reduces)]
                jd.working_directory = self.__tmp_path
                jd.output = self.__bigjob + "/agent/stdout-" + uname + ".txt"
                jd.error = self.__bigjob + "/agent/stderr-" + uname + ".txt"
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
        
        ############################################################################################
        # Wait for task completion of map tasks - synchronization    
        wait_for_all_jobs(jobs, job_start_times,job_states, 5)
        ############################################################################################
        

    def reduce_job_submit(self):
    ##########################################################################################        
        part_list_argument = []
        jobs = []
        job_start_times = {}
        job_states = {}

        k = self.__tmp.replace('//','/').split('/')
        src = k[1]        
        k = self.__output_url.replace('//','/').split('/')
        dest = k[1]
        dest_url = self.ftp_protocol + "://"+dest+self.__output
        
        print " moving files .... to output location.... " + dest_url
        file_transfer_time = time.time()
        for u in self.__chunk_list:
            for i in range(self.__nbr_reduces):
                self.__sorted_partition_file_names.append( u + "sorted-part-" + str(i) )
            
        file_transfer_size = 0    
        for u in self.__sorted_partition_file_names:               
            print " >>> Partition to be moved ...  " + u + " to " + dest_url
            part_file =  saga.filesystem.file(u)
            file_transfer_size = file_transfer_size + part_file.get_size()
            part_file.move(dest_url,saga.filesystem.Overwrite) 
        file_transfer_comp = time.time() - file_transfer_time
        print " Time taken to transfer partition files to output location " + str( round(file_transfer_comp,3)) + "\n\n"
        print " The total file size transferred is : " + str ( round(file_transfer_size,3) ) + "\n\n"

        for i in range(0,self.__nbr_reduces):
            part_list_string =""
            for u in self.__sorted_partition_file_names:   
                file_name=(os.path.split(u))[1]
                if u.endswith(str(i)):
                    part_list_string = part_list_string + ":" + self.__output + "/" + file_name     
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
                jd2.working_directory = self.__output
                jd2.error = self.__bigjob + "/agent/reduce-err" + str(self.__nbr_reduce_jobs)  
                subjob = self.__mjs.create_job(jd2)
                subjob.run()
                print "Submited Reduce sub-job " + i + "."
                jobs.append(subjob)
                job_start_times[subjob]=time.time()
                job_states[subjob] = subjob.get_state()
                self.__nbr_reduce_jobs = self.__nbr_reduce_jobs + 1
            except:
                #traceback.print_exc(file=sys.stdout)
                print " Reduce Job failed. Cancelling bigjob......"
                self.__mjs.cancel()
                sys.exit(0)
                try:
                    self.__mjs.cancel()
                except:
                    pass                      
            
        print "************************ All Reduce Jobs submitted ************************"

        print " No of Reduce subjobs created - " + str( self.__nbr_reduce_jobs)
        # Wait for task completion of map tasks - synchronization      
        
        ############################################################################################
        # Wait for task completion of map tasks - synchronization    
        wait_for_all_jobs(jobs, job_start_times, job_states,  5)
        ############################################################################################
            
        self.__mjs.cancel() 
        

    ############################################################################################

def main():
    print " **************************  Map Reduce Framework Started ************************* "
    initial_time = time.time()
    fsp = MapReduce()
    fsp.conf_check()    
    print " \n >>> All Configuration parameters provided \n"
    starttime = time.time()
    fsp.split()
    runtime = time.time() -starttime
    print " \n Time taken to chunk : " + str(round(runtime,3)) +  "\n\n";
    starttime = time.time()
    fsp.map_job_submit()
    runtime = time.time()-starttime
    print " >>> Map Phase completed and it took ... " + str(round(runtime,3)) + "\n\n"
    starttime = time.time()
    fsp.reduce_job_submit()
    runtime = time.time()-starttime
    print " >>> Reduce Phase completed and it took ... " + str(round(runtime,3)) + "\n\n"  
    final_time = time.time() - initial_time
    print " >>> The total time taken is " + str(round(final_time,3)) + "\n\n"
    sys.exit(0)
    
if __name__=="__main__":
    main()

