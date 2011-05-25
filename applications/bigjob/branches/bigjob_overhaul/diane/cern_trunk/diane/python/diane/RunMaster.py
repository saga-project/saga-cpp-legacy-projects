import diane
logger = diane.getLogger('RunMaster')

import diane.util

import DIANE_CORBA 
import DIANE_CORBA__POA

import sys, time

config = diane.getConfig('RunMaster')
config.addOption('LOST_WORKER_TIMEOUT',60,'timout in seconds to declare worker as "lost"')
config.addOption('CONTROL_DELAY',1,'default periodicity of control loop')
config.addOption('IDLE_WORKER_TIMEOUT', 600, 'if a worker stays idle for this time then it is automatically removed from the pool, 0 means that the worker is never removed')

# --- temporary config and logger workaround
import diane.WorkerRegistry as WorkerRegistryModule
WorkerRegistryModule.config = config
WorkerRegistryModule.logger = logger
# ---

import diane.journal

from diane.TaskInfo import TaskInfo,TaskStatus

from diane.Peer import Peer

# make sure that you always import with full package name (otherwise pickle on the client side will compain)
from diane.WorkerRegistry import WorkerRegistry

import streamer
from streamer import EMPTY as DXP_EMPTY
    
from diane.BaseThread import BaseThread
class ControlThread(BaseThread):
    """ WorkerAgent management thread: check and remove lost workers.
    """
    def __init__(self,master):
        BaseThread.__init__(self,'ControlThread')
        self.master = master
        self.processing = False
        
    def run(self):
        time_start = time.time()
        while not self.should_stop():
            ##lock = self.master.worker_registry.lock

            # detect and remove lost workers
            lost = []
            try:
                ##lock.acquire()
                for wid in self.master.worker_registry:
                    w = self.master.worker_registry.get(wid)
                    try:
                        w.alive_lock.acquire()
                        if w.alive and w.lost():
                            logger.warning('worker wid=%d is lost (last contact %f seconds ago) and will be removed',wid,time.time()-w.last_contact_time)
                            self.master.journal.addEntry('worker_lost',wid=wid,last_contact_time=w.last_contact_time)
                            
                            self.master.remove_worker(w)
                            continue

                        if w.alive:
                            if w.processing_tasks: # no more processing task
                                w.idle_time = 0
                            else:
                                if w.idle_time == 0: # worker was not idle before
                                    w.idle_time = time.time() # so we mark it as such with the current timestamp
                            if w.idle_too_long():
                                logger.warning('worker wid=%d was idle for too long (%f seconds) and will be removed',wid,time.time()-w.idle_time)
                                self.master.journal.addEntry('worker_idle',wid=wid,start_idle_time=w.idle_time)
                                self.master.remove_worker(w)
                                continue
                    finally:
                        w.alive_lock.release()
                        
            finally:
                ##lock.release()
                pass

            # processing ongoing
            if self.processing:
                assert(self.master.task_scheduler)
                if not self.master.task_scheduler.has_more_work():
                    logger.info('task manager: work finished')
                    self.master.journal.addEntry('master_finished')
                    self.master.shutdown()
            logger.debug('control loop time up %d'%(time.time()-time_start))
            time.sleep(config.CONTROL_DELAY)

    def activateProcessing(self):
        self.processing = True


def require_worker_initialized(w,yes=True):
    msg = {True:'is not',False:'already'}
    if w.initialized is not yes:
        logger.error('worker wid=%d %s initialized',w.wid,msg[w.initialized])
        raise DIANE_CORBA.XHangup(DXP_EMPTY) #PENDING: use another exception instead    


