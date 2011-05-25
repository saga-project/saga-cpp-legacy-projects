import atexit, diane, gzip, logging, os, pickle, random, sys, threading, time
from diane.util import importName

#store lockfile, timestamp data and error logs in gangadir/agent_factory
DATA_PATH = os.path.join(config.Configuration.gangadir, 'agent_factory')

#pickled timestamps
AGENT_FACTORY_DATA_FILE = os.path.join(DATA_PATH, 'agent_factory_data')

#failure_log directory to store the information about the failed jobs
FAILURE_LOG_DIR = os.path.join(DATA_PATH, 'failure_log')

#setup the logger
logger = diane.getLogger('agent_factory')

#exception definitions for the FLock class 
class LockAcquireError(Exception):
    pass

class LockReleaseError(Exception):
    pass

class FLock(object): 
    """A simple file lock class. Creates a directory (since mkdir on UNIX is atomic) which serves as a lock and allows detecting previous instances of the application."""
    def __init__(self, path):
        self.path = path
        self.filename = 'lockfile'
        self.pid = os.getpid()
        self.lockfile = os.path.join(self.path, self.filename + '_' + str(self.pid))

    def acquire(self):
        """Creates a directory serving as a lock file. The name of the directory is lockfile_PID where PID is the process id of the process that wants to acquire the lock."""
        #check the lockfile for the current process
        if os.path.exists(self.lockfile):
            logger.error('Lockfile for the current agent_factory process already exists')
            raise LockAcquireError
        #check the lockfile for another process
        files = []
        if os.path.exists(self.path):
            files = os.listdir(self.path)
        for f in files:
            if f.startswith(self.filename):
                logger.error('Lockfile for another agent_factory process already exists')
                raise LockAcquireError
        logger.info('Acquiring lockfile: %s', self.lockfile)
        os.makedirs(self.lockfile)

    def release(self):
        """Releases the acquired lock. Raises an exception if an attempt is made to release a lock which wasn't acquired first."""
        if not os.path.exists(self.lockfile):
            #trying to release a lock which wasn't acquired; raise an exception
            logger.error('Attempting to release a lock which doesn\'t exist')
            raise LockReleaseError 
        logger.info('Releasing lockfile: %s', self.lockfile)
        os.rmdir(self.lockfile) 

