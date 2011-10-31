import sys
import getopt
import saga
import time
import pdb
import os
import traceback
import logging


from bigjob.bigjob_manager import *
from bigjob_dynamic.many_job import *

from bigdata.troy.compute.api import PilotJob, PilotJobService

""" This variable defines the coordination system that is used by BigJob
    e.g. 
        advert://localhost (SAGA/Advert SQLITE)
        advert://advert.cct.lsu.edu:8080 (SAGA/Advert POSTGRESQL)
        redis://localhost:6379 (Redis at localhost)
        tcp://localhost (ZMQ)
"""
COORDINATION_URL = "advert://localhost"


class PilotJobService(PilotJobService):
    """ TROY PilotJobService based on BigJob.
                    
    """

    # Class members
    __slots__ = (
        'id',           # Reference to this PJS
        'state',       # Status of the PJS
        'pilot_jobs',    # List of PJs under this PJS
        '__mjs'
    )

    def __init__(self, pjs_id=None):
        """ Create a PilotJobService object.

            Keyword arguments:
            pjs_id -- Don't create a new, but connect to an existing (optional)
        """
        self.__mjs = None
        self.pilot_jobs=[]

    def create_pilotjob(self, rm=None, pilot_job_description=None, pj_type=None, context=None):
        """ Add a PilotJob to the PilotJobService

            Keyword arguments:
            pilot_job_description -- PilotJob Description
            
            Return value:
            A PilotJob handle
        """
        
        if self.__mjs == None:
            logging.debug("Create Dynamic BigJob Service")            
            self.__mjs = many_job_service([], COORDINATION_URL)
            
        resource_description = self.__translate_pj_bj_description(pilot_job_description)
        bigjob = self.__mjs.add_resource(resource_description)
        pj = PilotJob(bigjob)
        self.pilot_jobs.append(pj)
        return pj
        
    def __translate_pj_bj_description(self, pilot_job_description):
        resource_description={}
        if pilot_job_description.has_key("service_url"):
            resource_description["resource_url"] = pilot_job_description["service_url"] 
            
        if pilot_job_description.has_key("queue"):
            resource_description["queue"] = pilot_job_description["queue"] 
        else:
            resource_description["queue"] = None
            
        if pilot_job_description.has_key("allocation"):
            resource_description["allocation"] = pilot_job_description["allocation"] 
        else:
            resource_description["allocation"] = None
        
        for i in pilot_job_description.keys():
            resource_description[i] = pilot_job_description[i] 
        
        return resource_description
    
    def list_pilotjobs(self):
        return self.__mjs.get_resources()
        
    
    def cancel(self):
        """ Cancel the PilotJobService.

            This also cancels all the PilotJobs that were under control of this PJS.

            Keyword arguments:
            None

            Return value:
            Result of operation
        """
        self.__mjs.cancel()
        
    
    def _submit_wu(self, work_unit):
        subjob = self.__mjs.create_job(work_unit.subjob_description)
        subjob.run()
        work_unit.subjob=subjob
        return work_unit
    
    
class PilotJob(PilotJob):
    """ TROY Wrapper for bigjob class """
     
    def __init__(self, bigjob):
        self.__bigJob = bigjob
        
    def cancel(self):
        self.__bigjob.cancel()
    
    def get_state(self):
        return self.__bigjob.get_state()