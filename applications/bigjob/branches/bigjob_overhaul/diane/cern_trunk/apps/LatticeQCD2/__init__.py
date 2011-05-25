# some utility functions

def dotjoin(*args):
    return '_'.join(args)

# for small files only
def append_to_file(dest, src):
    d = file(dest,'a')
    s = file(src,'r')
    d.write(s.read())
    s.close()
    d.close()

def count_lines(fn):
    try:
        return len(file(fn).readlines())
    except IOError: #FIXME: eexist
        return 0

def chmod_executable(path):
    "make a file executable by the current user (u+x)"
    import stat
    os.chmod(path,stat.S_IXUSR|os.stat(path).st_mode)


# utility modules

import re
import glob
import os
import time
import shutil
from diane.util.compatibility import uuid

import diane.logger
logger = diane.logger.getLogger('LQCD')

copyfile = shutil.copyfile
remove = os.remove
rename = os.rename 
append_to_file = append_to_file 

copyfile = diane.logger.logit(copyfile)
remove = diane.logger.logit(remove)
rename = diane.logger.logit(rename)
append_to_file = diane.logger.logit(append_to_file)

###############################################################################################################
# Here we define the data structures with parameters for the run, tasks and the task result.

PARAMETERS_FILE = 'input_Nf2+1'
PARAMETERS_FILE_TEMPLATE = PARAMETERS_FILE+'.TEMPLATE'
#EXE_NAME = 'hmc_su3_Nf2+1'
#EXE_NAME = 'hmc_su3_newmuI'

EXE_NAME = 'run_su3.sh'
TAR_NAME = 'grid_su3_omp.tgz'
SNAPSHOT_PREFIX = 'snap'

class LQCDTaskIn:
    def __init__(self,snapshot_name,ntraj=1):
        self.snapshot_name = snapshot_name
        r = re.compile('%s[_](?P<ntraj_count>\d+)[_](?P<beta>\d+(\.\d*)?|\.\d+)[_](?P<seed>\d+)'%SNAPSHOT_PREFIX).match(os.path.basename(snapshot_name))
        assert r, 'malformed snapshot file %s'%snapshot_name
        self.beta = r.group('beta')
        self.seed = r.group('seed')
        self.ntraj_count = int(r.group('ntraj_count'))
        #print 'TASK',snapshot_name, self.beta, self.seed
        self.runtype = 'auto'
        self.ntraj = ntraj
        #self.ntraj_count = count_lines(self._fn('fort.11',basedir = os.path.dirname(os.path.abspath(snapshot_name))))
        self.task_uuid = uuid()

    def _fn(self,fn,basedir=''):
        return os.path.join(basedir,dotjoin(fn,"%04d"%self.ntraj_count,str(self.beta),str(self.seed)))

    def dat_fn(self,fn,basedir=''):
        return os.path.join(basedir,'dat',self._fn(fn))

    def tmp_fn(self,fn,basedir=''):
        return os.path.join(basedir,'tmp',dotjoin(self._fn(fn),str(self.task_uuid)))

    def bak_fn(self,fn,basedir=''):
        return os.path.join(basedir,'bak',dotjoin(self._fn(fn),'bak'))

    def expand_template(self,template_filename,dest_filename):
        template = file(template_filename).read()
        template = template.replace('###BETA###',str(self.beta))
        template = template.replace('###RUNTYPE###',str(self.runtype))        
        template = template.replace('###SEED###',str(self.seed))
        template = template.replace('###NTRAJ###',str(self.ntraj))
        file(dest_filename,'w').write(template)

class ResourceUsageInfo:
    def __init__(self):
        self.elapsed_wallclock_time = 0
        self.rusage1 = None
        self.rusage2 = None
        
class LQCDTaskOut:
    def __init__(self):
        self.outputs = [] # list of files: fort.15 (Files objects)
        self.task_in = None # original task structure (with updated ntraj_count)
        self.info = None #ResourceUsageInfo

class LQCDWorkerInit:
    def __init__(self):
        self.file_server_url = None
        self.parameters_template_file = None
        
class LQCDRun:
    def __init__(self):
        self.basedir = ''
        self.file_server_url = None
        self.parameters_template_file = None
        self.compare_by = "equalize"
        self.compare_params = None
        
###############################################################################################################
# Here we define the application components which will enable the framework to use the application's functionality.

from diane import ITaskScheduler, IApplicationWorker, TaskInfo
from diane.FileTransfer import FileTransferOptions

from send_file import createFileTransferClient
from diane.util.File import File
    
class LQCDWorker(IApplicationWorker):
    def initialize(self,init):
        """
        This method is called only once per worker agent.
        The init parameters are sent by the master (and they result of calling make_init() method of the application manager).
        """
        logger.info('*** initialize: worker id=%d',self._agent.wid)

        self.ftc = createFileTransferClient(init.file_server_url,program=self._agent.program)

        self.parameters_template_file = init.parameters_template_file

#         #KUBA: AMD FIX BEGIN
#         global EXE_NAME
        
