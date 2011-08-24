""" 

This Module contains the API for the TROY PilotJob Framework.
TROY is an implementation of the P* Model.

"""

#
# Supported list of PilotJob backend types.
#
class PILOTJOB_TYPE():
    UNKNOWN = 0
    BIGJOB = 1
    DIANE = 2
    CONDOR = 3

#
# TROY PilotJob Description
#
class PilotJobDescription():
    """ TROY PilotJobDescription.
        
        A PilotJobDescription is a based on the SAGA Job Description.

        The PilotJobDescription is used by the application to specify 
        what kind of PilotJobs it requires.
    """
    def __init__(self):
        # Pilot / Agent description
        self.executable = None 
        self.arguments = None 
        self.cleanup = None 
        self.environment = None 
        self.interactive = None
        self.job_contact = None
        self.job_project = None
        self.job_start_time = None
        self.working_directory = None

        # I/O
        self.input = None
        self.error = None
        self.output = None
        self.file_transfer = None

        # Concurrency
        self.number_of_processes = None # Total number of processes to start
        self.processes_per_host = None # Nr of processes per host
        self.threads_per_process = None # Nr of threads to start per process
        self.total_cpu_count = None # total number of cpus requested
        self.spmd_variation = None # Type and startup mechanism

        # Requirements
  	    self.candidate_hosts = None 
        self.cpu_architecture = None
        self.total_physical_memory = None
        self.operating_system_type = None
        self.total_cpu_time = None
        self.wall_time_limit = None
        self.queue = None


#
# TROY PilotJob
#
class PilotJob():
    """ TROY PilotJob. 
    
        This is the object that is returned by the PilotJobService when a 
        new PilotJob is created based on a PilotJobDescription.

        The PilotJob object can be used by the application to keep track 
        of PilotJobs that are active.
        
        A PilotJob has state, can be queried, can be cancelled and be 
        re-initialized.
    """

    def __init__(self):
        """ Create a PilotJob object.

            Keyword arguments:
            None

            Return value:
            Handle to a PilotJob

        """
        pass

    def cancel(self):        
        """ Remove the PilotJob from the PilotJob Service.

            Keyword arguments:
        """
        pass

    def reinitialize(self, pilotjob_description):        
        """ Re-Initialize the PilotJob to the (new) PilotJobDescription.
        
            Keyword arguments:
            pilotjob_description -- A PilotJobDescription
        """
        pass

    def get_description(self):        
        """ Return the description of this PilotJob. """
        pass

    def get_state(self):        
        """ Return the state of the PilotJob. """
        pass

    def get_state_detail(self):        
        """ Return the detailed state of the PilotJob. """
        pass


#
# TROY PilotJob Service
#
class PilotJobService():
    """ TROY PilotJobService.
    
        The PilotJobService is responsible for creating and managing 
        the PilotJobs.

        It is the application's interface to the Pilot-Manager in the 
        P* Model.
        
    """

    def __init__(self):
        """ Create a PilotJobService object.

            Keyword arguments:
            None

            Return value:
            Handle to PilotJobService
        """
        pass

    def create_pilotjob(self, rm, pilotjob_desc, pj_type=None, context=None):
        """ Add a PilotJob to the PilotJobService

            Keyword arguments:
            rm -- Contact string for the resource manager
            pilotjob_desc -- PilotJob Description
            pj_type -- TROY backend type (optional)
            context -- Security context (optional)

            Return value:
            A PilotJob handle
        """
        pass

    def list(self):
        """ Return the PilotJobs that are managed by this PilotJobService.  

            Keyword arguments:
            None

            Return value:
            A list of PilotJobs.
        """
        pass



#
# TROY WorkUnitService
# 
class WorkUnitService():
    """ TROY WorkUnitService.
    
        The WorkUnitService is the application's interface to submit 
        WorkUnits to the Pilot-Manager in the P* Model.

        It can provide the application with a list of WorkUnits that are 
        managed by the Pilot-Manager.

        The WorkUnitService is linked to a PilotJobService for the actual 
        exection of the WorkUnits.
    """

    def __init__(self, pilot_service):
        """ Create a Work Service object.

            Keyword arguments:
            pilotjob_service -- The PilotJob Service to which this 
                                Work Unit Service will connect

            Return value:
            Handle to Work Service
        """
        pass

    def submit(self, wu):
        """ Submit a WU to this Work Unit Service.

            Keyword argument:
            wu -- The Work Unit from the application

            Return:
            WorkUnit object
        """
        pass

    def list(self):        
        """ Return the list of WUs that are submitted.

            Keyword argument:

            Return:
            List of WorkUnit objects
        """
        pass


#
# TROY WorkUnitDescription
# 
class WorkUnitDescription():
    """ TROY WorkUnitDescription.
    
        The WorkUnitDescription is a job/task/call description based on 
        SAGA Job Description. 
        
        It offers the application to describe a WorkUnit in an abstract 
        way that is dealt with by the Pilot-Manager.
    """

    def __init__(self):
        # Action description
        self.executable = None # The "action" to execute
        self.arguments = None # Arguments to the "action"
        self.cleanup = None # 
        self.environment = None # "environment" settings for the "action"
        self.interactive = None
        self.job_contact = None
        self.job_project = None
        self.job_start_time = None
        self.working_directory = None

        # I/O
        self.input = None
        self.error = None
        self.output = None
        self.file_transfer = None

        # Concurrency
        self.number_of_processes = None # Total number of processes to start
        self.processes_per_host = None # Nr of processes per host
        self.threads_per_process = None # Nr of threads to start per process
        self.total_cpu_count = None # Total number of cpus requested
        self.spmd_variation = None # Type and startup mechanism

        # Requirements
  	    self.candidate_hosts = None 
        self.cpu_architecture = None
        self.total_physical_memory = None
        self.operating_system_type = None
        self.total_cpu_time = None
        self.wall_time_limit = None
        self.queue = None




#
# TROY WorkUnit(WU)
# 
class WorkUnit():
    """ TROY WorkUnit.
    
        This is the object that is returned by the WorkUnitService when a 
        new WorkUnit is created based on a WorkUnitDescription.

        The WorkUnit object can be used by the application to keep track 
        of WorkUnits that are active.

        A WorkUnit has state, can be queried and can be cancelled.
    """

    def __init__(self):
        """ Create a new WorkUnit

            Keyword arguments:

            Return value:
            Handle to WorkUnit
        """
        pass

    def get_description(self):        
        """ Return the description of this WU. """
        pass

    def get_state(self):        
        """ Return the state of the WU. """
        pass

    def get_state_detail(self):        
        """ Return the detailed (application specific) state of the WU. """
        pass

    def cancel(self):
        """ Cancel the WU. """
        pass
