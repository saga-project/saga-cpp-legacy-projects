# Page 164 package saga.job

from object import Object, ObjectType
from attributes import Attributes
from task import Async, Task
from monitoring import Steerable
from permissions import Permissions
from error import NotImplemented

class State(object):
    """
    The state is equivalent to the inherited saga.task.State, but adds the SUSPENDED state:

    status: This object is not completly specified yet.
    ===================================================    
    """
    NEW       =  1
    RUNNING   =  2
    DONE      =  3
    CANCELED =  4
    FAILED    =  5
    SUSPENDED =  6

class JobDescription(Object,Attributes):
    """
    This object encapsulates all the attributes which define a job to be run. It has
    no methods of its own, but implements the saga::attributes interface in order
    to provide access to the job properties, which are expressed as JSDL keywords.

    status: This object is not completly specified yet.
    ===================================================
    
    B{Attributes:}
    
        - B{Executable}
            - name: Executable
            - desc: command to execute.
            - type: String
            - mode: ReadWrite
            - value: ""
            - notes: 
                - this is the only required attribute.
                - can be a full pathname, or a pathname relative to the 'WorkingDirectory' as evaluated on the execution host.
                - semantics as defined in JSDL
                - available in JSDL, DRMAA
        - B{Arguments}
            - name:  Arguments
            - desc:  positional parameters for the command.
            - mode:  ReadWrite, optional
            - type:  Vector String B{(need update TODO )}
            - value: -
            - notes: 
               - semantics as specified by JSDL
               - available in JSDL, DRMAA
        - B{SPMDVariation}
            - name:  SPMDVariation
            - desc:  SPMD job type and startup mechanism
            - mode:  ReadWrite, optional
            - type:  String
            - value: -
            - notes: 
                - the SPMD JSDL extension defines the value to be an URI. For simplicity, SAGA allows
                  the following strings, which map into the respective URIs: MPI, GridMPI, IntelMPI,
                  LAM-MPI, MPICH1, MPICH2, MPICH-GM, MPICH-MX, MVAPICH, MVAPICH2, OpenMP, POE, PVM, None
            - the value '' (no value, default) indicates that the application is not a SPMD application.
            - as JSDL, SAGA allows other arbitrary values.
        - B{TotalCPUCount}
            - name:  TotalCPUCount
            - desc:  total number of cpus requested for this job mode:  ReadWrite, optional
            - type:  Int
            - value: '1'
            - notes: 
                - semantics as defined in JSDL.
                - available in JSDL, DRMAA
        - B{NumberOfProcesses}
            - name:  NumberOfProcesses
            - desc:  total number of processes to be started
            - mode:  ReadWrite, optional
            - type:  Int
            - value: '1'
            - notes: 
               - semantics as defined in JSDL
               - available in JSDL, SPMD extension
        - B{ProcessesPerHost}
            - name:  ProcessesPerHost
            - desc:  number of processes to be started per host
            - mode:  ReadWrite, optional
            - type:  Int
            - value: '1'
            - notes: 
                - semantics as defined in JSDL
                - available in JSDL, SPMD extension
        - B{ThreadsPerProcess}
            - name:  ThreadsPerProcess
            - desc:  number of threads to start per process
            - mode:  ReadWrite, optional
            - type:  Int
            - value: '1'
            - notes: 
                - semantics as defined in JSDL
                - available in JSDL, SPMD extension
        - B{Environment}
            - name:  Environment
            - desc:  set of environment variables for the job
            - mode:  ReadWrite, optional
            - type:  Vector String B{Need Update TODO }
            - value: -
            - notes: 
                - exported into the job environment
                - format: 'key=value'
                - semantics as specified by JSDL
                - available in JSDL, DRMAA
        - B{WorkingDirectory}
            - name:  WorkingDirectory
            - desc:  working directory for the job
            - mode:  ReadWrite, optional
            - type:  String
            - value: '.'
            - note: semantics as specified by JSDL
            - available in JSDL, DRMAA
        - B{Interactive}
            - name:  Interactive
            - desc:  run the job in interactive mode
            - mode:  ReadWrite, optional
            - type:  Bool
            - value: 'False'
            - notes: 
                - this implies that stdio streams will stay connected to the submitter after job submission, and during job execution.
                - if an implementation cannot handle interactive jobs, and this attribute is present, and 'True', the job creation MUST
                  raise an 'IncorrectParameter' error with a descriptive error message.
                - not supported by JSDL, DRMAA
        - B{Input}
            - name:  Input
            - desc:  pathname of the standard input file
            - mode:  ReadWrite, optional
            - type:  String
            - value: -
            - notes: 
                - semantics as specified by JSDL
                - available in JSDL, DRMAA
                - will not be used if 'Interactive' is 'True'
        - B{Output}
            - name:  Output
            - desc:  pathname of the standard output file
            - mode:  ReadWrite, optional
            - type:  String
            - value: -
            - notes: 
                - semantics as specified by JSDL
                - available in JSDL, DRMAA
                - will not be used if 'Interactive' is 'True'
        - B{Error}
            - name:  Error
            - desc:  pathname of the standard error file
            - mode:  ReadWrite, optional
            - type:  String
            - value: -
            - notes: 
                - semantics as specified by JSDL
                - available in JSDL, DRMAA
                - will not be used if 'Interactive' is 'True'
        - B{FileTransfer}
            - name:  FileTransfer
            - desc:  a list of file transfer directives
            - mode:  ReadWrite, optional
            - type:  Vector String B{need Update TODO}
            - value: -
            - notes: 
                - translates into jsdl:DataStaging
                - used to specify pre- and post-staging
                - semantics as specified in JSDL
                - staging is part of the 'Running' state
                - syntax similar to LSF (see earlier notes)
                - available in JSDL, DRMAA
        - B{Cleanup}
            - name: Cleanup
            - desc: defines if output files get removed after the job finishes
            - mode: ReadWrite, optional
            - type: String
            - value: 'Default'
            - notes: 
                - can have the Values 'True', 'False', and 'Default'
                - On 'False', output files MUST be kept after job the finishes
                - On 'True', output files MUST be deleted after job the finishes
                - On 'Default', the behaviour is defined by the implementation or the backend.
                - translates into 'DeleteOnTermination' elements in JSDL
        - B{JobStartTime}
            - name:  JobStartTime
            - desc:  time at which a job should be scheduled
            - mode:  ReadWrite, optional
            - type:  Int
            - value: -
            - notes: 
                - Could be viewed as a desired job start time, but that is up to the resource manager.
                - format: number of seconds since epoch
                - available in DRMAA
                - not supported by JSDL
        - B{TotalCPUTime}
            - name:  TotalCPUTime
            - desc:  estimate total number of CPU seconds which the job will require.
            - mode: ReadWrite, optional
            - type: Int
            - value: -
            - notes: 
                - intended to provide hints to the scheduler for scheduling policies.
                - available in JSDL, DRMAA
                - semantics as defined in JSDL
        - B{TotalPhysicalMemory}
            - name:  TotalPhysicalMemory
            - desc:  Estimated amount of memory the job requires
            - mode:  ReadWrite, optional
            - type: Float
            - value: -
            - notes: 
                - unit is in MegaByte
                - memory usage of the job is aggregated across all processes of the job
                - semantics as defined by JSDL
                - available in JSDL
        - B{CPUArchitecture}
            - name:  CPUArchitecture
            - desc:  compatible processor for job submission
            - mode:  ReadWrite, optional
            - type:  Vector String B{Need Update TODO}
            - value: -
            - notes: 
                - allowed values as specified in JSDL
                - semantics as defined by JSDL
                - available in JSDL
        - B{OperatingSystemType}
            - name:  OperatingSystemType
            - desc:  compatible operating system for job submission
            - mode:  ReadWrite, optional
            - type:  Vector String B{Need Update TODO}
            - value: -
            - notes: 
                - allowed values as specified in JSDL
                - semantics as defined by JSDL
                - available in JSDL
        - B{CandidateHosts}
            - name:  CandidateHosts
            - desc:  list of host names which are to be considered by the resource manager as candidate targets
            - mode: ReadWrite, optional
            - type: Vector String B{Need Update TODO}
            - value: -
            - notes: 
                - semantics as defined by JSDL
                - available in JSDL
        - B{Queue}
            - name:  Queue
            - desc:  name of a queue to place the job into
            - mode:  ReadWrite, optional
            - type:  String
            - value: -
            - notes: 
                - While SAGA itself does not define the semantics of a "queue", many backend systems can make use of this attribute.
                - not supported by JSDL
        - B{JobContact}
            - name: JobContact
            - desc: set of endpoints describing where to report job state transitions.
            - mode: ReadWrite, optional
            - type: Vector String B{Need Update TODO}
            - value: -
            - notes: 
                - format: URI (e.g. fax:+123456789, sms:+123456789, mailto:joe@doe.net).
                - available in DRMAA
                - not supported by JSDL    
                
    """

    def __init__(self):
        """ 
        Initialize the object
        @summary: initialize the object
        @Raise NotImplemented:
        @raise NoSuccess:
        @Note:    a JobDescription is not associated with a Session, 
            but can be used for JobServices from different sessions.
        """
        super(JobDescription, self).__init__()
          
