
from diane.analysis_tools.analyzer import JournalAnalyzer, TimeSlicer, MultipleAnalyzer
import diane.analysis_tools.analyzer

class WorkerStats:
    def __init__(self):
        self.registered = 0
        self.removed = 0
        self.task_cnt = 0

class WorkerHistoryAnalyzer(JournalAnalyzer):
    def __init__(self,time):
        self.time=time
        self.workers = {}

        self.n_added_workers_per_time_unit = [0] # all workers which came new and pulled a task in a given time unit
        self.n_removed_workers_per_time_unit = [0] # all workers which were removed in a given time unit
        self.n_invalid_workers_per_time_unit = [0] #a subset of n_added_workers_per_time_unit which never managed to do anything useful
        self.n_invalid_workers = [0] # number of invalid workers which where in the pool in a given time unit
        self.n_workers = [0] # number of workers which were in the pool in a given time unit

        self.snapshot_wid_map = {}
        self.reset_counters = False

        self.invalid_workers_ids = []
        
    def new_window(self):
        """This method is called when the main event loop moved to the new time window."""
        if self.reset_counters:
            self.reset_counters = False
            v = 0
        else:
            v = self.n_workers[-1]

        self.n_workers.append(v)

        self.n_added_workers_per_time_unit.append(0)
        self.n_removed_workers_per_time_unit.append(0)
        self.n_invalid_workers_per_time_unit.append(0)

        self.n_invalid_workers.append(0) #filled backwards by the future events

    def event_master_shutdown(self,t,p):
        #next time window should start from scratch
        self.reset_counters = True 

    def event_register_worker(self,t,p):
        w = WorkerStats()
        w.registered = t
        if self.workers.has_key(p.wid):
            print "WARNING: worker %s already registered, MULTIPLE WORKER ID!"%p.wid
        self.workers[p.wid] = w
        self.n_added_workers_per_time_unit[-1] += 1
        self.n_workers[-1] += 1

    def event_task_scheduled(self,t,p):
        self.snapshot_wid_map[(p.beta,p.seed)] = p.wid

    def event_task_result_processed(self,t,p):
        wid = self.snapshot_wid_map[(p.beta,p.seed)]
        self.workers[wid].task_cnt += 1

    def event_worker_removed(self,t,p):
        w = self.workers[p.wid]
        w.removed = t
        self.n_removed_workers_per_time_unit[-1] += 1
        self.n_workers[-1] -= 1

        if w.task_cnt == 0: #invalid worker (did not do any job)            
            self.invalid_workers_ids.append(p.wid)
            idx = int((self.time()-self.workers[p.wid].registered)/self.time.window_size)
            self.n_invalid_workers_per_time_unit[-1-idx] += 1
            #fix statistics in the selected time window (may be in the past)
            for i in range(idx+1):
                self.n_invalid_workers[-1-i] += 1

    def postprocess(self):
        """This method is called at the end of the event processing."""
        #

        

        print 'all workers',len(self.workers)
        print 'max wid', max(self.workers.keys())

        #cross-check
        all_workers_cnt = 0
        for e in self.n_added_workers_per_time_unit:
            all_workers_cnt += e

        print 'all_workers_cnt',all_workers_cnt
        assert all_workers_cnt==len(self.workers)

        invalid_workers_cnt = 0    
        for e in self.n_invalid_workers_per_time_unit:
            invalid_workers_cnt += e

        print 'invalid workers',invalid_workers_cnt

from diane.analysis_tools.stats import Histogram1D, QuantileFinder

# hardcoded knowledge!
ITERATIONS_PER_TASK = 3

# select BETA,SEED events to print more verbose information (debugging)
#BETA,SEED='5.1880','2345'
#BETA,SEED='5.1815',4942
BETA,SEED='',0

