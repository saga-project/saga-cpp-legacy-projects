""" 

This Module contains the API for the TROY PilotJob Framework.
TROY is an implementation of the P* Model.

"""

#
# Supported list of PilotJob backend types.
#
class PILOTJOB_TYPE(object):
    UNKNOWN = 0
    BIGJOB = 1
    DIANE = 2
    CONDOR = 3

#
# TROY PilotJob Description
#
class PilotJobDescription(object):
    """ TROY PilotJobDescription.
        
        A PilotJobDescription is a based on the SAGA Job Description.

        The PilotJobDescription is used by the application to specify 
        what kind of PilotJobs it requires.
    """

    # Class members
    __slots__ = (
        # Pilot / Agent description
        'executable',
        'arguments',
        'cleanup',
        'environment',
        'interactive',
        'job_contact',
        'job_project',
        'job_start_time',
        'working_directory',
        # I/O
        'input',
        'error',
        'output',
        'file_transfer',
        # Concurrency
        'number_of_processes',      # Total number of processes to start
        'processes_per_host',       # Nr of processes per host
        'threads_per_process',      # Nr of threads to start per process
        'total_cpu_count',          # total number of cpus requested
        'spmd_variation',           # Type and startup mechanism
        # Requirements
  	    'candidate_hosts',
        'cpu_architecture',
        'total_physical_memory', 
        'operating_system_type',
        'total_cpu_time',
        'wall_time_limit',
        'queue'
    )

    def __init__(self):
        pass
    

#
# TROY PilotJob
#
class PilotJob(object):
    """ TROY PilotJob. 
    
        This is the object that is returned by the PilotJobService when a 
        new PilotJob is created based on a PilotJobDescription.

        The PilotJob object can be used by the application to keep track 
        of PilotJobs that are active.
        
        A PilotJob has state, can be queried, can be cancelled and be 
        re-initialized.
    """

    # Class members
    __slots__ = (
            'id',           # Reference to this PJ
            'description',  # Description of PilotJob
            'context',      # SAGA context
            'rm',           # Resource Manager URL
            'pj_type',      # Type of TROY backend
            'state',        # State of the PilotJob
            'state_detail'  # Adaptor specific state of the PilotJob
    )

    def cancel(self):        
        """ Remove the PilotJob from the PilotJob Service.

            Keyword arguments:
            None
        """
        pass

    def reinitialize(self, pilotjob_description):        
        """ Re-Initialize the PilotJob to the (new) PilotJobDescription.
        
            Keyword arguments:
            pilotjob_description -- A PilotJobDescription
        """
        pass



#
# TROY PilotJob Service
#
class PilotJobService(object):
    """ TROY PilotJobService.
    
        The PilotJobService is responsible for creating and managing 
        the PilotJobs.

        It is the application's interface to the Pilot-Manager in the 
        P* Model.
        
    """

    # Class members
    __slots__ = (
        'id',           # Reference to this PJS
        'state',       # Status of the PJS
        'pilot_jobs'    # List of PJs under this PJS
    )

    def __init__(self, id=None):
        """ Create a PilotJobService object.

            Keyword arguments:
            id -- Don't create a new, but connect to an existing (optional)
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

    def cancel(self):
        """ Cancel the PilotJobService.

            This also cancels all the PilotJobs that were under control of this PJS.

            Keyword arguments:
            None

            Return value:
            Result of operation
        """
        pass





#
# TROY WorkUnitService
# 
class WorkUnitService(object):
    """ TROY WorkUnitService.
    
        The WorkUnitService is the application's interface to submit 
        WorkUnits to the Pilot-Manager in the P* Model.

        It can provide the application with a list of WorkUnits that are 
        managed by the Pilot-Manager.

        The WorkUnitService is linked to a PilotJobService for the actual 
        exection of the WorkUnits.
    """

    # Class members
    __slots__ = (
        'id',                # Reference to this WUS
        'state',             # Status of the WUS
        'pilotjob_services', # PilotJobService(s) that are connected
        'work_units'         # List of WUs that have been submitted to this WUS
    )

    def __init__(self, pilot_services):
        """ Create a Work Service object.

            Keyword arguments:
            pilotjob_services -- The PilotJob Service(s) to which this 
                                Work Unit Service will connect

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

    def cancel(self):
        """ Cancel the WUS.
            
            Cancelling the WUS also cancels all the WUs submitted to it.

            Keyword arguments:
            None

            Return:
            Result
        """
        pass



#
# TROY WorkUnitDescription
# 
class WorkUnitDescription(object):
    """ TROY WorkUnitDescription.
    
        The WorkUnitDescription is a job/task/call description based on 
        SAGA Job Description. 
        
        It offers the application to describe a WorkUnit in an abstract 
        way that is dealt with by the Pilot-Manager.
    """

    # Class members
    __slots__ = (
        # Action description
        'executable',           # The "action" to execute
        'arguments',            # Arguments to the "action"
        'cleanup',
        'environment',          # "environment" settings for the "action"
        'interactive', 
        'job_contact',
        'job_project',
        'job_start_time',
        'working_directory',
        # I/O
        'input',
        'error',
        'output',
        'file_transfer',
        # Concurrency
        'number_of_processes',  # Total number of processes to start
        'processes_per_host',   # Nr of processes per host
        'threads_per_process',  # Nr of threads to start per process
        'total_cpu_count',      # Total number of cpus requested
        'spmd_variation',       # Type and startup mechanism
        # Requirements
  	    'candidate_hosts',
        'cpu_architecture',
        'total_physical_memory',
        'operating_system_type',
        'total_cpu_time',
        'wall_time_limit',
        'queue'
    )

    def __init__(self):
        pass




#
# TROY WorkUnit(WU)
# 
class WorkUnit(object):
    """ TROY WorkUnit.
    
        This is the object that is returned by the WorkUnitService when a 
        new WorkUnit is created based on a WorkUnitDescription.

        The WorkUnit object can be used by the application to keep track 
        of WorkUnits that are active.

        A WorkUnit has state, can be queried and can be cancelled.
    """

    # Class members
    __slots__ = (
        'id',               # Reference to this WU
        'description',      # Description of this WU
        'state',            # State of this WU 
        'state_detail'      # Detailed (application specific) state of this WU
    )

    def cancel(self):
        """ Cancel the WU. """
        pass
