import os
import diane

logger = diane.getLogger('CachingExecutableApplication')

#######
# main hooks

# this application plugin extends the executable application, hence a convenience shortcut
import diane_test_applications.ExecutableApplication as executable

# the main hook for the user run file
def run(input,config):
     input.scheduler = diane.SimpleTaskScheduler
     input.manager = ApplicationManager
     input.worker = Worker
     input.data = RunData()

# use the same posprocessing hook as ExecutableApplication
run_post = executable.run_post

########
# run data extensions

class Policy(executable.ExecutablePolicy):
    """ Extended policies to control caching parameters.
    """

    BREAK_CACHE_LOCK_TIMEOUT = 3600


class RunData(executable.ExecutableRunData):
    def __init__(self):
        executable.ExecutableRunData.__init__(self)
        self.policy = Policy()
        self.input_cache = FileCache()


class FileCache:
    def __init__(self,path=None):
        # basic security mechanism: list of authorized directories where input files may be read from
        self.authorized_dirs = [] 
        # list of paths to input files (may be absolute) 
        self.files = []
        self._path = path

    # return the directory where the cache is located on a local filesystem on the worker node
    # filename may be used as a hint (in case cache location is depends on it)
    def access_path(self,fn=None):
        return self._path


##########
# implementation

# input cache attribute will be added dynamically to these objects
# so we just create aliases for convenience
WorkerData = executable.ExecutableWorkerData

from MkdirLockFile import AlreadyLocked, LockTimeout
from MkdirLockFile import MkdirLockFile as LockFile


class Worker(executable.ExecutableWorker):
    def initialize(self,worker_data):
        executable.ExecutableWorker.initialize(self,worker_data)

        # here goes the caching logic which uses worker_data.input_cache attribute

        # shortcuts
        ftc = self._agent.ftc
        cache = worker_data.input_cache
        options = ftc.Options()
        options.md5_lazy = True # don't download files if md5 checksum is the same

        import stat,time

        timeout = worker_data.policy.BREAK_CACHE_LOCK_TIMEOUT

        for fn in cache.files:
            logger.info('getting file %s to the local cache',fn)
            p = cache.access_path(fn) # location of the cache on the local disk
            dest = os.path.join(p,os.path.basename(fn)) # full path to the file cached on the local disk

            logger.info('getting file %s to the local cache path %s',fn,dest)

            lock = LockFile(dest)

            try:
                # wait for a lock, force it on timeout and try to get it again, ad infinitum
                while not lock.i_am_locking():
                    try:
                        t0 = time.time()
                        logger.info('trying to acquire lock for file %s, timeout = %ds',dest,timeout)
                        lock.acquire(timeout=timeout)
                        logger.info('acquired lock for file %s after %0.3fs',dest,time.time()-t0)
                    except LockTimeout:
                        logger.warning('timeout reached (%ss), breaking lock for file %s',timeout,dest)
                        lock.break_lock()

                logger.info('downloading the file %s from cache (md5check first)',dest)
                ftc.download(dest,fn,options)
            finally:
                logger.info('release lock for file %s',dest)
                lock.release()
                

class ApplicationManager(executable.ExecutableApplicationManager):
    def initialize(self,run_data):
        ret = executable.ExecutableApplicationManager.initialize(self,run_data)
        
        # here we dynamically add input_cache attribute to the worker data
        self.worker_init.input_cache = run_data.input_cache

        # now we must authorize the file server to access cached files
        file_server = self.scheduler.job_master.file_server

        print 'fs',dir(file_server)
        file_server.servant.setAuthorizedDirs(run_data.input_cache.authorized_dirs)
        
        return ret