class JobService(Object, Async):
    """
    The job_service represents a resource management backend, and as such allows to create and submit jobs, and to discover jobs.
    
    status: This object is not completly specified yet.
    ===================================================
    """

    def __init__(self, session, url = ""):
        """
        Create the object
        session: session to associate with the object
        url: contact url for resource manager
        @raise NotImplemented:
        @raise IncorrectURL:
        @raise PermissionDenied:
        @raise AuthorizationFailed:
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @Note: 'url' defaults to an empty string - in that case, the implementation must perform a  resource discovery, 
               or fall back to a fixed value, or find a valid rm contact in any other way. If that is not possible,
               a BadParameter exception MUST be raised, and MUST indicate that a rm contact string is needed.
        @Note: if the rm identified by the url cannot be contacted (i.e. does not exist), a BadParameter exception is raised.

        """

    def create_job(self, jd):
        #in JobDescription jd, out job job
        """
        Create a job instance
        jd: description of job to be submitted
        job: a job object representing the submitted job instance
        @PreCondition:    jd has an 'Executable' attribute.
        @PostCondition: job is in 'New' state
        @PostCondition:  jd is deep copied (no state is shared after method invocation)
        @PostCondition: 'Owner' of the job is the id of the context used for creating the job.
        @raise NotImplemented:
        @raise BadParameter:
        @raise PermissionDenied:
        @raise AuthorizationFailed:
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @Note: calling run() on the job will submit it to the resource, and advance its state.
        @Note: if the job description does not have a valid 'Executable' attribute, a BadParameter exception is raised.
        @Note: if the job description contains values which are outside of the allowed range, or cannot be parsed, 
                  or are otherwise invalid and not usable for creating a job instance, a BadParameter exception is raised, 
                  which MUST indicate which attribute(s) caused this exception, and why.

        """
        
        
    def run_job(self, commandline, host = ""):
        #in string commandline, in string host = "", out job job, out opaque stdin, out opaque stdout, out opaque stderr
        """
        Run a command synchronously.
        commandline: the command and arguments to be run
        host: hostname to be used by rm for submission
        stdin: IO handle for the running job's standard input stream
        stdout: IO handle for the running job's standard output
        stderr: IO handle for the running job's standard error
        job: a job object representing the submitted job instance
        @PostCondition: job is in 'Running', 'Done' or 'Failed' state.
        @PostCondition: 'Owner' of the job is the id of the context used for creating the job.
        @raise NotImplemented:
        @raise BadParameter:
        @raise PermissionDenied:
        @raise AuthorizationFailed:
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @Note: This is a convenience routine built on the create_job method, and is intended to simplify
                  the steps of creating a job_description, creating and running the job, and then
                  querying the standard I/O streams.
        @Note: the I/O handles have to be passed to the call as references, in most languages, as calls
                  often allow only one return value (perl or python being notable exceptions). If these
                  parameters are omitted, the job is to be started non-interactively, and the output I/O streams may be discarded.
        @Note:  the job is guaranteed to run on the given host, or not at all.
        @Note:  the method is exactly equivalent to the sequence of (1) creation of a job_description with 'Executable' set to the values from the
             commandline, 'Interactive' set if I/O is requested, 'CandidateHost' set to host; (2)
             create_job() with that description; (3) calling run() on that job. This method can
             raise any of the exceptions which can occur in this sequence, with the semantics defined in
             the detailed description of the methods used in this sequence. No other exception are to be expected.
        @Note:  if 'host' is an empty string (the default), the implementation MUST choose an arbitrary host for execution.
        @Note:  stdin, stdout and stderr are guaranteed to contain/provide the complete standard I/O
             streams, beginning at the start of the remote process.

        """
        job = Job()
        stdin = StdIO()
        stdout = StdIO()
        stderr = StdIO()
        return job, stdin, stdout, stderr

    def list(self):
        #out array<string>   job_ids
        """
        Get a list of jobs which are currently known by the resource manager.
        job_ids: an list of job identifiers
        @permission: Query on jobs identified by the returned ids
        @raise NotImplemented:
        @raise PermissionDenied:
        @raise AuthorizationFailed:
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @Note: which jobs are viewable by the calling user context, and how long a resource manager keeps job information, are both implementation dependent.
        @Note: a returned job_id may translate into a job (via get_job()) which is not controllable by the requesting application (e.g. it could cause an 'AuthorizationFailed' exception).

        """
        
        return []
    
    def get_job (self, job_id):
        #in  string job_id, out job job
        """
        Given a job identifier, this method returns a Job object representing this job.
        job_id: job identifier as returned by the resource manager
        job: a job object representing the job identified by job_id
        @PreCondition:    Job identified by job_id is managed by the job_service.
        @permission: Query on the job.
        @raise NotImplemented:
        @raise BadParameter:
        @raise DoesNotExist:
        @raise PermissionDenied:
        @raise AuthorizationFailed:
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @Note: in general, only a job_service representing the resource manager which submitted the job may be
            able to handle the job_id, and to identify the job. However, other job_services may succeed as well.
        @Note: if the resource manager can handle the job_id, but the referenced job is not alive, a 'DoesNotExist' exception is raised.
        @Note: if the resource manager cannot parse the job_id at all, a BadParameter exception is raised.

        """
        
        return Job()
        
    def get_self (self):
        #out job_self job
        """
        This method returns a Job object representing _this_ job, i.e. the calling application.
        self: a job_self object representing I{B{this}} job.
        @PreCondition: the application is managed by the job_service.
        @PostCondition: job_self is, by definition, in 'Running' state.
        @permission: Query on the job.
        @raise NotImplemented:
        @raise PermissionDenied:
        @raise AuthorizationFailed:
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @Note: in general, only a job_service representing the resource manager which started the application
                 which now calls get_self() can successfully return a job_self instance. However, other
                 job_services may succeed as well.
        @Note: if a job_service cannot handle the calling job as a job_self instance, a 'NoSuccess' exception
                 is raised, with a descriptive error message.

        """
 
