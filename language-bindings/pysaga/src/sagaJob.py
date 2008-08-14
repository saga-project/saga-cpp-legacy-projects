# Page 164

  package saga.job
  {
    enum state
    {
      New        =  1,  // same as in  saga::task::state
      Running    =  2,  // same as in  saga::task::state
      Done       =  3,  // same as in  saga::task::state
      Canceled   =  4,  // same as in  saga::task::state
      Failed     =  5,  // same as in  saga::task::state
      Suspended =    6
    }
    class job_description : implements    saga::object
                            implements    saga::attributes
                         // from object: saga::error_handler
    {
      CONSTRUCTOR          (out job_description obj);
      DESTRUCTOR           (in job_description obj);
      // Attributes:
      //   name: Executable
      //   desc: command to execute.
      //   type: String
      //   mode: ReadWrite
                                                             168
saga-core-wg@ogf.org
GFD-R-P.90            SAGA Job Management           January 15, 2008
      //   value: ’’
      //   notes: - this is the only required attribute.
      //          - can be a full pathname, or a pathname
      //            relative to the ’WorkingDirectory’ as
      //            evaluated on the execution host.
      //          - semantics as defined in JSDL
      //          - available in JSDL, DRMAA
      //
      //   name:  Arguments
      //   desc:  positional parameters for the command.
      //   mode:  ReadWrite, optional
      //   type:  Vector String
      //   value: -
      //   notes: - semantics as specified by JSDL
      //          - available in JSDL, DRMAA
      //
      //   name:  SPMDVariation
      //   desc:  SPMD job type and startup mechanism
      //   mode:  ReadWrite, optional
      //   type:  String
      //   value: -
      //   notes: - as defined in the SPMD extension of JSDL
      //   notes: - semantics as defined in JSDL
      //          - available in JSDL, SPMD extension
      //          - the SPMD JSDL extension defines the value
      //            to be an URI. For simplicity, SAGA allows
      //            the following strings, which map into the
      //            respective URIs: MPI, GridMPI, IntelMPI,
      //            LAM-MPI, MPICH1, MPICH2, MPICH-GM, MPICH-MX,
      //            MVAPICH, MVAPICH2, OpenMP, POE, PVM, None
      //          - the value ’’ (no value, default) indicates
      //            that the application is not a SPMD
      //            application.
      //          - as JSDL, SAGA allows other arbitrary values.
      //            The implementation must clearly document
      //            which values are supported.
      //
      //   name:  TotalCPUCount
      //   desc:  total number of cpus requested for this job
      //   mode:  ReadWrite, optional
      //   type:  Int
      //   value: ’1’
      //   notes: - semantics as defined in JSDL
      //          - available in JSDL, DRMAA
      //
      //   name:  NumberOfProcesses
                                                                 169
saga-core-wg@ogf.org
GFD-R-P.90             SAGA Job Management          January 15, 2008
      //   desc:  total number of processes to be started
      //   mode:  ReadWrite, optional
      //   type:  Int
      //   value: ’1’
      //   notes: - semantics as defined in JSDL
      //          - available in JSDL, SPMD extension
      //
      //   name:  ProcessesPerHost
      //   desc:  number of processes to be started per host
      //   mode:  ReadWrite, optional
      //   type:  Int
      //   value: ’1’
      //   notes: - semantics as defined in JSDL
      //          - available in JSDL, SPMD extension
      //
      //   name:  ThreadsPerProcess
      //   desc:  number of threads to start per process
      //   mode:  ReadWrite, optional
      //   type:  Int
      //   value: ’1’
      //   notes: - semantics as defined in JSDL
      //          - available in JSDL, SPMD extension
      //
      //   name:  Environment
      //   desc:  set of environment variables for the job
      //   mode:  ReadWrite, optional
      //   type:  Vector String
      //   value: -
      //   notes: - exported into the job environment
      //          - format: ’key=value’
      //          - semantics as specified by JSDL
      //          - available in JSDL, DRMAA
      //
      //   name:  WorkingDirectory
      //   desc:  working directory for the job
      //   mode:  ReadWrite, optional
      //   type:  String
      //   value: ’.’
      //   notes: - semantics as specified by JSDL
      //          - available in JSDL, DRMAA
      //
      //   name:  Interactive
      //   desc:  run the job in interactive mode
      //   mode:  ReadWrite, optional
      //   type:  Bool
      //   value: ’False’
                                                                 170
saga-core-wg@ogf.org
GFD-R-P.90            SAGA Job Management           January 15, 2008
      //   notes: - this implies that stdio streams will stay
      //            connected to the submitter after job
      //            submission, and during job execution.
      //          - if an implementation cannot handle
      //            interactive jobs, and this attribute is
      //            present, and ’True’, the job creation MUST
      //            throw an ’IncorrectParameter’ error with a
      //            descriptive error message.
      //          - not supported by JSDL, DRMAA
      //
      //   name:  Input
      //   desc:  pathname of the standard input file
      //   mode:  ReadWrite, optional
      //   type:  String
      //   value: -
      //   notes: - semantics as specified by JSDL
      //          - available in JSDL, DRMAA
      //          - will not be used if ’Interactive’ is ’True’
      //
      //   name:  Output
      //   desc:  pathname of the standard output file
      //   mode:  ReadWrite, optional
      //   type:  String
      //   value: -
      //   notes: - semantics as specified by JSDL
      //          - available in JSDL, DRMAA
      //          - will not be used if ’Interactive’ is ’True’
      //
      //   name:  Error
      //   desc:  pathname of the standard error file
      //   mode:  ReadWrite, optional
      //   type:  String
      //   value: -
      //   notes: - semantics as specified by JSDL
      //          - available in JSDL, DRMAA
      //          - will not be used if ’Interactive’ is ’True’
      //
      //   name:  FileTransfer
      //   desc:  a list of file transfer directives
      //   mode:  ReadWrite, optional
      //   type:  Vector String
      //   value: -
      //   notes: - translates into jsdl:DataStaging
      //          - used to specify pre- and post-staging
      //          - semantics as specified in JSDL
      //          - staging is part of the ’Running’ state
                                                                 171
saga-core-wg@ogf.org
GFD-R-P.90            SAGA Job Management           January 15, 2008
      //          - syntax similar to LSF (see earlier notes)
      //          - available in JSDL, DRMAA
      //
      //   name:  Cleanup
      //   desc:  defines if output files get removed after the
      //          job finishes
      //   mode: ReadWrite, optional
      //   type: String
      //   value: ’Default’
      //   notes: - can have the Values ’True’, ’False’, and
      //            ’Default’
      //          - On ’False’, output files MUST be kept
      //            after job the finishes
      //          - On ’True’, output files MUST be deleted
      //            after job the finishes
      //          - On ’Default’, the behaviour is defined by
      //            the implementation or the backend.
      //          - translates into ’DeleteOnTermination’ elements
      //            in JSDL
      //
      //   name:  JobStartTime
      //   desc:  time at which a job should be scheduled
      //   mode:  ReadWrite, optional
      //   type:  Int
      //   value: -
      //   notes: - Could be viewed as a desired job start
      //            time, but that is up to the resource
      //            manager.
      //          - format: number of seconds since epoch
      //          - available in DRMAA
      //          - not supported by JSDL
      //
      //   name:  TotalCPUTime
      //   desc:  estimate total number of CPU seconds which
      //          the job will require.
      //   mode: ReadWrite, optional
      //   type: Int
      //   value: -
      //   notes: - intended to provide hints to the scheduler.
      //            scheduling policies.
      //          - available in JSDL, DRMAA
      //          - semantics as defined in JSDL
      //
      //   name:  TotalPhysicalMemory
      //   desc:  Estimated amount of memory the job requires
      //   mode:  ReadWrite, optional
                                                                 172
saga-core-wg@ogf.org
GFD-R-P.90            SAGA Job Management           January 15, 2008
      //   type: Float
      //   value: -
      //   notes: - unit is in MegaByte
      //          - memory usage of the job is aggregated
      //            across all processes of the job
      //          - semantics as defined by JSDL
      //          - available in JSDL
      //
      //   name:  CPUArchitecture
      //   desc:  compatible processor for job submission
      //   mode:  ReadWrite, optional
      //   type:  Vector String
      //   value: -
      //   notes: - allowed values as specified in JSDL
      //          - semantics as defined by JSDL
      //          - available in JSDL
      //
      //   name:  OperatingSystemType
      //   desc:  compatible operating system for job submission
      //   mode:  ReadWrite, optional
      //   type:  Vector String
      //   value: -
      //   notes: - allowed values as specified in JSDL
      //          - semantics as defined by JSDL
      //          - available in JSDL
      //
      //   name:  CandidateHosts
      //   desc:  list of host names which are to be considered
      //          by the resource manager as candidate targets
      //   mode: ReadWrite, optional
      //   type: Vector String
      //   value: -
      //   notes: - semantics as defined by JSDL
      //          - available in JSDL
      //
      //   name:  Queue
      //   desc:  name of a queue to place the job into
      //   mode:  ReadWrite, optional
      //   type:  String
      //   value: -
      //   notes: - While SAGA itself does not define the
      //            semantics of a "queue", many backend systems
      //            can make use of this attribute.
      //          - not supported by JSDL
      //
      //   name:  JobContact
                                                                 173
saga-core-wg@ogf.org
GFD-R-P.90             SAGA Job Management           January 15, 2008
      //   desc:   set of endpoints describing where to report
      //           job state transitions.
      //   mode: ReadWrite, optional
      //   type: Vector String
      //   value: -
      //   notes: - format: URI (e.g. fax:+123456789,
      //             sms:+123456789, mailto:joe@doe.net).
      //           - available in DRMAA
      //           - not supported by JSDL
    }
    class job_service : implements    saga::object
                         implements   saga::async
                      // from object saga::error_handler
    {
      CONSTRUCTOR           (in session          s,
                             in url              rm = "",
                             out job_service     obj);
      DESTRUCTOR            (in job_service      obj);
      create_job            (in  job_description jd,
                             out job             job);
      run_job               (in  string          commandline,
                             in  string          host = "",
                             out job             job,
                             out opaque          stdin,
                             out opaque          stdout,
                             out opaque          stderr);
      list                  (out array<string>   job_ids);
      get_job               (in  string          job_id,
                             out job             job);
      get_self              (out job_self        job);
    }
    class job : extends       saga::task
                 implements   saga::async
                 implements   saga::attributes
                 implements   saga::permissions
              // from task    saga::object
              // from task    saga::monitorable
              // from object saga::error_handler
    {
      // no CONSTRUCTOR
      DESTRUCTOR            (in job              obj);
                                                                  174
saga-core-wg@ogf.org
GFD-R-P.90            SAGA Job Management            January 15, 2008
      // job inspection
      get_job_description  (out job_description  jd);
      get_stdin            (out opaque          stdin);
      get_stdout           (out opaque          stdout);
      get_stderr           (out opaque          stderr);
      // job management
      suspend              (void);
      resume               (void);
      checkpoint           (void);
      migrate              (in job_description    jd);
      signal               (in int                signum);
      // Attributes:
      //   name: JobID
      //   desc: SAGA representation of the job identifier
      //   mode: ReadOnly
      //   type: String
      //   value: -
      //   notes: - format: as described earlier
      //
      //   name: ExecutionHosts
      //   desc: list of host names or IP addresses allocated
      //          to run this job
      //   mode: ReadOnly, optional
      //   type: Vector String
      //   value: -
      //   notes: -
      //
      //   name: Created
      //   desc: time stamp of the job creation in the
      //          resource manager
      //   mode: ReadOnly, optional
      //   type: Time
      //   value: -
      //   notes: - can be interpreted as submission time
      //
      //   name: Started
      //   desc: time stamp indicating when the job started
      //          running
      //   mode: ReadOnly, optional
      //   type: Time
      //   value: -
      //
      //   name: Finished
                                                                  175
saga-core-wg@ogf.org
GFD-R-P.90             SAGA Job Management          January 15, 2008
      //   desc:  time stamp indicating when the job completed
      //   mode:  ReadOnly, optional
      //   type:  Time
      //   value: -
      //
      //   name:  WorkingDirectory
      //   desc:  working directory on the execution host
      //   mode:  ReadOnly, optional
      //   type:  String
      //   value: -
      //   notes: - can be used to determine the location of
      //            files staged using relative file paths
      //
      //   name:  ExitCode
      //   desc:  process exit code as collected by the wait(2)
      //          series of system calls.
      //   mode: ReadOnly, optional
      //   type: Int
      //   value: -
      //   notes: - exit code is collected from the process
      //            which was started from the ’Executable’
      //            attribute of the job_description object.
      //          - only available in final states, if at all
      //
      //   name:  Termsig
      //   desc:  signal number which caused the job to exit
      //   mode:  ReadOnly, optional
      //   type:  Int
      //   value: -
      //   notes: - only available in final states, if at all
      // Metrics:
      //   name: job.state
      //   desc: fires on state changes of the job, and has
      //          the literal value of the job state enum.
      //   mode: ReadOnly
      //   unit: 1
      //   type: Enum
      //   value: New
      //   notes: - the state metric is inherited from
      //            saga::task, but has a different set
      //            of possible values
      //          - see description of job states above
      //
      //   name: job.state_detail
                                                                 176
saga-core-wg@ogf.org
GFD-R-P.90            SAGA Job Management           January 15, 2008
      //   desc:  fires as a job changes its state detail
      //   mode:  ReadOnly, optional
      //   unit:  1
      //   type:  String
      //   value: -
      //
      //   name:  job.signal
      //   desc:  fires as a job receives a signal, and has a
      //          value indicating the signal number
      //   mode: ReadOnly, optional
      //   unit: 1
      //   type: Int
      //   value: -
      //   notes: - no guarantees are made that any or all
      //            signals can be notified by this metric
      //
      //   name:  job.cpu_time
      //   desc:  number of CPU seconds consumed by the job
      //   mode:  ReadOnly, optional
      //   unit:  seconds
      //   type:  Int
      //   value: -
      //   notes: - aggregated across all processes/threads
      //
      //   name:  job.memory_use
      //   desc:  current aggregate memory usage
      //   mode:  ReadOnly, optional
      //   unit:  megabyte
      //   type:  Float
      //   value: 0.0
      //   notes: - metric becomes ’Final’ after job
      //            completion, and then shows the memory
      //            high water mark
      //
      //   name:  job.vmemory_use
      //   desc:  current aggregate virtual memory usage
      //   mode:  ReadOnly, optional
      //   unit:  megabyte
      //   type:  Float
      //   value: 0.0
      //   notes: - metric becomes ’Final’ after job
      //            completion, and then shows the virtual
      //            memory high water mark
      //
      //   name:  job.performance
      //   desc:  current performance
                                                                 177
saga-core-wg@ogf.org
GFD-R-P.90             SAGA Job Management          January 15, 2008
     //    mode:  ReadOnly, optional
     //    unit:  FLOPS
     //    type:  Float
     //    value: 0.0
     //    notes: - metric becomes ’Final’ after job
     //             completion, and then shows the performance
     //             high water mark
   }
   class job_self : extends      saga::job
                      implements saga::steerable
                  // from job    saga::async
                  // from job    saga::attributes
                  // from job    saga::task
                  // from job    saga::object
                  // from job    saga::monitorable
                  // from job    saga::permissions
                  // from job    saga::error_handler
   {
     // no CONSTRUCTOR
     DESTRUCTOR             (in job_self        obj);
   }
 }