#         if file('/proc/cpuinfo').read().find('Opteron') != -1:
#             EXE_NAME += '.amd_opteron'
#         #KUBA: AMD FIX END
        
        self.ftc.download_file(TAR_NAME)

        if os.system("tar xfzv %s"%TAR_NAME) != 0:
            raise Exception('cannot extract tarfile %s'%TAR_NAME)

        #chmod_executable(EXE_NAME)
        
        self.ftc.download_file(init.parameters_template_file)
        return EXE_NAME

    def finalize(self,x):
        """ This method is called only once per worker agent.
        """
        logger.info('*** finalize: worker id=%d',self._agent.wid)
        
    def do_work(self,task):
        """ This method is called to execute a task. If execution succeeds it returns the CrashResult object. This object is
        automatically sent back to the master. How many tasks are executed by a given worker agent depends
        on the scheduler and other runtime conditions.
        """

        task.expand_template(self.parameters_template_file,'parameters')

        self.ftc.download_file(task.dat_fn(SNAPSHOT_PREFIX),dest='fort.2') #lazy = True, override = True

        rename('fort.2','fort.1')

        info = ResourceUsageInfo()

        try:
            import resource
        except ImportError:
            resource = None

        if resource:
            info.rusage1 = resource.getrusage(resource.RUSAGE_CHILDREN)

        t0 = time.time()
        cmd = './%s 4 %s'%(EXE_NAME,'parameters')

        # increase the stack size limit....
        #cmd = '(env; ulimit -s 20000; ./%s < %s)'%(EXE_NAME,'parameters')

        info.elapsed_wallclock_time = time.time()-t0

        retval = os.system(cmd) 

        #self.ftc.upload_file('hmc_su3_omp.stdout',dest=task._fn('hmc_su3_omp.stdout','dat'), opts=FileTransferOptions(overwrite=True))
        #self.ftc.upload_file('hmc_su3_omp.stderr',dest=task._fn('hmc_su3_omp.stderr','dat'), opts=FileTransferOptions(overwrite=True))

        if retval != 0:
            raise Exception('error executing: %s'%cmd, info)

        if resource:
            info.rusage2 = resource.getrusage(resource.RUSAGE_CHILDREN)
        
        out = LQCDTaskOut()
        out.info = info
        out.task_in = task
        out.task_in.ntraj_count = task.ntraj_count + task.ntraj
        out.outputs = []
        for fn in ['fort.15']:
            out.outputs.append(File(fn,dest=task.tmp_fn(fn)))
                       
        self.ftc.upload_file('fort.2',dest=task.tmp_fn(SNAPSHOT_PREFIX))
        
        return out
    
import threading

