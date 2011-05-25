import diane
logger = diane.getLogger('WorkerAgent')
config = diane.getConfig('WorkerAgent')
logger.debug('original config %s',repr(config))
config.addOption('HEARTBEAT_DELAY',10,"default periodicity of heartbeat")
config.addOption('HEARTBEAT_TIMEOUT',30,"timeout for heartbeat calls, if a heartbeat call may not be completed in HEARTBEAT_TIMEOUT seconds, we assume that the peer (master or directory service) is lost")
config.addOption('BOOTSTRAP_CONTACT_TIMEOUT',30,"timeout for bootstraping new connections")
config.addOption('BOOTSTRAP_CONTACT_REPEAT',10,"the number of times the agent will attempt to establish the first contact with the master")
config.addOption('PULL_REQUEST_DELAY',0.2,"delay in seconds between updating the result to the master and pulling new task")
config.addOption('APPLICATION_SHELL','','Shell for running the application. If left empty then the application package is imported directly into the WorkerAgent process. Otherwise, a separate process is started with the specified shell (e.g. "sh"). That process is called a servlet (and implemented by WorkerServlet module) and the application package is imported there.')


# constants
miliseconds = 1000

import omniORB
from omniORB import CORBA
import DIANE_CORBA 
import time
import os
import sys
import streamer

from diane.application import create_application_proxy

import diane.application

from diane.BaseThread import BaseThread
class HeartbeatThread(BaseThread):
    """ Periodically ping given peer (master or directory service).
    """
    def __init__(self,worker_agent,peer):
        BaseThread.__init__(self,name='HeartbeatThread')
        self.worker_agent = worker_agent
        self.peer = peer

    def run(self):
        try:
            last_ping_time = 0
            while not self.should_stop():
                if last_ping_time:
                    time_left_to_next_ping = min(delay,config.HEARTBEAT_DELAY) - (time.time()-last_ping_time)
                    if time_left_to_next_ping>0:
                        logger.debug('time left to next ping = %d',time_left_to_next_ping)
                        time.sleep(1)
                        continue
                delay = self.peer.ping(self.worker_agent.uuid,'worker')
                if not delay:delay = sys.maxint
                last_ping_time = time.time()
                    
        except CORBA.TRANSIENT:
            logger.warning('Unable to contact master after timeout=%d seconds'%config.HEARTBEAT_TIMEOUT)
        except DIANE_CORBA.XHangup,x:
            logger.info('XHangup: %s closed the session while ping() call (heartbeat)',self.peer.__class__.__name__)
            self.worker_agent.finalization_cookie = x.finalization_cookie
            #logger.warning('Master removed me from the list due to the heartbeat timeout (network delays? server busy? master process suspended?)')
        except Exception,x:
            logger.exception('heartbeat failure')

        self.stop() # not really needed but let's be formal
        self.worker_agent.stop()

def copy_ref_with_timeout(objref,timeout):
    """ Return a copy of the remote object reference with a client timeout set to it (in seconds)."""
    objref = objref._duplicate(objref) #make a copy of the obj's reference to assign a timeout to it
    omniORB.setClientCallTimeout(objref,timeout*miliseconds)
    return objref


from StandingCall import StandingCall, StandingCallStopped, StandingCallFailed