class StdIO(object):
    """
    status: This object is not completly specified yet.
    ===================================================
    
    Should implement methods: ['__class__', '__delattr__', '__doc__', '__enter__', '__exit__', '__getattribute__', '__hash__', '__init__', '__iter__', '__new__', '__reduce__', '__reduce_ex__', '__repr__', '__setattr__', '__str__', 'close', 'closed', 'encoding', 'fileno', 'flush', 'isatty', 'mode', 'name', 'newlines', 'next', 'read', 'readinto', 'readline', 'readlines', 'seek', 'softspace', 'tell', 'truncate', 'write', 'writelines', 'xreadlines']
    This class is used to give acces to the opaque data from stdin, stdout and stderr
    @summary: This class is used to give acces to the opaque data from stdin, stdout and stderr
    """
    
    
class Job(Task, Attributes, Permissions, Async): # Async inherited from Permissions
    """
    status: This object is not completly specified yet.
    ===================================================
 
    
    Attributes:
    name: JobID
    desc: SAGA representation of the job identifier
    mode: ReadOnly
    type: String
    value: -
    notes: - format: as described earlier
    
    name: ExecutionHosts
    desc: list of host names or IP addresses allocated to run this job
    mode: ReadOnly, optional
    type: Vector String B{Need Update TODO}
    value: -
    notes: -
    
    name: Created
    desc: time stamp of the job creation in the resource manager
    mode: ReadOnly, optional
    type: Time
    value: -
    notes: - can be interpreted as submission time
    
    name: Started
    desc: time stamp indicating when the job started running
    mode: ReadOnly, optional
    type: Time
    value: -
    
    name: Finished
    desc:  time stamp indicating when the job completed
    mode:  ReadOnly, optional
    type:  Time
    value: -
    
    name:  WorkingDirectory
    desc:  working directory on the execution host
    mode:  ReadOnly, optional
    type:  String
    value: -
    notes: - can be used to determine the location of
    files staged using relative file paths
    
    name:  ExitCode
    desc:  process exit code as collected by the wait(2)
    series of system calls.
    mode: ReadOnly, optional
    type: Int
    value: -
    notes: - exit code is collected from the process which was started from the 'Executable'  attribute of the job_description object.
           - only available in final states, if at all
    
    name:  Termsig
    desc:  signal number which caused the job to exit
    mode:  ReadOnly, optional
    type:  Int
    value: -
    notes: - only available in final states, if at all
    
    Metrics:
    name: job.state
    desc: fires on state changes of the job, and has the literal value of the job state enum.
    mode: ReadOnly
    unit: 1
    type: Enum
    value: New
    note: the state metric is inherited from Task, but has a different set of possible values
           
    name: job.state_detail
    desc:  fires as a job changes its state detail
    mode:  ReadOnly, optional
    unit:  1
    type:  String
    value: -
    
    name:  job.signal
    desc:  fires as a job receives a signal, and has a value indicating the signal number
    mode: ReadOnly, optional
    unit: 1
    type: Int
    value: -
    note: - no guarantees are made that any or all signals can be notified by this metric
  
    name:  job.cpu_time
    desc:  number of CPU seconds consumed by the job
    mode:  ReadOnly, optional
    unit:  seconds
    type:  Int
    value: -
    notes: - aggregated across all processes/threads
    
    name:  job.memory_use
    desc:  current aggregate memory usage
    mode:  ReadOnly, optional
    unit:  megabyte
    type:  Float
    value: 0.0
    note:  metric becomes 'Final' after job completion, and then shows the memory high water mark
  
    name:  job.vmemory_use
    desc:  current aggregate virtual memory usage
    mode:  ReadOnly, optional
    unit:  megabyte
    type:  Float
    value: 0.0
    note:  metric becomes 'Final' after job completion, and then shows the virtual memory high water mark
  
    name:  job.performance
    desc:  current performance
    mode:  ReadOnly, optional
    unit:  FLOPS
    type:  Float
    value: 0.0
    notes: - metric becomes 'Final' after job completion, and then shows the performance high water mark
    
    """

    #job inspection
    def get_job_description(self):
        #out job_description  jd
        """
        Retrieve the job_description which was used to submit this job instance.
        jd: a JobDescription object
        @PostCondition:   jd is deep copied (no state is shared after method invocation)
        @permission: Query:
        @raise NotImplemented:
        @raise DoesNotExist:
        @raise PermissionDenied:
        @raise AuthorizationFailed:
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @Note: There are cases when the JobDescription is not available. This may include cases when
              the job was not submitted through SAGA and get_job() was used to retrieve the job, or when this state information has been
              lost (e.g. the client application restarts and the particular SAGA implementation did not
              persist the information). In that case, a 'DoesNotExist' exception is raised, with a descriptive error message.
        """
        return JobDescription()
    
    def get_stdin(self):
        """
        Retrieve input stream for a job.
        stdin: standard input stream for the job
        @PreCondition:    the job is interactive.
        @PostCondition:   the jobs standard input stream is available  at stdin.
        @permission: Write (application can write to the jobs stdin).
        @raise NotImplemented:
        @raise BadParameter:
        @raise DoesNotExist:
        @raise IncorrectState:
        @raise PermissionDenied:
        @raise AuthorizationFailed:
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @Note: if the preconditions are met, but the standard input stream is not available for some
                 reason, a 'DoesNotExist' exception is raised.
        @Note: the stream MUST be valid until the job reaches a final state. If it is, for some reason,
                 disconnected earlier, a language typical error message is raised (e.g. EBADF could be
                 returned on writes on that stream in C).
        @Note: if the job is not interactive, e.g. it was  submitted with the 'Interactive' attribute set
                 to 'False', an 'IncorrectState' exception is raised.
        @Note: if the job is not in 'New' state, it is not guaranteed that the job did not receive other
                 data on its standard input stream before.

        """
        return StdIO
    
    def get_stdout(self):
        """
        Retrieve output stream of job
        stdout: standard output stream for the job
        @PreCondition:    the job is interactive.
        @PostCondition:   the jobs standard output stream is available from stdout.
        @permission: Read (application can read the jobs stdout).
        @raise NotImplemented:
        @raise BadParameter:
        @raise DoesNotExist:
        @raise IncorrectState:
        @raise PermissionDenied:
        @raise AuthorizationFailed:
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @Note: if the preconditions are met, but the standard output stream is not available for some
              reason, a 'DoesNotExist' exception is raised.
        @Note: the stream MUST be valid until the job reaches a final state. If it is, for some reason,
              disconnected earlier, a language typical error message is raised (e.g. EBADF could be
              returned on reads on that stream in C).
        @Note: if the job is not interactive, e.g. it was submitted with the 'Interactive' attribute set
              to 'False', an 'IncorrectState' exception is raised.
        @Note: if the job is not in 'New' state, it is not guaranteed that the job did write data on
              its standard output stream before, which are then not returned on the returned stream.

        """
        return StdIO
    
    def get_stderr(self):
        """
        Retrieve error stream of job
        stderr: standard error stream for the job
        @PreCondition:    the job is interactive.
        @PostCondition:   the jobs standard error stream is available from stderr.
        @permission: Read (application can read the jobs stderr).
        @raise NotImplemented:
        @raise BadParameter:
        @raise DoesNotExist:
        @raise IncorrectState:
        @raise PermissionDenied:
        @raise AuthorizationFailed:
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @Note: if the preconditions are met, but the standard error stream is not available for some
              reason, a 'DoesNotExist' exception is raised.
        @Note: the stream MUST be valid until the job reaches a final state. If it is, for some reason,
              disconnected earlier, a language typical error message is raised (e.g. EBADF could be
              returned on reads on that stream in C).
        @Note: if the job is not interactive, e.g. it was submitted with the 'Interactive' attribute set
              to 'False', an 'IncorrectState' exception is raised.
        @Note: if the job is not in 'New' state, it is not guaranteed that the job did write data on
              its standard error stream before, which are then not returned on the returned stream.

        """
        return StdIO
    
    #job management
    def suspend(self):
        """
        Ask the resource manager to perform a suspend operation on the running job.
        @PreCondition:    the job is in 'Running' state.
        @PostCondition:   the job is in 'Suspended' state.
        @permission: Exec (job can be controlled).
        @raise NotImplemented:
        @raise IncorrectState:
        @raise PermissionDenied:
        @raise AuthorizationFailed:
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @Note:  if the job is not in 'Running' state, an 'IncorrectState' exception is raised.

        """
        pass

    def resume(self):
        """
        Ask the resource manager to perform a resume operation on a suspended job.
        @PreCondition:   the job is in 'Suspended' state.
        @PostCondition:   the job is in 'Running' state.
        @permission: Exec (job can be controlled).
        @raise NotImplemented:
        @raise IncorrectState:
        @raise PermissionDenied:
        @raise AuthorizationFailed:
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @Note:  if the job is not in 'Suspended' state, an 'IncorrectState' exception is raised.

        """
        pass
    
    def checkpoint(self):
        """
        Ask the resource manager to initiate a checkpoint operation on a running job.
        @PreCondition: the job is in 'Running' state.
        @PostCondition: the job is in 'Running' state.
        @PostCondition: the job was checkpointed.
        @permission: Exec (job can be controlled).
        @raise NotImplemented:
        @raise IncorrectState:
        @raise PermissionDenied:
        @raise AuthorizationFailed:
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @Note: The semantics of checkpoint(), and the actions taken to initiate a checkpoint, are resource
              manager specific. In particular, the implementation or backend can trigger either
              a system level or an application level
        @note: if the job is not in 'Running' state, an 'IncorrectState' exception is raised.

        """
        pass
    
    def migrate(self, jd):
        #in job_description jd
        """
        Ask the resource manager to migrate a job.
        jd: new job parameters to apply when the job is migrated
        @PreCondition:    the job is in 'Running' or 'Suspended' state.
        @PostCondition:    the job keeps its state.
        @PostCondition: jd is deep copied (no state is shared after method invocation)
        @PostCondition: the job reflects the attributes specified in the job_description.
        @permission:   Exec (job can be controlled).
        @raise NotImplemented:
        @raise BadParameter:
        @raise IncorrectState:
        @raise AuthorizationFailed:
        @raise AuthenticationFailed:
        @raise PermissionDenied:
        @raise Timeout:
        @raise NoSuccess:
        @Note: jd might indicate new resource requirements, for example.
        @Note: the action of migration might change the job identifier within the resource manager.
        @Note: ideally, the submitted job description was obtained by get_job_description(), and then
                 changed by the application. This is not a requirement though.
        @Note: if the job is not in 'Running' or 'Suspended' state, an 'IncorrectState' exception is raised.
        @Note: the method can call the same exceptions as the submit_job() and run() methods, in
                 particular in respect to an incorrect job_description.
       
        """
        pass
    
    def signal(self, signum):
        #in int signum
        """
        Ask the resource manager to deliver an arbitrary signal to a dispatched job.
        signum: signal number to be delivered
        @PreCondition:    job is in 'Running' or 'Suspended' state.
        @PostCondition:    the signal was delivered to the job.
        @Permission:  Exec (job can be controlled).
        @raise NotImplemented:
        @raise BadParameter:
        @raise IncorrectState:
        @raise PermissionDenied:
        @raise AuthorizationFailed:
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @Note: there is no guarantee that the signal number specified is valid for the operating system
                  on the execution host where the job is running, or that the signal can be delivered.
        @Note: if the signal number is not supported by the backend, a BadParameter exception is raised.
        @Note: if the job is not in 'Running' or 'Suspended' state, an 'IncorrectState' exception is raised.
        
        """
        pass


class JobSelf(Job, Steerable):
    """
    The job_self class IS-A job which represents the current application (i.e. the
    very application which owns that job_self instance). It can only by created
    by calling get_self() on a job service (that call can fail though).
    
    status: This object is not completly specified yet.
    ===================================================
    """