class LQCDTaskScheduler(ITaskScheduler):
    def __init__(self,job_master,appmgr):
        ITaskScheduler.__init__(self,job_master,appmgr)
        self.tid = 0
        self.worker_init = LQCDWorkerInit()
        self.snapshots_tasks = {}
        self.output_area_lock = threading.Lock()
        self.errors_found = False

    def initialize(self,run_data):
        self.run_data = run_data
        self.run_data.basedir = os.path.abspath(run_data.basedir)
        self.worker_init.file_server_url = self.run_data.file_server_url
        self.worker_init.parameters_template_file = self.run_data.parameters_template_file
        logger.info('basedir %s',run_data.basedir)
        
    def run(self):
        logger.info('calling run()')
        # must call base class implementation first
        ITaskScheduler.run(self)

        once = True
        
        while self.has_more_work() and not self.should_stop():
            try:
                self.output_area_lock.acquire()

                alive_workers = [w for w in self.job_master.worker_registry.alive_workers.values() if hasattr(w,'snapshot_task') and not w.snapshot_task is None]

                self.snapshot_tasks = dict([(task.snapshot_name,task) for task in
                                           [LQCDTaskIn(snapshot_name,ntraj=self.run_data.ntraj) for snapshot_name in
                                            glob.glob(os.path.join(self.run_data.basedir,'dat',SNAPSHOT_PREFIX+'*'))]])

                unassigned_snapshot_tasks= [task for task in self.snapshot_tasks.values() if (task.beta,task.seed) not in
                                            [(w.snapshot_task.beta,w.snapshot_task.seed) for w in alive_workers]]


                # give priority to tasks which have least iterations done so far (ntraj_count)
                # tasks with most number of iterations will appear first on the list
                def equalize(t1,t2):
                    return -cmp(t1.ntraj_count,t2.ntraj_count)


                # give priority to the tasks closer to the left edge of [m,M] window
                # outside of this window just equalize

                #m = 5.1815
                #M = 5.18525

                def left_priority_window(t1,t2):
                    b1,b2 = float(t1.beta) ,float(t2.beta)

                    m,M = self.run_data.compare_params
                    
                    def in_range(x):
                        return m <= x and x <= M

                    if in_range(b1):
                        if in_range(b2):
                            if b1<b2:
                                return 1
                            elif b1>b2:
                                return -1
                            else:
                                return equalize(t1,t2)
                        else:
                            return 1
                    else:
                        if in_range(b2):
                            return -1
                        else:
                            return equalize(t1,t2)

                unassigned_snapshot_tasks.sort(locals()[self.run_data.compare_by])

                # some security checks to make sure that the same snapshot is never done by two workers at the same time
                _check_active_snapshots = [(w.snapshot_task.beta,w.snapshot_task.seed) for w in alive_workers]
                #if len(set(_check_active_snapshots)) != len(_check_active_snapshots):
                #       logger.error("same snapshot assigned to worker more than once (beta,seed,wid):")
                #       logger.error(str([zip(_check_active_snapshots,[w.wid for w in alive_workers])]))
                #       self.errors_found = True
                #       return
                

                if once:
                    logger.info('')
                    logger.info('unassigned snapshot tasks (%d)',len(self.snapshot_tasks))
                    for t in unassigned_snapshot_tasks:
                        logger.info(str(t.snapshot_name))

                once = False
                
                waiting_workers = self.job_master.worker_registry.waiting_workers.values()

                #logger.info('waiting workers: %s',[w.wid for w in waiting_workers])
                #logger.info('unassigned snapshot tasks: %s',[(t.beta,t.seed) for t in unassigned_snapshot_tasks])
                
                for w in waiting_workers:
                    self.tid += 1
                    t = TaskInfo(self.tid)

                    if w.snapshot_task is None:
                        try:
                            w.snapshot_task = unassigned_snapshot_tasks.pop()
                        except IndexError:
                            # more workers than available snapshots
                            break
                    else:
                        #FIXME: this is ugly and should be implemented idealy in tasks_completed() but there is currently no way
                        # of assotiating the worker to the completed task in that method...
                        w.snapshot_task.ntraj_count += w.snapshot_task.ntraj
                        pass
                        
                    t.task_input = w.snapshot_task
                    
                    logger.info('scheduling (%s,%s,%d) -> %s',t.task_input.beta,t.task_input.seed,t.task_input.ntraj_count,w.wid)
                    self.job_master.schedule(w,[t])
            finally:
                self.output_area_lock.release()
            time.sleep(1)

    def has_more_work(self):
        return not self.errors_found #loop forever (bail out on errors!)

    def worker_added(self,w):
        w.init_input = self.worker_init
        w.snapshot_task = None

    def worker_initialized(self,w):
        logger.info('worker wid=%d initialized OK, executable: %s',w.wid,w.init_output)

    def tasks_completed(self, tasks):
        try:
            self.output_area_lock.acquire()

            bdir = self.run_data.basedir
            for t in tasks:
                elapsed_wallclock_time = t.task_output.info.elapsed_wallclock_time
                try:
                    ru1,ru2 = t.task_output.info.rusage1[:],t.task_output.info.rusage2[:]
                except TypeError:
                    ru1,ru2 = None,None

                #save resoruce usage information for further processing
                self.job_master.journal.addEntry('lqcd_task_resources',tid=t.tid,wid=t.details.assigned_wid, elapsed_wallclock_time=elapsed_wallclock_time, usage_before=ru1, usage_after=ru2)

                tin0 = t.task_input # ntraj at task start
                tin1 = t.task_output.task_in # ntraj at task end
                
                logger.info('starting moving file set (%s,%s)',tin0.beta,tin0.seed)
                for f in t.task_output.outputs:
                    f.dest = os.path.join(bdir,f.dest) # put the files into the tmp area
                    f.write()

                # make backup of current snapshot and results
                rename(tin0.dat_fn(SNAPSHOT_PREFIX,bdir), tin0.bak_fn(SNAPSHOT_PREFIX,bdir))
                
                for fn in ['fort.15']:
                    if os.path.exists(tin0.dat_fn(fn,bdir)):
                        copyfile(tin0.dat_fn(fn,bdir), tin0.bak_fn(fn,bdir))

                # move the new snapshot from tmp to dat area
                rename(tin1.tmp_fn(SNAPSHOT_PREFIX,bdir), tin1.dat_fn(SNAPSHOT_PREFIX,bdir))

                for fn in ['fort.15']:
                    append_to_file(tin0.dat_fn(fn,bdir), tin1.tmp_fn(fn,bdir))
                    remove(tin1.tmp_fn(fn,bdir))
                    rename(tin0.dat_fn(fn,bdir), tin1.dat_fn(fn,bdir)) # new
                logger.info('ended moving file set (%s,%s)',tin0.beta,tin0.seed)                
        finally:
            self.output_area_lock.release()
        
    def tasks_failed(self,tasks):
        for t in tasks:
            wid = t.details.assigned_wid
            self.job_master.remove_worker(wid)

    def tasks_unscheduled(self,tasks):
        pass

    def tasks_lost(self,tasks):
        pass



        

###############################################################################################################
# Here we finally connect our application to the framework.  When a run is
# started this rundef() function is executed first: it sets the default run
# behaviour for this application.  Then a similar rundef() function from the
# run file provided by the user is executed and may customize the paeremetrs.

def run(input,config):
     input.scheduler = LQCDTaskScheduler
     input.manager = None
     input.worker = LQCDWorker
     input.data = LQCDRun()

