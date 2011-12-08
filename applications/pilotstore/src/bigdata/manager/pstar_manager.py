import sys
import os
import time
import threading
import logging
import pdb
import Queue
import saga
import uuid
import traceback

from bigdata.troy.api import WorkDataService
from bigdata.troy.compute.api import WorkUnit, State
from bigdata.manager.pilotdata_manager import PilotData


""" Loaded Module determines scheduler:
    
    bigdata.scheduler.data_compute_scheduler - selects random locations for PD and WUs
    bigdata.scheduler.data_compute_affinity_scheduler - considers affinity descriptions
    
"""

from bigdata.scheduler.data_compute_affinity_scheduler import Scheduler

class WorkDataService(WorkDataService):
    """ TROY WorkDataService.
    
        The WorkDataService is the application's interface to submit 
        WorkUnits and PilotData/DataUnit to the Pilot-Manager 
        in the P* Model.
    """    

    def __init__(self, wds_id=None):
        """ Create a Work Data Service object.

            Keyword arguments:
            wds_id -- Reconnect to an existing WDS (optional).
        """
        self.id=uuid.uuid1()
        
        # Pilot Data
        self.pilot_data={}
        self.pilot_store_services=[]
        self.pd_queue = Queue.Queue()
        
        # Pilot Job
        self.pilot_job_services=[]
        self.work_units={}
        self.scheduler = Scheduler()
        
        # Background Thread for scheduling
        self.wu_queue = Queue.Queue()
        self.stop=threading.Event()
        self.scheduler_thread=threading.Thread(target=self._scheduler_thread)
        self.scheduler_thread.start()

    ###########################################################################
    # Pilot Job
    
    def add_pilot_job_service(self, pjs):
        """ Add a PilotJobService to this WUS.

            Keyword arguments:
            pilotjob_services -- The PilotJob Service(s) to which this 
                                 Work Unit Service will connect.

            Return:
            Result
        """
        self.pilot_job_services.append(pjs)


    def remove_pilot_job_service(self, pjs):
        """ Remove a PilotJobService from this WUS.

            Note that it won't cancel the PilotJobService, it will just no
            longer be connected to this WUS.

            Keyword arguments:
            pilotjob_services -- The PilotJob Service(s) to remove from this
                                 Work Unit Service. 

            Return:
            Result
        """
        self.pilot_job_services.remove(pjs)


    def submit_work_unit(self, work_unit_description):
        """ Submit a WU to this Work Unit Service.

            Keyword argument:
            wud -- The WorkUnitDescription from the application

            Return:
            WorkUnit object
        """
        wu = WorkUnit(work_unit_description)
        self.work_units[wu.id]=wu
        self.wu_queue.put(wu)
        return wu
    
    ###########################################################################
    # Pilot Data 
    
    def add_pilot_store_service(self, pss):
        """ Add a PilotStoreService 

            Keyword arguments:
            pss -- The PilotStoreService to add.

            Return:
            None
        """
        self.pilot_store_services.append(pss)

    
    def remove_pilot_store_service(self, pss):

        """ Remove a PilotStoreService 
            
            Keyword arguments:
            pss -- The PilotStoreService to remove 
            
            Return:
            None
        """
        self.pilot_store_services.remove(pss)
    
    
    def list_pilotstores(self):
        """ List all PDs of PDS """
        return self.pilot_store_services
    
    
    def list_pilotdata(self):
        """ List all PDs of PDS """
        return self.pilot_data.items()
    
    
    def get_pilotdata(self, pd_id):
        if self.pilot_data.has_key(pd_id):
            return self.pilot_data[pd_id]
        return None
    
    
    def submit_pilot_data(self, pilot_data_description):
        """ creates a pilot data object and binds it to a physical resource (a pilotstore) """
        pd = PilotData(self, pilot_data_description)
        self.pilot_data[pd.id]=pd
        self.pd_queue.put(pd)
        return pd
    
    def cancel(self):
        """ Cancel the PDS. 
            All associated PD objects are deleted and removed from the associated pilot stores.            
            
            Keyword arguments:
            None

            Return:
            None
        """
        # terminate background thread
        self.stop.set()
   
    ###########################################################################
    # Internal Scheduling
    def __update_scheduler_resources(self):
        logging.debug("__update_scheduler_resources")        
        ps = [s for i in self.pilot_store_services for s in i.list_pilotstores()]
        self.scheduler.set_pilot_stores(ps)
        pj = [p for i in self.pilot_job_services for p in i.list_pilotjobs()]
        logging.debug("Pilot-Jobs: " + str(pj))
        self.scheduler.set_pilot_jobs(pj)
    
    def _schedule_pd(self, pd):
        """ Schedule PD to a suitable pilot store
        
            Currently one level of scheduling is used:
                1.) Add all resources managed by PSS of this PSS
                2.) Select one resource
        """ 
        logging.debug("Schedule PD")
        self.__update_scheduler_resources()
        selected_pilot_store = self.scheduler.schedule_pilot_data()
        return selected_pilot_store 
    
    def _schedule_wu(self, wu):
        logging.debug("Schedule PD")
        self.__update_scheduler_resources()
        selected_pilot_job = self.scheduler.schedule_pilot_job(wu.work_unit_description)
        return selected_pilot_job
    
    def _scheduler_thread(self):
        while True and self.stop.isSet()==False:            
            try:
                logging.debug("Scheduler Thread: " + str(self.__class__) + " Pilot Data")
                pd = self.pd_queue.get(True, 1)  
                # check whether this is a real pd object  
                if isinstance(pd, PilotData):
                    ps=self._schedule_pd(pd)                
                    if(ps!=None):
                        ps.put_pd(pd)
                        logging.debug("Transfer to PS finished.")
                        pd.update_state(State.Running)
                        pd.add_pilot_store(ps)                    
                    else:
                        self.pd_queue.put(pd)
            except Queue.Empty:
                pass
                    
            try:    
                logging.debug("Scheduler Thread: " + str(self.__class__) + " Pilot Job")
                wu = self.wu_queue.get(True, 1)
                if isinstance(wu, WorkUnit):
                    pj=self._schedule_wu(wu) 
                    if pj !=None:
                        pj._submit_wu(wu)                    
                    else:
                        self.wu_queue.put(pd)
            except Queue.Empty:
                pass
            except:
                exc_type, exc_value, exc_traceback = sys.exc_info()
                print "*** print_tb:"
                traceback.print_tb(exc_traceback, limit=1, file=sys.stdout)
                print "*** print_exception:"
                traceback.print_exception(exc_type, exc_value, exc_traceback,
                              limit=2, file=sys.stdout)
            time.sleep(5)        

        logging.debug("Re-Scheduler terminated")
    
    
    def get_state(self):
        return self.state
    
    
    
    def get_id(self):
        return str(self.id)
    
    
