import diane

logger = diane.getLogger('ExecutableApplication')

import os.path, shutil, glob

class ExecutablePolicy:
    """ Collection of parameters (with default values) to control the Executable application.
    """ 
    ## Allow missing output files.
    ## True => worker will try to upload as many output files as possible and report the task as completed.
    ## False => if any of the output files cannot be uploaded then report the task as failed.
    ALLOW_MISSING_OUTPUT = False
    

class ExecutableRunData:
    def __init__(self):
        # list of TaskData items
        self.tasks = []

        # unless specified otherwise all tasks share these defaults
        self.task_defaults = ExecutableTaskData()
        self.task_defaults.input_files = []
        self.task_defaults.output_files = []
        self.task_defaults.args = []

        self.policy = ExecutablePolicy()

        # these attributes are set automatically and are needed to copy the input files
        self._udir = None # user directory (where the diane-run was executed)
        self._idir = None # input_files directory
        self._odir = None # output_files directory
        self._rdir = None # run directory

    def newTask(self):
        d = ExecutableTaskData()
        self.tasks.append(d)
        return d

class ExecutableWorkerData:
    def __init__(self,task_defaults,policy):
        self.task_defaults = task_defaults
        self.policy = policy

class ExecutableTaskData:
    def __init__(self):
        self.executable = None
        self.input_files = None
        self.output_files = None
        self.args = None
        
        self.application_label = None
        self.application_details = []
        #this attribute is set automatically (task identifier)
        self._tid = None

    def dirpath(self):
        return '%05d'%self._tid
        
class ExecutableTaskResult:
    def __init__(self,task_data=None):
        self.task_data = task_data
        self.rusage1 = None #resource.struct_rusage information before the task was started
        self.rusage2 = None #same just after the task was started
        # specifies if some of the policies have been effectively applied
        # the KEY is the policy name
        # the VALUE may be anything useful to indicate HOW the policy was applied
        # unless otherwise speicified, the VALUE is simply True/False indicating IF the policy was applied
        self.applied_policy = ExecutablePolicy()

############################################################################################

class ExecutableWorker(diane.IApplicationWorker):
    def initialize(self,worker_data):
        self.worker_data = worker_data

    def do_work(self,task_data):
        ftc = self._agent.ftc

        # get the parameters of the task
        def get_value(name):
            v = getattr(task_data,name)
            if v is None: 
                return getattr(self.worker_data.task_defaults,name)
            else:
                return v

        executable = os.path.basename(get_value('executable'))
        args = get_value('args')
        output_files = get_value('output_files')

        # resolve default input files to the shared directory
        # resolve task-specific input files to the task subdirectory
        if task_data.input_files is None:
            input_files = [os.path.join('input_files',os.path.basename(f)) 
                           for f in self.worker_data.task_defaults.input_files]
        else:
            input_files = [os.path.join('input_files',task_data.dirpath(),os.path.basename(f)) 
                           for f in task_data.input_files]

        # download input files
        for f in input_files:
            ftc.download(os.path.basename(f),f)

        # prepare result object
        result = ExecutableTaskResult()

        try:
            import resource
        except ImportError:
            resource = None

        ##############################################
        # change the permission bit of the executable
        diane.util.chmod_executable(executable)

        # execute the application
        import subprocess

        stdout = file('_stdout','w')
        stderr = file('_stderr','w')

        if resource:
            result.rusage1 = resource.getrusage(resource.RUSAGE_CHILDREN)
        
        logger.info('executing %s %s',executable,args)

        retcode = subprocess.call([os.path.join('.',executable)]+args, stdout = stdout, stderr = stderr)

        if resource:
            result.rusage2 = resource.getrusage(resource.RUSAGE_CHILDREN)

        ##############################################
        # handle output files

        from diane.FileTransfer import FileTransferOptions
        opts = FileTransferOptions(overwrite=True)

        # Upload files. If keep_going is True, then try upload as many
        # files as possible, ingoring exceptions (return rue if no
        # errors or false if some errors occured). Otherwise just bail
        # out on first transfer error with an exception.
        def upload_files(fns,keep_going):
            status = False
            for fn in fns:
                try:
                    ftc.upload(fn,os.path.join('output_files',task_data.dirpath(),fn),opts=opts)
                except Exception,x:
                    if keep_going:
                        status = True
                    else:
                        raise
            return status

        # upload standard files (report any errors immediately)
        upload_files([stdout.name,stderr.name],keep_going=False)
        
        # keep going only if set by the policy

        def flatten_sublists(l):
            return [item for sublist in l for item in sublist]
        
        all_output_files = flatten_sublists([glob.glob(fnp) for fnp in output_files])
        
        if retcode != 0:
            upload_files(all_output_files,keep_going=True)
            raise Exception('application %s returned non-zero code %d'%(executable, retcode))
        else:
            status = upload_files(all_output_files,keep_going = self.worker_data.policy.ALLOW_MISSING_OUTPUT)
            result.applied_policy.ALLOW_MISSING_OUTPUT = status

        result.task_data = task_data
        return result
        
    def finalize(self,cookie):
        pass

