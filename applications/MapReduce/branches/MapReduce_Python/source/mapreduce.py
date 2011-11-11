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
"""

import saga
import os
import time
import pdb
import os.path
import uuid
import getopt
import sys
import glob
sys.path.insert(0, os.getcwd() + "/../")
from bigjob.bigjob_manager import bigjob, subjob
sys.path.append(os.path.dirname(__file__))
from mrfunctions import *

APPLICATION_NAME="Map_Reduce"

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
            if result_map.has_key(state)==False:
                result_map[state]=1
            else:
                result_map[state] = result_map[state]+1
            if old_state != state:
                print "Job " + str(jobs[i]) + " changed from: " + old_state + " to " + state
            if old_state != state and has_finished(state)==True:
                print "Job: " + str(jobs[i]) + " Runtime: " + str(time.time()-job_start_times[jobs[i]]) + " s."
            if has_finished(state)==True:
                finish_counter = finish_counter + 1
            job_states[jobs[i]]=state

        if finish_counter == number_of_jobs:
            break
        time.sleep(2)

class MapReduce:
    def __init__(self,input_dir,output_dir,tmp_dir,nbr_reduces,mapper,reducer,advert_host,resource_url,number_nodes,queue, allocation, walltime, ppn, workingdirectory, userproxy,chunk,npworkers, maparg=None, redarg=None):

        # cache filename
        self.__input_dir= input_dir
        # Output
        self.__output_dir= output_dir
        # Temp
        self.__tmp_dir = tmp_dir
        # Number of reduces
        self.__nbr_reduces = int(nbr_reduces)
        # mapper file
        self.__mapper = mapper
        # reducer file
        self.__reducer = reducer
        
        #bigjob parameters
        self.advert_host = advert_host
        self.resource_url = resource_url
        self.number_of_processes = int(number_nodes)
        self.queue = None
        self.allocation = None
        self.walltime = int(walltime)
        self.ppn = int(ppn)
        self.workingdirectory = workingdirectory
        self.userproxy = None
        self.chunk=int(chunk)
        self.npworkers = npworkers
        if maparg != None:
            self.maparg=maparg.split(",")
        else:
            self.maparg=[]
        if redarg != None:
            self.redarg=redarg.split(",")
        else:
            self.redarg=[]
        
        
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
        self.__bj=''

    def chunk_input(self):
        mrf = mrfunctions(self.__input_dir,  self.__tmp_dir )
        self.__chunk_list = mrf.chunk_size(self.chunk)
        print self.__chunk_list

    def chunk_lines(self):
        mrf = mrfunctions(self.__input_dir,  self.__tmp_dir )
        self.__chunk_list = mrf.fastq_chunk(self.chunk)
        print self.__chunk_list
        
    
    def map_job_submit(self):
    ##########################################################################################
        print " >>> Starting BigJob ..................... \n"
        jobs = []
        job_start_times = {}
        job_states = {}
        print " >>> Create bigjob with advert service at ... " , self.advert_host + "\n"
        
        print " >> BigJob parameters " + self.advert_host + "\n" 
        print " >> resource url " + self.resource_url + "\n"
        print " >> Number of processes " +  str(self.number_of_processes) + "\n" 
        print " >> Queue " + str(self.queue) + "\n" 
        print " >> Allocation " +  str(self.allocation) + "\n" 
        print " >> Working directory" + self.workingdirectory + "\n" 
        print " >> userproxy " + str(self.userproxy) + "\n" 
        print " >> walltime " + str(self.walltime) + "\n" 
        print " >> ppn " + str(self.ppn) + "\n"
                                  
        self.__bj = bigjob(self.advert_host) 
        self.__bj.start_pilot_job( self.resource_url, None, self.number_of_processes, self.queue, self.allocation, 
                                   self.workingdirectory, self.userproxy, self.walltime, self.ppn)
        i=0
        for u in self.__chunk_list:          
            uname="-".join(u)
            i=i+1
            print " >>> chunk path/name to be submitted to map subjob  " + uname

            # create job description
            try:
                
                jd = saga.job.description()
                jd.executable = self.__mapper
                jd.number_of_processes = self.npworkers
                jd.spmd_variation = "single"                
                jd.arguments = u + [str(self.__nbr_reduces)] + self.maparg
                jd.working_directory = saga.url(self.__tmp_dir).path
                jd.output = "stdout-map" + str(i) + ".txt"
                jd.error = "stderr-map" + str(i) + ".txt"
                sj = subjob()
                sj.submit_job(self.__bj.pilot_url, jd)
                print "Submited sub-job " + uname + "."
                jobs.append(sj)
                job_start_times[sj]=time.time()
                job_states[sj] = sj.get_state()
            except:
                #traceback.print_exc(file=sys.stdout)
                print " Map Job failed. Cancelling bigjob......"
                self.__bj.cancel()
                sys.exit(0)
                try:
                    self.__bj.cancel()
                except:
                    pass  
            
        print "************************ All Jobs submitted ************************"
        print " No of map subjobs created - " + str(len(jobs))
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

        print " moving files .... to output location.... " + saga.url(self.__output_dir).path
        file_transfer_time = time.time()
        self.__sorted_partition_file_names = glob.glob(saga.url(self.__tmp_dir).path +  "/*sorted-part-*")
            
        file_transfer_size = 0   
        for u in self.__sorted_partition_file_names:               
            #print " >>> Partition to be moved ...  " + u + " to " + self.__output_dir
            part_file =  saga.filesystem.file(u)
            file_transfer_size = file_transfer_size + part_file.get_size()
            part_file.move(self.__output_dir) 
        
        file_transfer_comp = time.time() - file_transfer_time
        print " Time taken to transfer partition files to output location " + str( round(file_transfer_comp,3)) + "\n\n"
        print " The total file size transferred is(MB): " + str ( round((file_transfer_size/(1024.0 * 1024.0)),3) ) + "\n\n"

        for i in range(0,self.__nbr_reduces):
            part_list_string =""
            for u in self.__sorted_partition_file_names:   
                file_name=(os.path.split(u))[1]
                if str(u.split("-")[-1:][0]) == str(i):
                    part_list_string = part_list_string + ":" + saga.url(self.__output_dir).path + "/" + file_name     
            part_list_argument.append(part_list_string)
        
        for i in part_list_argument:
            print " The argument list is " + i
            # create job description
            try:
                jd2 = saga.job.description()
                jd2.executable = self.__reducer
                jd2.number_of_processes = self.npworkers
                jd2.spmd_variation = "single"                
                jd2.arguments = [i] + self.redarg
                jd2.working_directory = saga.url(self.__output_dir).path
                jd2.output = "stdout_reduce" + str(self.__nbr_reduce_jobs) 
                jd2.error =  "stderr-reduce" + str(self.__nbr_reduce_jobs)  
                sj = subjob()
                sj.submit_job(self.__bj.pilot_url, jd2)
                print "Submited Reduce sub-job " + str(self.__nbr_reduce_jobs)
                jobs.append(sj)
                job_start_times[sj]=time.time()
                job_states[sj] = sj.get_state()
                self.__nbr_reduce_jobs = self.__nbr_reduce_jobs + 1
            except:
                #traceback.print_exc(file=sys.stdout)
                print " Reduce Job failed. Cancelling bigjob......"
                self.__bj.cancel()
                sys.exit(0)
                try:
                    self.__bj.cancel()
                except:
                    pass                      
            
        print "************************ All Reduce Jobs submitted ************************"

        print " Reduce subjobs created "
        # Wait for task completion of map tasks - synchronization      
        
        ############################################################################################
        # Wait for task completion of map tasks - synchronization    
        wait_for_all_jobs(jobs, job_start_times, job_states,  5)
        ############################################################################################
            
        self.__bj.cancel() 
        