class TaskExecutionAnalyzer(JournalAnalyzer):
    def __init__(self,time):
        self.time = time
        self.task_duration = {} #per beta
        self.task_duration_histogram = {} #per beta, filled in the postprocessing step

        self.total_iterations = {} # per beta

        self.n_snapshots_per_time_unit = [0]  # number of snapshots produced in a time unit
        self.n_snapshots = [0] # number of snapshots produced so far

        # NEED TO INITIALIZE WITH ALL BETA VALUES!
        self.n_beta_snapshots_per_time_unit = {} # number of snapshots per beta produced in a time unit
        self.n_beta_snapshots = {} # number of snapshots per beta produced so far

        self.USE_FILESERVER_EVENTS = False
        self.task_started = {}

        self.snapshot_upload_duration = Histogram1D(0,100,25,'snapshot upload duration')
        self.snapshot_download_duration = Histogram1D(0,100,25,'snapshot download duration')

        def add_qfinder(h):
            h.qfinder = QuantileFinder()

        
        add_qfinder(self.snapshot_upload_duration)
        add_qfinder(self.snapshot_download_duration)

        # auxiliary dictionary of lqcd_tasks to compute overhead as difference between stop_upload and skip_download
        self.task_master_overhead = Histogram1D(0,1000,50,'master task handling + M/W communication overhead')
        self.tmp_lqcd_task_overhead = {}


    def new_window(self):

        def duplicate_last_element(v):
            v.append(v[-1])
        
        duplicate_last_element(self.n_snapshots)
        self.n_snapshots_per_time_unit.append(0) # zero for the next time unit

        for beta in self.n_beta_snapshots:
            self.n_beta_snapshots_per_time_unit[beta].append(0)
            duplicate_last_element(self.n_beta_snapshots[beta])

    #we will use the information from file.log and master.log in order to have the maximally broad
    #coverage of run periods (e.g. run2 miss the master.log at the beginning)
    #therefore we will use the task_started dict to protect double counting
    def event_task_scheduled(self,t,p):
        self.task_started[(p.beta,p.seed)] = t
        if (p.beta,p.seed) == (BETA,SEED):
            print 'event_task_scheduled',p.beta,p.seed

    def get_beta_seed(self,fn,tmp=False):
        #depending on the upload or download the filename is different (tmp part appended)
        try:
            if tmp:
                beta,seed = fn.split('_')[-3:-1]
            else:
                beta,seed = fn.split('_')[-2:] #FIXME: add assert on len
            seed = int(seed)
            return beta,seed
        except ValueError:
            return None,None
        
    def event_start_download(self,t,p):
        if not self.USE_FILESERVER_EVENTS:
            return

        beta,seed = self.get_beta_seed(p.fn)
        if beta is None:
            return

        self.task_started[(beta,seed)] = t
        if (beta,seed) == (BETA,SEED):
            print 'event_start_download',beta,seed

    def event_stop_download(self,t,p):
        beta,seed = self.get_beta_seed(p.fn)
        if beta is None:
            return
        self.snapshot_download_duration.fill(p.elapsed_time)
        self.snapshot_download_duration.qfinder.fill(p.elapsed_time)

    def event_skip_download(self,t,p):

        beta,seed = self.get_beta_seed(p.fn)
        if beta is None:
            return

        #calculate task handling overheads
        try:
            self.task_master_overhead.fill(t-self.tmp_lqcd_task_overhead[(beta,seed)])
        except KeyError:
            print 'WARNING: problem calclulating task overhead for skip_download',t,p

        if not self.USE_FILESERVER_EVENTS:
            return
        self.task_started[(beta,seed)] = t
        if (beta,seed) == (BETA,SEED):
            print 'event_skip_download',beta,seed

    def count_task_finished(self,t,beta,seed):
        try:
            t_start = self.task_started[(beta,seed)]
            del self.task_started[(beta,seed)]
        except KeyError:
            if (beta,seed) == (BETA,SEED):
                print 'skipped',beta,seed
            return #possibly double counted
        except Exception,x:
            print x, beta, thread

        if (beta,seed) == (BETA,SEED):
            print 'finished',beta,seed
            
        self.n_snapshots_per_time_unit[-1] += ITERATIONS_PER_TASK
        self.n_snapshots[-1] += ITERATIONS_PER_TASK

        self.task_duration.setdefault(beta,[])
        self.task_duration[beta].append(t-t_start)

        self.total_iterations.setdefault(beta,0)
        self.total_iterations[beta] += ITERATIONS_PER_TASK

    def event_task_result_processed(self,t,p):
        if (p.beta,p.seed) == (BETA,SEED):
            print 'event_task_result_processed',repr(p.beta),repr(p.seed)
        self.count_task_finished(t,p.beta,p.seed)

    def event_stop_upload(self,t,p):
        beta,seed = self.get_beta_seed(p.fn,tmp=True)
        if beta is None:
            return

        self.tmp_lqcd_task_overhead[(beta,seed)] = t

        if (beta,seed) == (BETA,SEED):
            print 'event_stop_upload',beta,seed
        #print 'upload',p.fn,p.elapsed_time
        self.snapshot_upload_duration.fill(p.elapsed_time)
        self.snapshot_upload_duration.qfinder.fill(p.elapsed_time)

        if not self.USE_FILESERVER_EVENTS:
            return
        self.count_task_finished(t,beta,seed)

    def postprocess(self):
        print 'number of betas:', len(self.total_iterations)
        print 'betas=',self.total_iterations.keys()

        def make_quantiles(h):
            h.qfinder.make_quantiles([5,25,50,75,95])

        make_quantiles(self.snapshot_upload_duration)
        make_quantiles(self.snapshot_download_duration)
       


    def book_histos(self):

        # 5.1820 is neither high nor low
        def is_low(b):
            return b<beta_c

        def is_high(b):
            return b>beta_c2
        beta_c = 5.1818
        beta_c2 = 5.1821

        self.task_duration_histogram_low_beta = Histogram1D(0,50000,100,'task execution time, $\\beta<%f$, [s] '%beta_c)

        self.task_duration_histogram_high_beta = Histogram1D(0,50000,100,'task execution time, $\\beta>%f$, [s] '%beta_c)

        
        for beta in self.task_duration:
            h = Histogram1D(0,50000,100,'task execution time, $\\beta=%s$, [s]'%beta)
            h.qfinder = QuantileFinder()
            self.task_duration_histogram[beta]=h
            for d in self.task_duration[beta]:
                h.fill(d,ITERATIONS_PER_TASK)
                h.qfinder.fill(d)
                if is_low(float(beta)):
                    self.task_duration_histogram_low_beta.fill(d,ITERATIONS_PER_TASK)
                if is_high(float(beta)):
                    self.task_duration_histogram_high_beta.fill(d,ITERATIONS_PER_TASK)

            h.qfinder.make_quantiles([5,25,50,75,95])

