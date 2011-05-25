import time

class TaskStatus:
    """ Task status enumeration.
    """

    ## Not processing yet.
    NEW = 0
    
    ## Processing (assigned to a worker).
    PROCESSING = 1

    ## Sucessfully completed by a worker.
    COMPLETED = 2

    ## Error reported by a worker.
    FAILED = 3

    ## Unable to determine the completion status (e.g. network down)
    LOST = 4

    ## Explicitly ignored by TaskScheduler.
    IGNORED = 5

    ## Task cancelled before processing on the worker started.
    ## Tasks are buffered by RunMaster until picked up by a worker.
    UNSCHEDULED = 6

    def is_status(status_number):
        """Return True if status number corresponds to a status defined by this class."""
        return status_number in TaskStatus.all()

    is_status = staticmethod(is_status)

    def all():
        """Return a list of all task status numbers."""
        return [TaskStatus.NEW,TaskStatus.PROCESSING,TaskStatus.FAILED,TaskStatus.LOST,TaskStatus.COMPLETED,TaskStatus.IGNORED, TaskStatus.UNSCHEDULED]
    all = staticmethod(all)

class TaskInfo:
    """ TaskInfo keeps track of the task status, input/output data and the history of task
    execution. TaskInfo is used by RunMaster (updates the task status and task_output)
    and TaskScheduler (assigns tasks to worker, defines task_input and consumes task_output).

    TaskInfo objects should normally be created by the TaskScheduler
    or ApplicationManager using the IMasterController.create_task()
    method. This is a change from 2.0-beta21 onwards, therefore some
    older application code may still use this constructor directly.

    The 'history' attribute is a list of TaskDetails objects. The last
    item in the list corresponds to most recent task execution attempt and is
    accessible as the 'details' property.

    The task_input holds the task input data and there is one input for all execution attempts.
    The task_output is defined in TaskDetails object and may be different for different attempts.
    """

    def __init__(self,tid):
        """ Create a TaskInfo object with a given task id. Tasks id must be unique integers.
        """

        ## Task identifier (int).
        self.tid = tid

        ## Task execution history.
        self.history = [TaskDetails()]

        ## Task input data.
        self.task_input = None

        ### Application- or user-defined string label which is a short summary information about the task.
        self.application_label = ''

        ### Application- or user-defined properties which provide more in-depth information about the task. Each item of the details list is a tuple (key,value) where key is a string and value is a string, int or float.
        self.application_details = []
        
    def assign(self,wid):
        """ Mark task as being processed by a given worker (execution attempt)
        and create a new entry in the history list.  Task may only be assigned
        if in NEW, FAILED or LOST state.
        """
        if not self.status is TaskStatus.NEW:
            # cannot assign tasks twice unless previous attempt failed
            assert(self.status in [TaskStatus.FAILED, TaskStatus.LOST, TaskStatus.UNSCHEDULED])
            self.history.append(TaskDetails())
        self.details.do_assign(wid)

    def update(self, result, error=None):
        """ Update the task status (result may be COMPLETED,FAILED,LOST) and optional error information.
        """
        self.details.do_update(result,error)

    def ignore(self):
        """ Declare the task as ignored.
        """
        self.details.do_ignore()
        
    def details(self):
        """ Return the current TaskDetails.
        """
        return self.history[-1]

    def status(self):
        """ Return the current task status."""
        return self.details.get_status()

    def task_output(self):
        """ Return current task output. """
        return self.details.task_output
    
    ##@name Shortcuts (properties)
    ## 
    ##@{    
    status = property(status, None, None, "status of the last task assign() attempt")    
    details = property(details, None, None, "details of the last task assign() attempt")
    task_output = property(task_output,None, None, "output of the last task assign() attempt")
    ##@}
    
    ##@name Statuses.
    ## These enums are here for user convenience (see also TaskStatus class)
    ##@{
    NEW = TaskStatus.NEW
    PROCESSING = TaskStatus.PROCESSING
    COMPLETED = TaskStatus.COMPLETED
    FAILED = TaskStatus.FAILED
    LOST = TaskStatus.LOST
    IGNORED = TaskStatus.IGNORED
    UNSCHEDULED = TaskStatus.UNSCHEDULED
    ##@}
    
    
class TaskDetails:
    """ Detailed task status for a single execution attempt.
    """
    
    def __init__(self):

        ## worker id to which  the task was assigned
        self.assigned_wid = -1

        ## None, FAILED, LOST, COMPLETED
        self.result = None

        ## True/False.
        self.ignored = False

        ## error code (app error, worker crash,...) - to be specified
        self.error = None

        ## output of the task execution
        self.task_output = None
        
        ## time when the worker is assigned to the worker (enters a buffer in RunMaster)
        self.time_buf_scheduled = 0 #

        ## time when the task was fetched by the worker
        self.time_start = 0

        ## time elapsed on the worker node (application only)
        self.time_execute = 0

        ## time when the task was returned by the worker
        self.time_finish = 0

        ## time when the task was consumed by TaskScheduler
        self.time_buf_received = 0

        ## time difference to complete the start request (network roundtrip and input data transfer)
        self.time_start_roundtrip = 0

        ## time difference to complete the finish request (network roundtrip and output data transfer)
        self.time_finish_roundtrip = 0 

    def get_status(self):
        """ Get the task status (one of the values defined in TaskStatus).
        """
        if self.ignored:
            return TaskStatus.IGNORED
        if self.assigned_wid == -1:
            return TaskStatus.NEW
        if self.result is None:
            #if self.time_start != 0:
            #    return TaskStatus.BUFFERED
            return TaskStatus.PROCESSING
        return self.result

    def do_assign(self,wid):
        assert(wid > 0)
        assert(self.assigned_wid == -1)
        self.assigned_wid = wid
        self.time_buf_scheduled = time.time()

    def do_update(self,result,error):
        assert(self.assigned_wid != -1)
        assert(result not in [TaskStatus.NEW, TaskStatus.PROCESSING])
        assert(result in [TaskStatus.COMPLETED,TaskStatus.IGNORED,TaskStatus.UNSCHEDULED,TaskStatus.LOST] or error is not None)
        self.time_buf_received = time.time()
        self.result = result
        self.error = error

    def do_ignore(self):
        self.ignored = True