class IMasterController:
    """Interface used by TaskScheduler to control task scheduling on the job master. """

    def schedule(self, worker, tasks):
        """Schedule tasks to be consumed by the worker (specified as wid or worker object).
        In case the worker cannot be scheduled tasks (e.g. worker is lost)
        then the tasks which could not be scheduled will be reported via the tasks_unscheduled() callback on the task manager."""
        raise NotImplementedError

    def unschedule(self, worker):
        """Release all scheduled tasks from the worker (specified as wid or worker object).
        The tasks which are currently executed by the worker are not affected.
        Sucessfully unscheduled tasks will be reported via the tasks_unscheduled() callback on the task manager.
        Return the list of succesfully unscheduled tasks. 
        """
        raise NotImplementedError

    def remove_worker(self,worker):
        """ Remove worker (specified as wid or worker object) from the pool and release all unscheduled tasks.
        All currently processing tasks will be declared lost.
        """
        raise NotImplementedError

    def ignore_tasks(self,tasks):
        """ Mark the tasks as ignored.
        """
        raise NotImplementedError

    def create_task(self,application_label=None, application_details=None):
        """ Return a new TaskInfo object with application-specific information.
        """
        raise NotImplementedError


## RunMaster service implementation of DIANE_CORBA.RunMaster interface.
class RunMaster (DIANE_CORBA__POA.RunMaster, IMasterController, Peer):
    """ RunMaster is a service communicating with WorkerAgents and
    dispatching/receiving tasks. The task scheduling is controlled by
    TaskScheduler. WorkerAgent control is performed by ControlThread."""
    
    def __init__(self,rid,server):
        Peer.__init__(self,server,logger)
        
        ## CORBAProgram server object which contains this service.
        self.server = server
        
        ## WorkerRegistry.
        self.worker_registry = WorkerRegistry()
        
        self.__ping_cnt = 0
        
        ## TaskScheduler.
        self.task_scheduler = None

        ## ControlThread,
        self.control_thread = ControlThread(self)

        ## cache for lookup efficiency
        ##self.free_workers = {}
        ##self.received_workers = {}

        # application bootstrap parameters
        self.app_boot_data = None

        self.file_server = None # default file server

        # numeric run identifier
        self.rid = rid

        # uuid
        from diane.util.compatibility import uuid
        self.uuid = str(uuid())
       
        # journal has an option to report events via MSG and uuid is used to identify the master
        self.journal = diane.journal.Journal('master.j', self.uuid)

        # automatically generated task id
        self.__tid = 1
        
    def schedule(self,worker,tasks):
        # access wid or worker object as argument
        if type(worker) is type(1):
            worker = self.worker_registry.get(worker)
            
        assert(worker.initialized)
        try:
            worker.alive_lock.acquire()
            if worker.alive:
                for t in tasks:
                    t.assign(worker.wid)
                    worker.scheduled_tasks.put(t)
                self.worker_registry.update_cache(worker)
                self.journal.addEntry('tasks_scheduled',wid=worker.wid,tids=[t.tid for t in tasks])
            else:
                # we do not add to the journal events which were immediately uncheduled
                tids = [t.tid for t in tasks]
                logger.warning('worker wid=%s: attempt to schedule tasks %s to a worker which is not alive',worker.wid,tids)
                logger.debug('task_scheduler.tasks_unscheduled(%s)',tids)
                self.task_scheduler.tasks_unscheduled(tasks)
        finally:
            worker.alive_lock.release()

    def unschedule(self,worker):
        # access wid or worker object as argument
        if type(worker) is type(1):
            worker = self.worker_registry.get(worker)
            
        assert(worker.initialized)
        try:
            worker.alive_lock.acquire()
            unscheduled_tasks = list(worker.scheduled_tasks.queue)
            for t in unscheduled_tasks:
                t.update(TaskStatus.UNSCHEDULED,None)
            try:
                tids = [t.tid for t in unscheduled_tasks]
                self.journal.addEntry('tasks_unscheduled',wid=worker.wid,tids=tids)
                logger.debug('task_scheduler.tasks_unscheduled(%s)',tids)
                self.task_scheduler.tasks_unscheduled(unscheduled_tasks)
            except Exception,x:
                logger.exception('Error in TaskScheduler.tasks_unscheduled() callback')
            worker.scheduled_tasks.clear()
        finally:
            worker.alive_lock.release()
        
    def remove_worker(self,worker):
        # access wid or worker object as argument
        if type(worker) is type(1):
            worker = self.worker_registry.get(worker)

        #optimization: dead worker may not become alive again, so we do not have to wait on mutex with this check
        if not worker.alive:
            return
        try:
            worker.alive_lock.acquire()
            #check if worker is not dead now
            if not worker.alive:
                return
            self.worker_registry.remove(worker.wid)
            self.worker_registry.update_cache(worker)
            self.journal.addEntry('worker_removed',wid=worker.wid)
            if self.task_scheduler:
                # notify task manager
                try:
                    logger.debug('task_scheduler.worker_removed(w); w.wid=%d,w.worker_uuid=%s'%(worker.wid,worker.worker_uuid))
                    self.task_scheduler.worker_removed(worker)
                except Exception,x:
                    logger.exception('Error in TaskScheduler.worker_removed(wid=%d) callback'%worker.wid)

                if worker.initialized:
                    self.unschedule(worker)
                
                    # handle lost tasks
                    lost_tasks = worker.processing_tasks.values()
                    for t in lost_tasks:
                        t.update(TaskStatus.LOST,None)

                    tids = [t.tid for t in lost_tasks]
                    self.journal.addEntry('tasks_lost',tids=tids)

                    logger.user('worker %d was removed',worker.wid)

                    try:
                        logger.debug('task_scheduler.tasks_lost(%s)',tids)
                        self.task_scheduler.tasks_lost(lost_tasks)
                    except Exception,x:
                        logger.exception('Error in TaskScheduler.tasks_lost() callback')
                    worker.processing_tasks.clear()
        finally:
            worker.alive_lock.release()
      
    def create_task(self,application_label=None, application_details=None):
        t = TaskInfo(self.__tid)
        if application_label:
            t.application_label = application_label
        if application_details:
            t.application_details = application_details

        self.__tid += 1

        logger.user('new_task_created: tid=%d %s %s',t.tid,t.application_label,t.application_details)
        self.journal.addEntry('new_task_created',tid=t.tid,application_label=t.application_label,application_details=t.application_details)
        return t

    def ignore_tasks(self,tasks):
        for t in tasks:
            t.ignore()
            logger.user('ignoring task %d - it will not be further attempted',t.tid)

        self.journal.addEntry('tasks_ignored',tids=[t.tid for t in tasks])

    def bootstrapContacts(self):
        self.control_thread.start()
        #self.server.threads.append(self.control_thread)
        
    def ping(self,peer_uuid,cookie):
        logger.debug('I am being pinged (%s,%s)'%(peer_uuid,cookie))

        #TEST: simulate network delay 
        #time.sleep(10) # set it to a value greater than timeout on the worker agent
        
        if peer_uuid:
            assert(cookie=='worker')
            wid = self._resolve_wid(peer_uuid)
            self.update_contact(wid)
            self.journal.addEntry('worker_ping',wid=wid)
        self.__ping_cnt+=1
        return 0

    def update_contact(self,wid):
        """
        Update worker contact timestamp unless the worker is not registered or not alive.
        """
        
        try:
            logger.debug('update from worker wid=%d, diff to last contact time: %f',wid,time.time()-self.worker_registry.get(wid).last_contact_time)
            worker =  self.worker_registry.get(wid)
            if worker.alive:
                worker.last_contact_time = time.time()
            else:
                logger.info('the worker wid=%d attempts to connect but it has been unregistered (lost or removed), closing session with worker (XHangup)',wid)
                self.journal.addEntry('worker_update_contact_error',wid=wid,msg='worker not alive')
                raise DIANE_CORBA.XHangup(DXP_EMPTY)
        except KeyError:
            logger.warning('received message from worker wid=%d which is not registered, closing session with worker (XHangup)',wid)
            self.journal.addEntry('worker_update_contact_error',wid=wid,msg='worker not registered')
            raise DIANE_CORBA.XHangup(DXP_EMPTY)
        if self.control_thread.processing and not self.task_scheduler.has_more_work(): #PENDING: fix ugly condition
            self.journal.addEntry('worker_update_contact_error',wid=wid,msg='task manager finished')            
            logger.debug('task manager work finished, closing session with worker wid=%d (XHangup)'%wid)
            raise DIANE_CORBA.XHangup(DXP_EMPTY)

    def get_init_data(self,worker_uuid):
        wid = self._resolve_wid(worker_uuid)        
        # during the execution of this method the tasks may not be scheduled
        # to this worker because it is not in the cache waiting list
        logger.debug('get_init_data %d',wid)
        self.update_contact(wid)
        w = self.worker_registry.get(wid)
        require_worker_initialized(w,False)
        self.CNT = 0
        self.journal.addEntry('get_init_data',wid=wid)
        return (self.app_boot_data,streamer.dumps(w.init_input))

    def put_init_result(self,worker_uuid,init_result,error):
        wid = self._resolve_wid(worker_uuid)
        # during the execution of this method the tasks may not be scheduled
        # to this worker because it is not in the cache waiting list        
        init_result = streamer.loads(init_result)
        logger.debug('put_init_result %d %s',wid,repr(init_result))
        self.update_contact(wid)

        w = self.worker_registry.get(wid)
        require_worker_initialized(w,False)
        w.init_output = init_result

        self.journal.addEntry('put_init_result',wid=wid)
        
        try:
            logger.debug('task_scheduler.worker_initialized(w) w.wid=%d w.worker_uuid=%s'%(w.wid,w.worker_uuid))            
            self.task_scheduler.worker_initialized(w)
        except Exception,x:
            logger.exception('Error in TaskScheduler.worker_initialized() callback')
        w.initialized = True
        logger.user('worker %d has been initialized and is now ready',w.wid)
        # the cache gets updates here and the worker enters the waiting list
        self.worker_registry.update_cache(w)
        # so only from now on the tasks may be scheduled to this worker
                
        
    def get_task_data(self,worker_uuid):
        wid = self._resolve_wid(worker_uuid)
        logger.debug('get_task_data %d',wid)
        self.journal.addEntry('get_task_request',wid=wid)
        try:
            worker_entry = self.worker_registry.get(wid)
            worker_entry.alive_lock.acquire()

            self.update_contact(wid)
            require_worker_initialized(worker_entry)
                  
            try:
                #TEST: time.sleep(1)
                import Queue
                task_info = worker_entry.scheduled_tasks.get(block=False)
                logger.debug('removed from scheduled_tasks queue:%d,%s',task_info.tid,repr(task_info.task_input))
            except Queue.Empty:
                self.journal.addEntry('get_task_data_error',wid=wid,msg=1)
                raise DIANE_CORBA.XRepeatCall(0)
            else:
                worker_entry.processing_tasks[task_info.tid] = task_info
                self.worker_registry.update_cache(worker_entry)
                task_info.details.time_start = time.time()
                self.journal.addEntry('get_task_data',wid=wid,tid=task_info.tid)
                return (task_info.tid,streamer.dumps(task_info.task_input))
        finally:
            worker_entry.alive_lock.release()
            
    def put_task_result(self,worker_uuid,tid,task_result,error):
        task_result = streamer.loads(task_result)
        wid = self._resolve_wid(worker_uuid)
        logger.debug('put_task_data %d %d %s',wid,tid,repr(task_result))
        self.journal.addEntry('put_task_result_request',wid=wid,tid=tid,error=error)
        
        try:
            worker_entry = self.worker_registry.get(wid)
            worker_entry.alive_lock.acquire()

            self.update_contact(wid)
            require_worker_initialized(worker_entry)

            # protect against multiple calls from the same worker with the same task
            # this may happen not only because of the login error in the Worker Agent
            # running lattice qcd application I observed TRANSIENT exception on the worker
            # but the call apparently made it to the master
            try:
                task_info=worker_entry.processing_tasks[tid]
            except KeyError:
                logger.debug('ignored multiple call to put_task_data() %d %d',wid,tid)
                return
            
            task_info.details.time_finish = time.time()
            del worker_entry.processing_tasks[tid]

            self.journal.addEntry('put_task_result',wid=wid,tid=tid,error=error)
            
            if error:
                task_info.update(TaskStatus.FAILED,task_result)
                try:
                    logger.debug('task_scheduler.tasks_failed(%s)'%str([task_info.tid]))
                    logger.warning('task %s (%s) failed: %s',task_info.tid,repr(task_info.application_label),task_result)
                    self.task_scheduler.tasks_failed([task_info])
                except Exception,x:
                    logger.exception('Error in TaskScheduler.tasks_failed() callback')
            else:
                logger.user('task %s completed (application_label=%s)'%(task_info.tid,repr(task_info.application_label)))
                task_info.details.task_output = task_result
                task_info.update(TaskStatus.COMPLETED,None)
                try:
                    logger.debug('task_scheduler.tasks_completed(%s)'%str([task_info.tid]))
                    self.task_scheduler.tasks_completed([task_info])
                except Exception,x:
                    logger.exception('Error in TaskScheduler.tasks_completed() callback')

            self.worker_registry.update_cache(worker_entry)
        finally:
            worker_entry.alive_lock.release()

    def _resolve_wid(self,worker_uuid):
        try:
            w = self.worker_registry.get_by_uuid(worker_uuid)
        except KeyError:
            logger.warning('I got request from a worker which is not registered, uuid=%s',worker_uuid)
            raise DIANE_CORBA.XHangup(DXP_EMPTY)
        return w.wid
        
    def runid(self):
        import getpass
        import diane.util.hostname
        import diane.workspace
        return "%s@%s:%s"%(getpass.getuser(),diane.util.hostname.hostname(),diane.workspace.getRundir(runid=self.rid))
        
    def startProcessing(self,input):
        import diane.util

        from diane.config import log_configuration
        log_configuration()


        #start the default file server
        import diane.FileTransfer        
        self.file_server = diane.FileTransfer.Server.main('FileTransferOID',self.server)

        # FIXME: segmentation fault if object reference is passed directly, workaround via stringified IOR
        self.file_server_ior = self.server.orb.object_to_string(self.file_server)

        import os
        self.journal.addEntry('master_start',runid=self.runid(), application_name=input.application.__name__, name=os.path.basename(input._runfile))


        # prepare application boot

        import application
        boot_data = application.make_boot_data(input)
        boot_data.runid = self.runid()
        boot_data.master_uuid = self.uuid
        self.app_boot_data = streamer.dumps(boot_data)
        




        # TEST: trigger race condition with registerWorker(): a fast worker registers before this method is completed
        # this problem should be fixed now
        #import time
        #time.sleep(5)

        def thread_crash_handler(t):
            try:
                logger.debug('Crash handler started: %s',t.__class__.__name__)
                return t._run()
            except Exception,x:
                logger.exception('Information from crash handler (%s): unhandled exception: %s',t.__class__.__name__,x)
                logger.info('Stopping the RunMaster and dumping the state into the "crash-dump.pickle" file')
                import pickle
                pickle.dump(self.worker_registry,file("crash-dump.pickle",'w'))
                self.shutdown()

        def funcToMethod(func,clas,method_name=None): #cookbook recipe 5.12
            setattr(clas,method_name or func.__name__,func)

        def installCrashHandler(clas):
            try:
                funcToMethod(clas.run,clas,"_run") # _run <- run
                funcToMethod(thread_crash_handler,clas,"run") # run <- thread_crash_handler
            except AttributeError:
                pass
            
        mgr = None
        if input.manager:
            mgr = input.manager()

        self.task_scheduler = input.scheduler(self,mgr)

        installCrashHandler(mgr.__class__)
        installCrashHandler(self.task_scheduler.__class__)
        
        self.task_scheduler.initialize(input.data)
        self.task_scheduler.log_configuration()
        self.task_scheduler.start()
        #self.server.threads.append(self.task_scheduler)


        #instruct the control thread that task manager is in place
        self.control_thread.activateProcessing()
        
    def shutdown(self):
        self.server.stop()

    def getStatusReport(self):
         status_summary = self.worker_registry.getStatusReport()
         import cPickle
         return cPickle.dumps(status_summary)

    def getStatusPickle(self):
        return self.worker_registry.getStatusPickle()
    
    def registerWorker(self,worker_uuid,capacity):

        # protect against initial race condition
        if not self.control_thread.processing:
            logger.debug('attempt to register a worker but processing not yet started (worker id is %s)',worker_uuid)
            raise DIANE_CORBA.XRepeatCall(0)
            
        # during the execution of this method the tasks may not be scheduled
        # to this worker because it is not in the cache waiting list
        logger.info('registering worker')
        w = self.worker_registry.add(worker_uuid,capacity)
        if w is None: # registration not sucessful (worker already registred)
            # send it a hangup signal: it is either my worker which is in a funny state
            # or it is another worker with duplicate worker_uuid
            logger.info('closing session with worker %s (XHangup)',worker_uuid)
            raise DIANE_CORBA.XHangup(DXP_EMPTY)

        import diane.workspace
        diane.workspace.makeSubdir(diane.workspace.workerSubdir(w.wid),self.rid)

        self.journal.addEntry('worker_registered',wid=w.wid,worker_uuid=worker_uuid)
        
        try:
            logger.debug('task_scheduler.worker_added(w) w.wid=%d w.worker_uuid=%s'%(w.wid,w.worker_uuid))
            self.task_scheduler.worker_added(w)
        except Exception,x:
            logger.exception('Error in TaskScheduler.worker_added() callback')

        return w.wid,self.file_server_ior
        
