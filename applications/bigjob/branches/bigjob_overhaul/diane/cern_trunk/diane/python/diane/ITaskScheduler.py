import diane
logger = diane.getLogger('TaskScheduler')
        
class Policy:
    """ Collection of policy values. Empty by default."""
    pass

#PENDING: TODO: instrument on the fly methods of TaskScheduler with
#log and journal statements. This will guarantee that all user-defined
#schedulers will be logged and journalled correctly.

from diane.BaseThread import BaseThread
class ITaskScheduler(BaseThread):
    """ TaskScheduler keeps track of the tasks, controls how tasks are
    assigned to workers and how task output is processed (integrated).
    TaskInfo objects are used to represent tasks.  WorkerEntry objects are
    used to represent the worker agents.  Task manager runs in a separate
    thread and is notified by RunMaster by callback methods (defined below).

    An application manager (if defined) may be used to separate the scheduling functionality from
    the application-specific actions. See IApplicationManager.
    """

    ## Policy parameters defining the behaviour of the TaskScheduler.
    ## The parameters are specific to the implementation and may be set by the user at runtime.
    policy = Policy()
    def __init__(self,job_master,appmgr):
        BaseThread.__init__(self,name=self.__class__.__name__)
        ## RunMaster instance.
        self.job_master = job_master
        ## Application manager.
        self.appmgr = appmgr
        if self.appmgr:
            self.appmgr.scheduler = self

    def run(self):
        """ If application manager is defined and is a BaseThread object then spawn a new thread controlling it.
        If overriden, the run() method must call this base class implementation of run() before doing anything else.
        """
        if self.appmgr and isinstance(self.appmgr,BaseThread):
            self.appmgr.start()
            
    def log_configuration(self):
        for p in dir(self.policy):
            if p[:2] != '__' and p[-2:] != '__':
                logger.info('%s.policy.%s = %s',self.__class__.__name__,p, repr(getattr(self.policy,p)))
            
    def initialize(self, job_input):
        """ Initialize the TaskScheduler passing the job input parameters as specified by the user
        when starting the run.
        """
        pass

    def has_more_work(self):
        """Return True if task manager has more work to do, False otherwise.
        RunMaster assumes that resources are needed as long as this method
        returns True."""
        raise NotImplementedError

    def worker_added(self,w_entry):
        """This method is called by RunMaster when the new worker agent is
        added.  The w_entry parameter is an instance of WorkerEntry class.
        Application specific initialization data may be assigned to
        w_entry.init_input at this point.
        """
        pass

    def worker_initialized(self,w_entry):
        """ This method is called by RunMaster when the worker agent
        sucessfully initialized and optionally returned initialization output
        (w_entry.init_output).  Until this method returns the worker will not
        be fully initialized (so the framework will not mark it as a ready
        worker).
        """
        pass
    
    def worker_removed(self,w_entry):
        """ This method is called when the worker has been removed (either
        lost or terminated due to some reason).
        """
        pass

    def tasks_completed(self,tasks):
        """This method is called when tasks completed ok. The tasks argument is a list of TaskInfo objects.
        """
        raise NotImplementedError

    def tasks_lost(self,tasks):
        """This method is called when the system could not determine if the
        tasks were completed or not. The contact to the worker agent which
        processed the tasks was lost and the task status will not be
        available."""
        raise NotImplementedError

    def tasks_failed(self,tasks):
        """This method is called when tasks completed with failure."""
        raise NotImplementedError

    def tasks_unscheduled(self,tasks):
        """This method is called when tasks have been unscheduled. The
        unscheduled tasks are those which have not been yet started. The
        unscheduling may happen when the worker is removed or on explicit
        IMasterController.unschedule() or if IMasterController.schedule()
        fails for some reason.
        """
        raise NotImplementedError