############################################################################################

class ExecutableApplicationManager(diane.SimpleApplicationManager):
    def initialize(self,run_data):
        self.done_counter = 0
        self.completed_counter = 0
        self.ignored_counter = 0
        self.N = len(run_data.tasks)

        # this value is automatically passed to Worker.initialize()
        self.worker_init = ExecutableWorkerData(run_data.task_defaults,run_data.policy) 

        logger.info('preparing output file area: %s',run_data._odir)
        os.makedirs(run_data._odir)

        logger.info('preparing input file area: %s',run_data._idir)
        os.makedirs(run_data._idir)

        def copy_files(dir,files):
            if not files is None:
                for fn in files:
                    ufn = os.path.join(run_data._udir,fn)
                    logger.info('copying input file %s', 
                                os.path.join(dir[len(os.path.commonprefix(
                                        [run_data._rdir,dir])):],os.path.basename(fn))[1:])
                    logger.debug('source file %s',ufn)
                    shutil.copy(ufn, dir) 

        # copy default input files
        copy_files(run_data._idir, run_data.task_defaults.input_files)

        def check_default(d,name):
            if getattr(d,name) is None and getattr(run_data.task_defaults,name) is None:
                raise Exception('undefined %s (task %s)'%(name,d._tid))

        tasks = []
        for d in run_data.tasks:
            t = self._task(application_label=d.application_label,application_details=d.application_details)
            d._tid = t.tid

            for name in ['args','input_files','output_files','executable']:
                check_default(d,name)

            # copy task-specific input files
            if not d.input_files is None:
                tdir = os.path.join(run_data._idir,d.dirpath())
                os.makedirs(tdir)
                copy_files(tdir,d.input_files)

            # make task-specific output area
            os.makedirs(os.path.join(run_data._odir,d.dirpath()))

            # each value of d is automatically passed to Worker.do_work()
            t.task_input = d 
            tasks.append(t)

        logger.info('%d executable tasks',self.N)
        return tasks 

    def merge(self,task_result):
        pass

    def tasks_done(self,tasks):
        for t in tasks:
            try:
                ru1,ru2 = t.task_output.rusage1,t.task_output.rusage2
            except AttributeError: #protect against task_output is None (which happens if OSError exception is raised by subprocess.call() on the worker node
                ru1,ru2=None,None
            if ru1 and ru2:
                logger.info('task resource usage, tid=%d, wid=%d, before=%s, after=%s',t.tid,t.details.assigned_wid,ru1[:],ru2[:])
                self.scheduler.job_master.journal.addEntry('executable_task_resources',tid=t.tid, wid=t.details.assigned_wid, usage_before=ru1[:],usage_after=ru2[:])

            if t.status == t.IGNORED:
                self.ignored_counter += 1
            else:
                self.merge(t.task_output)
                self.completed_counter += 1
        self.done_counter += len(tasks)

    def has_more_work(self):
        return self.done_counter < self.N

    def finalize(self):
        pass