class WorkUnit(WorkUnit):
    """ WorkUnit - Wrapper for BigJob subjob """

    def __init__(self, work_unit_description):
        self.id = uuid.uuid1()
        self.state = State.New       
        self.subjob = None # reference to BigJob Subjob 
        self.work_unit_description = work_unit_description # WU Description
        self.subjob_description = self.__translate_wu_sj_description(work_unit_description)
                
    def get_id(self):
        return self.id
    
    def get_state(self):
        if self.subjob != None:
            return self.subjob.get_state()
        return self.state


    def cancel(self):
        if self.subjob != None:
            return self.subjob.cancel()
        return None


    # INTERNAL
    def __translate_wu_sj_description(self, work_unit_description):
        jd = saga.job.description()
        if work_unit_description.has_key("executable"): 
            jd.executable = work_unit_description["executable"]
        jd.spmd_variation = "single"
        if work_unit_description.has_key("arguments"): 
            jd.arguments = work_unit_description["arguments"]
        
        if work_unit_description.has_key("number_of_processes"):
            jd.number_of_processes=str(work_unit_description["number_of_processes"])
        else:
            jd.number_of_processes="1"
        
        if work_unit_description.has_key("working_directory"): 
            jd.working_directory = work_unit_description["working_directory"]
        if work_unit_description.has_key("output"): 
            jd.output =  work_unit_description["output"]
        if work_unit_description.has_key("stderr"): 
            jd.error = work_unit_description["stderr"]
        return jd
        
