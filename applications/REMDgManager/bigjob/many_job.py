#!/usr/bin/env python

"""Module many_job.

This Module is used to launch a set of bigjobs.

"""

import sys
import getopt
import saga
import time
import uuid
import pdb
import socket
import os
import traceback
import advert_job

class many_job_service():

    def __init__(self, bigjob_list, advert_host):
        """ accepts resource list as key/value pair:
            ( {"gram_url" : "gram://qb1.loni.org/jobmanager-pbs", "number_cores" : "128", "allocation" : "loni_jha_big", "queue" : "workq", "re_agent": "$(HOME)/src/REMDgManager/bigjob/advert_launcher.sh"},
              {"gram_url" : "gram://eric1.loni.org/jobmanager-pbs", "number_cores" : "64", "allocation" : "loni_jha_big", "queue" : "workq", "re_agent": "$(HOME)/src/REMDgManager/bigjob/advert_launcher.sh"})
        """        
        self.uuid = uuid.uuid1()
        
        if advert_host==None:
            self.advert_host="fortytwo.cct.lsu.edu"   
        else:
            self.advert_host=advert_host

        # list of resource dicts (1 dict per resource) 
        # will also store state of bigjob
        self.bigjob_list=bigjob_list 
       
        # state variable storing state of sub-jobs 
        self.subjob_list = []
        self.subjob_bigjob_dict = {}

        self.init_bigjobs()
     

    def init_bigjobs(self):
        """ start on specified resources a bigjob """
        self.bigjob_list = self.schedule_bigjobs()
        for i in self.bigjob_list:
            gram_url = i["gram_url"]
            print "start bigjob at: " + gram_url
            bigjob = advert_job.advert_glidin_job(self.advert_host)
            bigjob.start_glidin_job(gram_url,
                                    i["re_agent"],
                                    i["number_cores"],
                                    i["queue"],
                                    i["allocation"],
                                    "$(HOME)", 
                                    None)
            i["bigjob"]=bigjob # store bigjob for later reference in dict


    def schedule_bigjobs(self):
        """ prioritizes bigjob_list (bigjob with shortest expected delay will have index 0) """
        # no scheduling for now (start bigjob in the user specified order)
        return self.bigjob_list

    def create_job (self, job_description):
        pass 

    def schedule_subjob (self, job_description):
        """ find resource (bigjob) for subjob
            returns bigjob object """
        pass    

    def __repr__(self):
        return str(self.uuid)

    def __del__(self):
        pass
                    
                    
class sub_job():
        
    def __init__(self, bigjob_id):
        self.bigjob_id=bigjob_id

 
    def run(self):
        pass

    def get_state(self):        
        pass
    
    def cancel(self):
        pass

    def __del__(self):
        pass
    
    def __repr__(self):        
        return self.job_url


""" Test Job Submission via Advert """
if __name__ == "__main__":
    print "Test ManyJob"
    resource_list =  ( {"gram_url" : "gram://qb1.loni.org/jobmanager-pbs", "number_cores" : "128", "allocation" : "loni_jha_big", "queue" : "workq", "re_agent": "$(HOME)/src/REMDgManager/bigjob/advert_launcher.sh"},
                       {"gram_url" : "gram://eric1.loni.org/jobmanager-pbs", "number_cores" : "64", "allocation" : "loni_jha_big", "queue" : "workq", "re_agent": "$(HOME)/src/REMDgManager/bigjob/advert_launcher.sh"})
    mjs = many_job_service(resource_list, None)
    print "Created manyjob: " + str(mjs)