class AgentFactory:
    def __init__(self,submitterName): 
        #dictionary to store timestamps of various objects
        self._timestamp = {}
        self._start_time = time.time()
        self.killswitch = os.path.join(DATA_PATH, 'kill_' + str(os.getpid())) #used to terminate the process (for example when running on cron)

        Submitter = importName(submitterName, submitterName)
        if Submitter is None:
            Submitter = importName('diane.submitters.' + submitterName, submitterName)
        if Submitter is None:
            logger.warning('Can\'t find %s submitter.', submitterName)
            return

        self.submitter=Submitter()
        self.submitter.parser.description = self.submitter.parser.description
        #update the help of --diane-worker-number option as now it has a slightly different meaning
        self.submitter.parser.get_option('--diane-worker-number').help = 'the minimum number of worker nodes to keep alive'
        self.submitter.parser.add_option('--repeat-interval', dest='REPEAT', type='int', default=0, help='time in secons before re-checking the number of workers. If not specified (e.g. for use with cron) the operation will be performed only once')
        self.submitter.parser.add_option('--run-time', dest='RUNTIME', type='int', default=0, help='time in seconds to run the script. After RUNTIME hours have passed, the script will terminate. This option is indented to be used with acrontab')
        self.submitter.parser.add_option('--diane-max-pending', dest='MAX_PENDING', type='int', default=20, help='the maximum number of workers which are waiting to be run. If the number of waiting workers exceeds MAX_PENDING no more workers will be submitted until the already submitted ones are run')
        self.submitter.parser.add_option('--pending-timeout', dest='TIMEOUT', type='int', default=43200, help='maximum time in seconds to keep pending jobs. When the job gets too old it is removed and new one is submitted instead. This is to avoid jobs beign stuck in the queue, etc.')
        self.submitter.parser.add_option('--purge-timeout', dest='PURGE_TIMEOUT', type='int', default=86400, help='maximum time in seconds to keep the information about old (finished) jobs. After timeout is exceeded, the job is removed from Ganga repository')
        self.submitter.parser.add_option('--lockfile_path', dest='LOCKFILE_DIR', type='string', default=DATA_PATH, help='specify where to keep the lockfile')
        self.submitter.parser.add_option('--kill', action='store_true', dest='KILL', default=False, help='kills the running instance of the directory service')
        self.submitter.parser.add_option('--square-fitness', action='store_true', dest='SQUARE_FITNESS', default=False, help='squares the fitness to increase the penalty factor')
        
        self.submitter.initialize()

        if not self.submitter.options.N_WORKERS > 0:
            logger.warning('You must specify the minimum number of workers to keep alive. Run with --help to see possible options.')
            return 

        #check for request to kill the process
        if self.submitter.options.KILL:
            logger.info('Kill command received; attempting to kill the agent_factory process')
            files = []
            if os.path.exists(self.submitter.options.LOCKFILE_DIR):
                files = os.listdir(self.submitter.options.LOCKFILE_DIR)
                for f in files:
                    if f.startswith('lockfile_'):
                        os.popen('touch ' + os.path.join(self.submitter.options.LOCKFILE_DIR, f.replace('lockfile', 'kill')))
            sys.exit(0)

    def timestamp(self,item): 
        """ Return the timestamp for a given item. Create one if no timestamp exist. """
        try:
            self._timestamp[item]
        except:
            self._timestamp[item] = time.time()
        return self._timestamp[item] 

    def age(self,item): 
        try:
            self._timestamp[item]
        except:
            self._timestamp[item] = time.time()
        return time.time() - self._timestamp[item] 

    def run(self):
        #start the monitoring component
        import Ganga.Core
        Ganga.Core.monitoring_component.enableMonitoring()
    
        #store the ids of submitted jobs to monitor their status
        pending = []
        for j in jobs:
            self.timestamp(j.id)
            pending.append(j)

        #load the timestamps from the previous run
        if os.path.isfile(AGENT_FACTORY_DATA_FILE):
            f = file(AGENT_FACTORY_DATA_FILE)
            try:
                self._timestamp = pickle.loads(file(AGENT_FACTORY_DATA_FILE).read())
            finally:
                f.close()

        #the list of all CEs to which jobs have been submitted
        CEs = []

        if self.submitter.options.SQUARE_FITNESS:
            logger.info('SQUARE_FITNESS option enabled')

        #keep the number of workers above specified threshold
        while True:
            #check for kill command
            if self.should_stop():
                logger.info('Kill command received. Terminating process')
                os.remove(self.killswitch)
                sys.exit(0)
            #if --runtime option is specified
            if self.submitter.options.RUNTIME > 0 and (time.time() - self._start_time) > self.submitter.options.RUNTIME:
                logger.info('running time exceeded %.1fh; terminating script.' , (self.submitter.options.RUNTIME / 3600))
                sys.exit(0)

            #if running the script only once, run the monitoring loop explicityly
            if self.submitter.options.REPEAT == 0:
                import Ganga.GPI
                Ganga.GPI.runMonitoring()
        
            try:
                alive=self.alive_worker_number()
                logger.info('%d workers alive', alive)

                #check the status of pending jobs and remove failed/completed from the list
                to_remove = []
                for j in pending:
                    #add the ce to the list
                    if j.backend.actualCE is not None:
                        if j.backend.actualCE not in CEs:
                            CEs.append(j.backend.actualCE)

                    if j.status == 'running' or j.status == 'completed' or j.status == 'failed':
                        to_remove.append(j)
                
                for j in to_remove:
                    #remove the job from the pending list
                    pending.remove(j)           

                #classify jobs as completed (without errors) and failed (with errors)
                completed, failed = [], []
                for j in [j for j in jobs.select(status='failed')] + [j for j in jobs.select(status='completed')]:
                    #to make the script more adaptable, remove the information abou the old (failed/completed) jobs
                    if self.age(j.id) > self.submitter.options.PURGE_TIMEOUT: #if the job information is more than 2 days old
                        logger.info('Removing the information about the old job %d', j.id)
                        j.remove()
                    # check if the job failed or completed without errors
                    if is_failed(j):
                        failed.append(j)
                    else:
                        completed.append(j)

                #check jobs with status 'submitted' (still waiting to be run) and purge them if they are outdated
                for j in jobs.select(status='submitted'):
                    if self.age(j.id) >= self.submitter.options.TIMEOUT:
                        logger.info('Removing job %d from the queue; %.1fh timeout exceeded...', j.id, (self.submitter.options.TIMEOUT / 3600))
                        pending.remove(j)
                        j.remove()

                #print simple summary
                logger.info('Summary: data format = ( alive, pending, remaining, completed, failed )')
                logger.info('[ %d, %d, %d, %d, %d ]', alive, len(pending), max(self.submitter.options.N_WORKERS - (alive + len(pending)), 0), len(completed), len(failed))
                
                alive += len(pending)
                
                fitness = self.compute_fitness(CEs, completed) 

                while alive<self.submitter.options.N_WORKERS and len(pending) < self.submitter.options.MAX_PENDING:
                    j, ce = Job(), self.next_ce(CEs, fitness)
                    if ce is not None:
                        logger.info('Submitting job %d to %s', j.id, ce)
                    else:
                        logger.info('Submitting job %d to the grid', j.id)
                    #TODO: Change this a bit; submitting should be abstracted; it makes no sense to specify ce if say were submitting to Local backend 
                    self.submitter.submit_worker(j, ce=ce) #specify the Computing Element; if the backend doesn't have CE the submitter may simply ignore the keyword parameter
                    pending.append(j)
                    self.timestamp(j.id) #store the timestamp (for purging stucked jobs)
                    alive+=1

                #store the timestamp information before exiting/repeating the loop
                f = open(AGENT_FACTORY_DATA_FILE, 'w')
                try:
                    f.write(pickle.dumps(self._timestamp))
                finally:
                    f.close()

                if self.submitter.options.REPEAT > 0:
                    time.sleep(self.submitter.options.REPEAT)
                else:
                    return

            except GangaException, x:
                logger.exception('ERROR: Error occurred during job submission: %s', x)

    def compute_fitness(self, CEs, completed): 
        """ Compute the fitness of each Computing Element as a ratio of running and completed (without errors) jobs to the total number of jobs """
        #pick the ce with a probability proportional to the ratio of completed and running jobs to all jobs; log computing element statistics
        logger.info('Computing Element statistics: data format = ( name, fitness, running, completed, total )')
        fitness = []
        for ce in CEs:
            #total number of jobs
            _total = [j for j in jobs if j.backend.actualCE == ce]
            #completed successfully
            _completed = [j for j in _total if j in completed]
            #currently running
            _running = [j for j in _total if j.status == 'running']
            if len(_total) == 0:
                fitness.append(float(1))
            else:
                _fitness = float(len(_completed) + len(_running)) / float(len(_total))
                if self.submitter.options.SQUARE_FITNESS:
                    fitness.append(_fitness * _fitness) #square the fitness to increase the penalty factor
                else:
                    fitness.append(_fitness)
            logger.info('{ %s, %f, %d, %d, %d }', ce, fitness[-1], len(_running), len(_completed), len(_total))
            #logger.info('{ ' + str(ce) + ', fitness=' + str(fitness[-1]) + ', [' + str(len(_completed)) + 'c/' + str(len(_running)) + 'r/' + str(len(_total)) + 't] }')
        return fitness 

    def next_ce(self, ces, fitness): 
        """ Choose the next Computing Element to submit the job to. Each CE has a fitness value assigned to it which is the ratio of running and completed (without errors) jobs to the total number of jobs sent to that CE. Selection is based on this value and a CE is chosen with a probability proportional to the fitness it has. Additionally, a job may be submitted without specifying a CE explicitly and in this case it will be up to the grid to decide on which CE should the job run. """
        total_fitness = sum(fitness)
        #each CE has a 0 <= fitness <= 1 fitness level assigned, so the sum(fitness) is at most len(CEs); since
        #we want to leave some decision making to the grid itself we generate a random number in the range between
        #0 and total_fitness + 1 which gives us the property that the 'let the grid choose' option is at least as
        #likely as choosing any of the other backends (and probably more likely in practice)
        #happens to be in that region 
        ce = None
        r = random.uniform(0, total_fitness + 1)
        for i in range(len(fitness)):
            r -= fitness[i]
            if r <= 0:
                ce = ces[i]
                break
        return ce 

    def alive_worker_number(self): 
        """ Return the number of alive workers """
        try:
            reg=pickle.load(os.popen('diane-master-ping -f ' + self.submitter.master_oid_filename + ' getStatusReport - 2> /dev/null'))
        except Exception, x:
            logger.error('Unable to contact the master %s\nError: %s', self.submitter.master_oid_filename, x)
            # sys.exit(1) # log the error and continue working; there is no need to terminate the script
        return reg.alive_workers_number 

    def should_stop(self):
        return os.path.exists(self.killswitch)