class WorkerAgent(BaseThread):
    """ Worker Agent.
    """
    def __init__(self,program,peer,ds_enabled,labels):
        BaseThread.__init__(self,name='WorkerAgent')
        from diane.util.compatibility import uuid
        self.uuid = str(uuid())
        self.program = program
        self.ds_enabled = ds_enabled
        if ds_enabled:
            self.ds = peer
            self.master = None
        else:
            self.ds = None
            self.master = peer
        self.labels = labels
        self.heartbeat_thread = None
        self.wid = None
        self.application = None
        self.finalization_cookie = None
        self.ftc = None #file transfer client connected to the default file transfer server
        
        # if this worker is run as a Ganga job (optional) then retrive the Ganga Job UUID to make it available
        # in monitoring/vcards etc...

        self.ganga_job_uuid =  os.environ.get('GANGA_JOB_UUID', '')

        if not self.ganga_job_uuid:
            try:
                self.ganga_job_uuid = open('ganga_job_uuid', 'r').read() 
            except IOError:
                pass 


    # we allow to pickle worker agent in order to give (restricted) access to attributes from the out-of-process Worker Servlet
    def __getstate__(self):
        state = self.__dict__.copy()
        state['program'] = None
        state['ds'] = None
        state['master'] = None
        state['application'] = None
        state['heartbeat_thread'] = None
        # remove all private data member (also the ones comming from the thread base classes)
        for s in state:
            if '_Thread__' in s: # FIXME: needs to be more generic
                state[s] = None
        return state
    
    def make_vcard(self):
        import vcard
        return vcard.make_vcard(extras = {'GANGA_JOB_UUID': self.ganga_job_uuid})


    def registerToMaster(self):
            try:
                bootmaster = StandingCall(copy_ref_with_timeout(self.master,config.BOOTSTRAP_CONTACT_TIMEOUT), 1, max_repeat=config.BOOTSTRAP_CONTACT_REPEAT)

                self.wid,file_server_ior = bootmaster.registerWorker(self.uuid,1) # ordinary workers have capacity 1

                file_server = self.program.orb.string_to_object(file_server_ior)
                
                from diane.FileTransfer import FileTransferClient
                self.ftc = FileTransferClient(file_server,self.uuid)
                import diane.workspace
                # collect and upload vcard
                vcard = self.make_vcard()
                file('vcard.txt','w').write(repr(vcard))
                self.ftc.upload('vcard.txt',os.path.join(diane.workspace.workerSubdir(self.wid),'vcard.txt'))

                #TEST - multiple registrations
                #for i in range(1000):
                #    self.wid = self.master.registerWorker(self._this())
                #logger.info('master ping %s',str(self.master.ping()))

                self.heartbeat_thread = HeartbeatThread(self,self.master)
                self.heartbeat_thread.start()
            except CORBA.TRANSIENT:
                logger.error('unable to establish connection to the master after timeout=%d seconds'%config.BOOTSTRAP_CONTACT_TIMEOUT)
                raise
            except DIANE_CORBA.XHangup:
                logger.warning('I was refused a registration (my peer_id=%s)',self.uuid)
                raise
                
    def run(self):

        import MSGWrap
        
        from diane.config import log_configuration
        log_configuration(title='initial configuration')        

        msg_data = { '_worker_uuid' : self.uuid }

        try:
            self.registerToMaster()

            master = StandingCall(self.master, config.HEARTBEAT_DELAY, should_stop = self.should_stop)
            
            
            app_boot,app_init = master.get_init_data(self.uuid) #(config.HEARTBEAT_DELAY,-1,self.should_stop,self.master,'get_init_data',self.uuid)
            _boot = streamer.loads(app_boot)
            msg_data['_master_uuid'] = _boot.master_uuid
            msg_data['_runid'] = _boot.runid
            import os
            msg_data['ganga_job_uuid'] = self.ganga_job_uuid

            # FIXME: if worker restart enabled, save diane.config.__all_configs and restore it after run has finished
            MSGWrap.sendStatus('_worker_create_application_proxy_start', msg_data)
            self.application = create_application_proxy(app_boot,app_init,agent=self)
            MSGWrap.sendStatus('_worker_create_application_proxy_finish', msg_data)
            
            self.program.registerAtExitHandler(self.finalize_application)
            
            MSGWrap.sendStatus('_worker_initialize_start', msg_data)
            app_init_output = self.application.initialize(app_init)
            MSGWrap.sendStatus('_worker_initialize_finish', msg_data)

            # config may have been updated and the value of config.HEARTBEAT_DELAY may have changed -> need to create the object again
            # FIXME: use a REFERENCE to config.HEARTBEAT_DELAY
            master = StandingCall(self.master, config.HEARTBEAT_DELAY, should_stop = self.should_stop)

            master.put_init_result(self.uuid,app_init_output,0) #(config.HEARTBEAT_DELAY,-1,self.should_stop,self.master,'put_init_result',self.uuid,app_init_output,0)

            while not self.should_stop():
                time.sleep(config.PULL_REQUEST_DELAY) # PENDING: this parameter should be dynamically controlled by the master
                tid,task_data = master.get_task_data(self.uuid) #(config.HEARTBEAT_DELAY,-1,self.should_stop,self.master,'get_task_data',self.uuid)
                try:
                    msg_data['tid'] = tid
                    MSGWrap.sendStatus('_worker_do_work_start', msg_data)
                    task_result = self.application.do_work(task_data)
                    MSGWrap.sendStatus('_worker_do_work_finish', msg_data)
                    error = 0
                except diane.application.ApplicationFailure,x: # recoverable problem
                    task_result = streamer.dumps(x)
                    error = 1
                    #FIXME: reporting failure is not yet well-defined
                
                master.put_task_result(self.uuid,tid,task_result,error) #(config.HEARTBEAT_DELAY,-1,self.should_stop,self.master,'put_task_result',self.uuid,tid,task_result,error)

        except diane.application.ApplicationFailure,x: # recoverable problem but raised by the application init
            pass
        except diane.application.ApplicationCritical,x: # unrecoverable problem
            pass
        except DIANE_CORBA.XHangup,x:
            self.finalization_cookie = x.finalization_cookie
        except StandingCallStopped,x:
            pass
        except StandingCallFailed,x:
            pass
        except Exception,x:
            logger.exception(x)
            
        # FIXME: call master.remove() for optimization
        # FIXME: restart worker for optimization, we may pass the restart flag in the program object and configure this behaviour in the config section
        # FIXME: 
        self.stop()

    def finalize_application(self):
        logger.debug('finalize_application() called')
        if self.application:
            if self.finalization_cookie is None:
                self.finalization_cookie = streamer.EMPTY
            try:
                self.application.finalize(self.finalization_cookie)
            except Exception,x:
                logger.warning('raised while application.finalize(): ',exc_info=True)

    def stop(self):
        BaseThread.stop(self)
        if self.heartbeat_thread:
            self.heartbeat_thread.stop()
        self.program.stop()
        
    def getStatusReport(self):
        return 'wid=%s'%(repr(self.wid),)
        
    def ping(self):
        return 0
    
    def kill(self):
        logger.info('killed by remote method invocation')
        self.stop()
        return True