def createServant(rid,server):
    
    from omniORB.BiDirPolicy import BIDIRECTIONAL_POLICY_TYPE, BOTH
    from omniORB.PortableServer import PERSISTENT, USER_ID, RETAIN
    
    policy = [server.root_poa.create_lifespan_policy(PERSISTENT),
              server.root_poa.create_id_assignment_policy(USER_ID),
              server.root_poa.create_servant_retention_policy(RETAIN),
              server.orb.create_policy(BIDIRECTIONAL_POLICY_TYPE,BOTH)
              ]

    #TODO: make a special locator object which may be used to lookup the master by corbaloc:
    #for the moment usage of corbaloc: is not supported
    #locator_poa = server.orb.resolve_initial_references("omniINSPOA")
    
    poa = server.root_poa.create_POA("RunMasterPOA",server.root_poa_manager,policy)
    poa._get_the_POAManager().activate()

    master_servant = RunMaster(rid,server)
    
    oid = "RunMaster"
    poa.activate_object_with_id(oid,master_servant)        
    master = master_servant._this()

    return master_servant


def main(rid,input,omniorb_config_file,enable_GSI): # directory_service_oid
    from CORBAProgram import CORBAProgram
    server = CORBAProgram(server=True,config_file=omniorb_config_file,enable_GSI=enable_GSI)

    master_servant = createServant(rid,server)
    master = master_servant._this()
    ior_filename = "MasterOID"
    file(ior_filename,"w").write(server.orb.object_to_string(master))
    logger.info("RunMaster activated, IOR in file %s",ior_filename)    

    master_servant.bootstrapContacts()
    master_servant.startProcessing(input)
    server.start()
    
