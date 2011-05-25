from diane.IApplicationManager import IApplicationManager

class SimpleApplicationManager(IApplicationManager):
    """ Application manager for simple task farming applications (bag of
    independent tasks).

    The simple application manager creates tasks which are then handled by the
    simple scheduler. In DIANE 1 this was called a 'Planner'.

    When the tasks are fisnihed the simple application manager is notified and may perform
    application-specific actions such as merging of the results. In DIANE 1 this was called an 'Integrator'.

    The tasks are represented by TaskInfo objects and may be created with _task() helper method:

     t = self._task()
     t.task_input = user_defined_parameters_here

    The task status and output data may be accessed like this:

     if t.status == t.COMPLETED:
       use t.task_output

    Additionally few advanced callbacks are provided for data and task allocation optimization.
    The workers are represented by WorkerEntry objects.
    """
    def __init__(self):

        ## The default worker initialization value (valid unless worker_init() callback is overriden in a special way).
        self.worker_init = None
        
    def initialize(self,run_input_data):
        """Initialize with the run input data provided by the user and return an
        initial list of tasks. Tasks are represented by TaskInfo objects and may be created using
        _task() method."""
        return []

    def tasks_done(self,tasks):
        """Task result integration method.  This method is called when the
        scheduler finds out that some tasks reached their final state.  In the
        case of SimpleTaskScheduler this means completed or ignored tasks.
        Assertion: task.status in [task.COMPLETED, task.IGNORED]"""
        pass

    def has_more_work(self):
        """ Return True if there is still more work to do. When False is
        returned the TaskScheduler terminates the run."""
        return False

    def finalize(self):
        """ Finalize and cleanup the application manager. Called at the exit point
        from the task scheduler. Should not block for a very long time.
        """
        pass

    ##@name Advanced.
    ## Optional methods for advanced task allocation and data optimization
    ##@{
    def make_input(self,tasks,worker):
        """Called by the scheduler just before the tasks are scheduled to the
        worker.  Allows to create task.input_data on-demand and depending on
        additional the worker information.  This method is optional as the
        task.input_data may be assigned by initialize() and add_tasks() in
        less aggressive way."""
        pass

    def make_init(self,worker):
        """ Return the initialization data for the worker (worker argument is a WorkerEntry object).
        Called by the scheduler before the worker agent is initialized. The worker_entry
        may be used to get additional worker attributes (such as wid).
        Default implementation returns self.worker_init
        """
        return self.worker_init
    ##@}    

    def _task(self, application_label=None, application_details=None):
        """ This is a helper function which may be used by the derived classes to create new tasks.
        This function is not thread-safe. If you want to use it from multiple threads (unlikely) then you should protect it with a mutex.
        """
        return self.scheduler.job_master.create_task(application_label=application_label, application_details=application_details)

        
    