################################################################################
# UTILITY METHODS
################################################################################

def is_failed(job): 
    """ Check the stderr.gz for indication of errors (keywords: error, exception) """
    failed = False

    JOB_LOG_DIR = os.path.join(FAILURE_LOG_DIR, str(job.id))
    if os.path.exists(JOB_LOG_DIR):
        return True #if the directory exists, we have already checked the job and there is no need to check it again
    
    if os.path.exists(os.path.join(job.outputdir, 'stderr.gz')):
        f = gzip.open(os.path.join(job.outputdir, 'stderr.gz'))
        try:
            #if the stderr.gz file contains a word 'error' or 'exception' assume the job has failed
            for line in f:
                if 'error' in line.lower() or 'exception' in line.lower():
                    failed = True
        finally:
            f.close()
    else:
        #if status is completed or failed and no outputdir exsits, assume the job failed before it could transfer the files
        failed = True

    #if the job has a failed status, store the log information in the failure_log directory
    if failed:
        os.makedirs(JOB_LOG_DIR)

        f = open(os.path.join(JOB_LOG_DIR, 'full_print'), 'w')
        try:
            full_print(job, f)
        finally:
            f.close()

        log_file = job.backend.loginfo()
        if log_file is not None:
            os.popen('cp ' + log_file + ' ' + os.path.join(JOB_LOG_DIR, 'loginfo'))
        else:
            os.popen('echo "loginfo unavailable, job.backend.loginfo() returned None" >> ' + os.path.join(JOB_LOG_DIR, 'loginfo_error'))
        
        os.popen('cp -r ' + job.outputdir + ' ' + JOB_LOG_DIR)

    return failed 

################################################################################
# MAIN
################################################################################

def main(submitterName):
    factory = AgentFactory('LCGSubmitter')

    #create a lock file
    lock = FLock(factory.submitter.options.LOCKFILE_DIR)

    #acquire lock
    try:
        lock.acquire()
        atexit.register(lock.release)
    except LockAcquireError:
        logger.error("Unable to acquire the lock file. Another instance of the agent_factory might be running")
        return

    #create the directory to store the data
    if not os.path.exists(DATA_PATH):
        os.makedirs(DATA_PATH)

    #setup the additional file handler to log to file
    hdlr = logging.FileHandler(os.path.join(DATA_PATH, 'log'))
    formatter = logging.Formatter('%(asctime)s %(levelname)s: %(message)s')
    hdlr.setFormatter(formatter)
    logger.addHandler(hdlr)

    #run the agent_factory
    factory.run()

################################################################################
# RUN AS A STANDALONE SCRIPT
################################################################################

if __name__ == '__main__':
    main('LCGSubmitter')