##################################################################

def main(argv=None):
    time_slicer = TimeSlicer()
    # change time window here if needed
    #time_slicer.window_size = 3600*2

    analyzer = MultipleAnalyzer(time_slicer)
    analyzer.add('worker_history',WorkerHistoryAnalyzer(time_slicer))
    analyzer.add('task_execution',TaskExecutionAnalyzer(time_slicer))

    analyzer = diane.analysis_tools.analyzer.main(analyzer,argv)

    # extra step, using intermediate data extracted from journal
    analyzer.task_execution.book_histos()

    import os.path

    # THIS FILE SHOULD BE RENAMED TO: RUN_NN_INVALID_WORKERS_IDS.dat
    try:
        iwf = file('invalid_workers_'+os.path.basename(analyzer.filename),'w')
        print >> iwf, analyzer.worker_history.invalid_workers_ids
        iwf.close()
        print 'written file:',iwf.name
    except AttributeError:
        print 'WARN: cannot find invalid_workers_ids in the analyzer object'
        pass

    return analyzer

# handy functions (BEWARE: run1 and run2 are not complete so numbers are lower!)
# run python -i analyzers.py run.j 

def total_duration():
    total = 0
    for td in analyzer.task_execution.task_duration.values():
        total += reduce(lambda x,y:x+y, td)
    return total

def total_iterations():
    return reduce(lambda x,y:x+y,analyzer.task_execution.total_iterations.values())

def total_workers():
    analyzer.worker_history.postprocess()

if __name__ == "__main__":
    analyzer = main()