def time_master_ping(master):
    t0 = time.time()
    for i in range(100000):
        master.pingMaster()
    t1 = time.time()
    logger.info('time delta: %f'%(t1-t0))


def main(master_oid,omniorb_config_filename,enable_GSI=False):
    import os, tempfile

    from CORBAProgram import CORBAProgram
    program = CORBAProgram(server=False,config_file=omniorb_config_filename,enable_GSI=enable_GSI)

    #install_exception_handlers()
    
    master = program.orb.string_to_object(master_oid)

    # set default timeout to master reference
    master = copy_ref_with_timeout(master,config.HEARTBEAT_TIMEOUT)

    # create the worker agent object
    # and start client loop in a separate thread
    worker_agent = WorkerAgent(program,master,False,[])
    worker_agent.start()

    program.start()

##     logger.debug('falling off the main thread...')
##     import threading
##     for t in threading.enumerate():
##         if t.isAlive():
##             logger.critical('thread %s is still alive',t)
    

#############################################################################################
### FIXME: SOMEHOW DISABLED BY TIMOUT MECHANISMS
def install_exception_handlers():

    config.REMOTE_CALL_RETRY = 0 # wait 2^N seconds (N=5 -> approx 1 minute)

    # default handlers for remote calls errors

    def handle_REMOTE_CALL_ERROR(program,retries, exc):
        if retries >= config.REMOTE_CALL_RETRY:
            return False
        logger.warning('REMOTE_CALL_ERROR handler: waiting %ds %s',1<<retries,repr(exc))
        time.sleep(1<<retries)
        return True

    omniORB.installTransientExceptionHandler(program,handle_REMOTE_CALL_ERROR)
    omniORB.installCommFailureExceptionHandler(program,handle_REMOTE_CALL_ERROR)    
###
#############################################################################################
    
   
    
