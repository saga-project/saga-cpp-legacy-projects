# Package: saga
# Module: task 
# Description: The module which specifies the classes concerning the Task model in saga
# Specification and documentation can be found in section 3.10, page 140-162 of the GFD-R-P.90 document
# Author: P.F.A. van Zoolingen, Computer Systems Section, Faculty of Exact Science (FEW), Vrije Universiteit, Amsterdam, The Netherlands.

# GFD-R-P.90
#package saga.Task
from object import Object, ObjectType
from monitoring import Monitorable
from error import NotImplemented
from org.ogf.saga.task import TaskMode
from buffer import Buffer
from file import Iovec

class State(object):
    """ 
    State specifies the possible states a Task can be in
    """
    
    NEW      = 1
    """
    @summary: this state identifies a newly constructed Task instance which has not yet
        run. This state corresponds to the BES state 'Pending'. This state is
        initial.
    """
    
    RUNNING  = 2
    """
    @summary: the run() method has been invoked on the Task, either explicitly or
        implicitly. This state corresponds to the BES state 'Running'. This
        state is initial
    """
    
    DONE     = 3
    """
    @summary: the synchronous or asynchronous operation has finished successfully. It
        corresponds to the BES state 'Finished'. This state is final.
    """
    
    CANCELED = 4
    """
    @summary: the asynchronous operation has been canceled, i.e. cancel() has been
        called on the Task instance. It corresponds to the BES state 'Canceled'.
        This state is final.
    """
    
    FAILED   = 5
    """
    @summary: the synchronous or asynchronous operation has finished unsuccessfully.
        It corresponds to the BES state 'Failed'. This state is final.
    """

class WaitMode(object):
    """
    The wait mode enum specifies the condition on which a wait() operation on a
    saga.Task.TaskContainer returns.
    """
    ALL = 0  
    """@summary: wait() returns if all Tasks in the container reached a final state"""
    ANY = 1
    """@summary: wait() returns if one or more Tasks in the container reached a final state."""

class Async(object):
    """
    This interface is empty on purpose, and is used only for tagging of SAGA 
    classes which implement the SAGA Task model. All methods of subclasses of Async
    will be able to return Task objects.
    """

class TaskType(object):
    """
    The variables in this class are used in classes which support the Task model
    to specify which version of the method they want to use. Classes supporting the
    Task model subclass L{Async}.
    """
    # Java way is
    # ASYNC(1),
    # TASK(2),
    # SYNC(3);
    
    NORMAL = 0
    """@summary: let the method return its specified return values """
    SYNC   = 1
    """@summary: let the method return a Task object in a final state. """
    ASYNC  = 2
    """@summary: let the method return a Task object in the RUNNING state """
    TASK   = 3
    """@summary: let the method return a Task object in the NEW state """

class Task(Object, Monitorable):
    """
    Tasks represent asynchronous API calls. They are only created
    by invoking an asynchronous method (Tasks should not be created directly) on a SAGA object
    which returns a Task object (with saga.Task.Async or saga.Task.Task).  
    But as saga.job.Job instances inherit from the Task class, Jobs are also effectively
    created as Tasks. If a Task gets created, it will share the state of the object it was created from.

        - B{Metric:}
            - name:  Task.state
            - desc:  fires on Task state change, and has the literal value of a the TaskState class variable.
            - mode:  ReadOnly
            - unit:  1
            - type: int
            - value: 0

    """
    delegateObject = None
    fileReadBuffer = None
    
    def __init__(self, **impl):
        #no constructor
        super(Task,self).__init__()
        if delegateObject in impl:
            if type(impl["delegateObject"]) is not org.ogf.saga.task.Task:
                raise BadParameter, "Parameter impl[\"delegateObject\"] is not a org.ogf.saga.task.Task. Type: " + str(type(impl["delegateObject"]))
            self.delegateObject = impl["delegateObject"]
        if fileReadBuffer in impl:
            if type(impl["fileReadBuffer"]) is not org.ogf.saga.buffer.Buffer:
                raise BadParameter, "Parameter impl[\"fileReadBuffer\"] is not a org.ogf.saga.buffer.Buffer Type: " + str(type(impl["fileReadBuffer"]))
            self.fileReadBuffer = impl["fileReadBuffer"]
 
#DOCUMENT: fileReadBuffer 
    
    def __del__(self):
        """
        Destroy the object.
        @PostCondition: state is no longer shared with the object the Task was created from.
        @PostCondition: the Task instance is 'CANCELED' prior to resource deallocation.
        @Note: if the instance was not in a final state before, the destructor performs a cancel()
            on the instance, and all notes to cancel() apply.
        """
        Object.__del__(self)
        Monitorable.__del__(self)
        pass
    
    def run(self):
        """
        Start the asynchronous operation.
        @summary:  Start the asynchronous operation.
        @precondition: Task is in 'NEW' state
        @postcondition: Task is in 'RUNNING' state
        @permission: appropriate permissions for the method represented by the Task
        @raise  NotImplemented:
        @raise IncorrectState:
        @raise Timeout:
        @raise NoSuccess:
        @note: run can only be called on a Task in 'NEW' state. All other states will cause the
               'IncorrectState' exception to be raised.
        @note: a 'Timeout' or 'NoSuccess' exception indicates that the backend was not able to start the Task.

        """
        try:
            self.delegateObject.run()
        except java.lang.Exception, e:
            raise convertException(e)
    
    def cancel(self, timeout=0.0):
        #in float timeout = 0.0
        """
        Cancel the asynchronous operation.
        @summary:  Cancel the asynchronous operation.
        @param   timeout:              time for freeing resources
        @type timeout: float
        @precondition: Task is in 'RUNNING' state.
        @postcondition: Task is in 'CANCELED' state.
        @raise NotImplemented:
        @raise IncorrectState:
        @raise Timeout:
        @raise NoSuccess:
        @note: for resource deallocation semantics and timeout semantics, see Section 2 of the GFD-R-P.90 document.
        @note: if cancel() fails to cancel the Task immediately, and tries to continue to cancel
            the Task in the background, the Task state remains 'RUNNING' until the cancel operation
            succeeded. The state then changes to 'CANCELED'.
        @note: if the Task is in a final state, the call has no effect, and, in particular, does NOT change
            the state from 'DONE' to 'CANCELED', or from 'FAILED' to 'CANCLED'. This is to avoid race conditions.
        @note: if the Task is in 'NEW' state, an 'IncorrectState' exception is raised.
        @note: a 'NoSuccess' exception indicates that the backend was not able to initiate the cancelation for the Task.
          

        """
        if type(timeout) is not float or type(timeout) is not int:
            raise BadParameter, "Parameter timeout is not a number. Type: " + str(type(timeout))
        try:
            if timeout is 0:
                self.delegateObject.cancel()
            else:
                self.delegateObject.cancel(timeout)
        except java.lang.Exception, e:
            raise convertException(e)
    
#TODO: check type checking default parameters for methods in for all!! classes.     

    def wait(self, timeout=-1.0):
        #in float timeout = -1.0, out boolean finished
        """
        Wait for the Task to finish.
        @summary:  Wait for the Task to finish.
        @param timeout: seconds to wait
        @type timeout: float
        @return: indicator if the Task is done running
        @rtype: bool
        @precondition: Task is not in 'NEW' state.
        @postcondition: if no timeout occurs, Task is in a final state.
        @raise NotImplemented:
        @raise IncorrectState:
        @raise Timeout:
        @raise NoSuccess:
        @note: wait returns success (True) as soon as the Task enters a final state
        @note: if the Task is already in a final state, the call returns success (True) immediately.
        @note: if the Task is in 'NEW' state, an 'IncorrectState' exception is raised.
        @note: wait returns no success (False) if the Task is, even after timeout, not in a final state.
        @note: a 'Timeout' or 'NoSuccess' exception indicates that the backend was not able to wait for the
            Task. Note that a 'Timeout' exception does not indicate that the Task is not in a final
            state after the given wait period - that causes an unsuccessfull (False) return value.
        @note: for timeout semantics, see Section 2 of the GFD-R-P.90 document

        """        
        if type(timeout) is not float or type(timeout) is not int:
            raise BadParameter, "Parameter timeout is not a number. Type: " + str(type(timeout))
        if timeout < 0 and timeout is not -1:
            raise BadParameter,"Parameter timeout is a negative number. timeout: " + str(timeout)
        try:
            retval = 0
            if timeout is -1:
                self.delegateObject.waitfor()
                return True
            else:
                retval = self.delegateObject.waitfor(timeout)
                if retval is 1:
                    return True
                else:
                    return False
        except java.lang.Exception, e:
            raise convertException(e)
      
    def get_state(self):
        #out state state
        """
        Get the state of the Task.
        @summary:  Get the state of the Task.
        @return: state of the Task
        @rtype: int
        @raise NotImplemented:
        @raise Timeout:
        @raise NoSuccess:
        @note: a 'Timeout' or 'NoSuccess' exception indicates that the backend was not able to retrieve the Task state.
        """
        try:
            retval = self.delegateObject.run()
            return retval.getValue()
        except java.lang.Exception, e:
            raise convertException(e)
      
    def get_result(self):
        """
        Get the result of the async operation.
        @summary: Get the result of the async operation.
        @return: return value of the async method
        @rtype: I{<return value>}
        @precondition: Task is not in NEW, FAILED or CANCELED state.
        @postcondition: Task is in a final state.
        @raise NotImplemented:
        @raise IncorrectState:
        @raise Timeout:
        @raise NoSuccess:
        @note: get_result implies a wait() - all notes to wait apply.
        @note: the method returns the type and value which would be returned by the synchronous version of
            the respective function call.
        """
        try:
            retval = self.delegateObject.getResult()
        except java.lang.Exception, e:
            raise convertException(e)
        
        if self.fileReadBuffer is not None:
            if self.fileReadBuffer.managedByImp is False:    #Buffer or Iovec
                self.fileReadBuffer.update_data()
            if type(self.fileReadBuffer) is list:            # list of Iovecs
                for iovec in list:
                    if iovec.managedByImp is False:
                        iovec.update_data()
        
        if type(retval) is java.lang.Boolean:
            if retval.booleanValue() is 1: return True
            else: return False
        elif type(retval) is org.ogf.saga.context.Context:
            return Context(delegateObject = retval)
        elif type(retval) is org.ogf.saga.file.Directory:  pass
        elif type(retval) is org.ogf.saga.file.File: pass
        elif type(retval) is org.ogf.saga.file.FileInputStream: pass
        elif type(retval) is org.ogf.saga.file.FileOutputStream: pass
        elif type(retval) is java.io.InputStream: pass     
        elif type(retval) is java.lang.Integer:
            if self.fileReadBuffer is not None:
               if self.fileReadBuffer.managedByImp is False: 
                    return self.fileReadBuffer.getData().toString()
            if type(retval) is java.lang.Integer: return retval.intValue()
            else:                                 return retval
        elif type(retval) is org.ogf.saga.job.Job: pass
        elif type(retval) is org.ogf.saga.job.JobDescription: pass
        elif type(retval) is org.ogf.saga.job.JobSelf: pass  
        elif type(retval) is java.util.List: 
            if retval.isEmpty() is False:
                if type(reval.get(0)) is java.lang.String:    # List<String>
                    list = []
                    for i in range(retval.size()):
                        list.append( retval.get(i).toString() )
                        return tuple(list) 
                if type(reval.get(0)) is org.ogf.saga.url.URL: # List<URL>
                    list = []
                    for i in range(retval.size()):
                        temp = URL (delegateObject = retval.get(i))
                        list.append(temp)
                        return tuple(list)                     
        elif type(retval) is org.ogf.saga.logicalfile.LogicalDirectory: pass
        elif type(retval) is org.ogf.saga.logicalfile.LogicalFile: pass
        elif type(retval) is java.lang.Long:
            return retval.longValue()
        elif type(retval) is org.ogf.saga.namespace.NSDirectory: pass
        elif type(retval) is org.ogf.saga.namespace.NSEntry: pass
        elif type(retval) is java.io.OutputStream: pass
        elif type(retval) is org.ogf.saga.stream.Stream: pass
        elif type(retval) is org.ogf.saga.stream.StreamInputStream: pass
        elif type(retval) is org.ogf.saga.stream.StreamOutputStream: pass
        elif type(retval) is java.lang.String: 
            return retval.toString()
        elif type(retval) is org.ogf.saga.url.URL:
            return URL(delgateObject=retval)
        elif type(retval) is java.lang.Void:
            return None
        else: return retval
        
#TODO: implement Task.get_value        

    def get_object(self):
        """
        Get the object from which this Task was created.
        @summary: Get the object from which this Task was created.
        @return: object this Task was created from
        @rtype I{<object>}
        @raise NotImplemented:
        @raise Timeout:
        @raise NoSuccess:
        @note: the method returns a shallow copy of the object this Task was created from.
        """
 
#TODO: add object reference to Task. Add in each Method giving a task
      
      # error handling
    def rethrow(self):
        """
        Re-raise any exception a failed Task caught.
        @summary:  Re-raise any exception a failed Task caught.
        @raise NotImplemented:
        @raise IncorrectURL:
        @raise BadParameter:
        @raise AlreadyExists:
        @raise DoesNotExist:
        @raise IncorrectState:
        @raise PermissionDenied:
        @raise AuthorizationFailed:
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @note: this method does nothing unless the Task is in 'FAILED' state, and also raises
                 'IncorrectState' if the Task is in any other state.
        @note: if in 'FAILED' state, the method raises an exception which indicates the reason why that
                 Task entered the 'FAILED' state (i.e. it raises the exception which caused it to enter the
                 'FAILED' state.
        @note: rethrow() can be called multiple times, always raising the same exception.
        """
        try:
            self.delegateObject.rethrow()
        except java.lang.Exception, e:
            raise convertException(e)
    
    
class TaskContainer(Object, Monitorable):
    """
    The TaskContainer class is designed to effectively handle a large number
    of asynchronous operations, since managing a large number of Tasks can be tedious.
    For example, when an application uses many Tasks, it would be inefficient to
    invoke the wait() method on each of them individually. The TaskContainer
    class provides (amongst other operations) a mechanism to wait for a set of Tasks.
 
        - B{Metric}
            - name:  TaskContainer.state
            - desc:  fires on state changes of any Task in container, and has the value of that Task's cookie.
            - mode:  ReadOnly
            - unit:  1
            - type:  Int
            - value: -

    """
    delegateObject = None
    
    def __init__(self, **impl ):
        """
        Initialize the TaskContainer.
        @summary: Initialize the TaskContainer.
        @raise NotImplemented:
        @raise Timeout:
        @raise NoSuccess:
        @note: a 'Timeout' or 'NoSuccess' exception indicates that the backend was not able to create a TaskContainer.
        """
        if delegateObject in impl:
            if type(impl["delegateObject"]) is not org.ogf.saga.task.TaskContainer:
                raise BadParameter, "Parameter impl[\"delegateObject\"] is not a org.ogf.saga.task.TaskContainer. Type: " + str(type(impl["delegateObject"]))
            self.delegateObject = impl["delegateObject"]
            return
        try:
            self.delegateObject = org.ogf.saga.task.TaskFactory.createTaskContainer()
        except java.lang.Exception, e:
            raise convertException(e)
       
    def add(self, task):
        """
        Add a Task to a TaskContainer.
        @summary: Add a Task to a TaskContainer.
        @param task: Task to add to the TaskContainer
        @type task: L{Task}
        @return: cookie identifying the added Task
        @rtype int
        @postcondition: the Task is managed by the Task container.
        @raise NotImplemented:
        @raise Timeout:
        @raise NoSuccess:
        @note: a Task can be added only once. Any attempt to add a Task to the container which already
                  is in the container is silently ignored, and the same cookie as for the original Task is
                  returned again.
        @note: a 'Timeout' or 'NoSuccess' exception indicates that the backend was not able to add the Task
                  to the container.
                  
        """
        if type(task) is not Task:
            raise BadParameter, "Parameter task is not a Task object. Type: " + str(type(task))
        try:
            self.delegateObject.add(task.delegateObject)
        except java.lang.Exception, e:
            raise convertException(e)
        
    def remove(self, cookie):
        """
        Remove a Task from a TaskContainer.
        @summary:  Remove a Task from a TaskContainer.
        @param cookie: cookie identifying the Task to be removed
        @type cookie: int
        @return: the removed Task
        @rtype: L{Task}
        @precondition: the Task is managed by the TaskContainer
        @postcondition: the Task is not managed by the TaskContainer 
        @raise NotImplemented:
        @raise DoesNotExist:
        @raise Timeout:
        @raise NoSuccess:
        @note: if a Task was added more than once, it can be removed only once - see notes to add().
        @note: if the Task identified by the cookie is not in the TaskContainer, a 'DoesNotExist' exception is raised.
        @note: a 'Timeout' or 'NoSuccess' exception indicates that the backend was not able to remove the
                  Task from the TaskContainer.
                  
        """
        if type(cookie) is not int:
            raise BadParameter, "Parameter cookie is not an int. Type: " + str(type(cookie))
        try:
            retval = self.delegateObject.remove(task.delegateObject)
            return Task(delegateObject = retval)
        except java.lang.Exception, e:
            raise convertException(e)

    def run(self):
        """
        Start all asynchronous operations in the TaskContainer.
        @summary:  Start all asynchronous operations in the TaskContainer.
        @precondition: all Tasks in the container are in 'NEW' state.
        @postcondition: all Tasks in the comtainer are in 'RUNNING' state.
        @permission: see permissions on Task.run()
        @raise NotImplemented:
        @raise IncorrectState:
        @raise DoesNotExist:
        @raise Timeout:
        @raise NoSuccess:
        @note: run() causes an 'IncorrectState' exception if any of the Tasks in the container causes
                  that exception on run().
        @note: a 'Timeout' or 'NoSuccess' exception indicates that the backend was not able to run one or
                  more Tasks in the container.
        @note: if the TaskContainer is empty, an 'DoesNotExist' exception is raised.
        @note: As the order of execution of the Tasks is undefined, no assumption on the individual
                  Task states can be made after any exception gets raised.
        """
        try:
            self.delegateObject.run()
        except java.lang.Exception, e:
            raise convertException(e)       
        
    def wait(self, mode = WaitMode.ALL, timeout = -1.0):
        """
        Wait for one or more of the Tasks to finish.
        @summary:  Wait for one or more of the Tasks to finish.
        @param mode: wait for All or Any Task
        @param timeout: seconds to wait
        @type mode: int
        @type timeout: float
        @return: finished Task or one of all the finished Tasks
        @rtype: L{Task}
        @postcondition: if no timeout occurs, All/Any Tasks in the container are in a final state.
        @raise NotImplemented:
        @raise IncorrectState:
        @raise DoesNotExist:
        @raise Timeout:
        @raise NoSuccess:
        @note: if mode is 'ALL", the wait call returns only if all Tasks in the container are finished,
                  or on timeout, whichever occurs first. The output Task is then any of the finished Tasks.
        @note: if mode is 'Any', the wait call returns on the first Task which would return on Task.wait() in
                  that timeout period, and returns that Task.
        @note: the default wait mode is 'ALL" (0).
        @note: the returned Task is removed from the container, which allows constructs like::
                    while tc.size() > 0:
                         task = tc.wait (WaitMode.ANY) 
                         ...
                    
        @note: wait() MAY cause an 'IncorrectState' exception if any of the Tasks in the container causes
                  that exception on wait().
        @note: if the TaskContainer is empty, an 'DoesNotExist' exception is raised.
        @note: a 'Timeout' or 'NoSuccess' exception indicates that the backend was not able to wait for one
                  or more Tasks in the container.
        @note: As the order of execution of the Tasks is undefined, no assumption on the individual
                  Task states can be made after any exception gets raised.
        @note: for timeout semantics, see Section 2 of the GFD-R-P.90 document
        """
        if type(timeout) is not float or type(timeout) is not int:
            raise BadParameter, "Parameter timeout is not a number. Type: " + str(type(timeout))
        if mode is not WaitMode.ALL or mode is not WaitMode.ANY:
            raise BadParameter, "Parameter mode is not WaitMode.ALL or WaitMode.ANY. mode: "+str(mode)
        if timeout < 0 and timeout is not -1.0:
            raise BadParameter,"Parameter timeout is a negative number. timeout: " + str(timeout)
        if mode is WaitMode.ANY:
            waitmode = org.ogf.saga.task.WaitMode.ANY
        else: 
            waitmode = org.ogf.saga.task.WaitMode.ALL
        try:
            retval = None
            if timeout is -1.0:
                retval = self.delegateObject.waitFor(waitmode)
            else:
                retval = self.delegateObject.waitFor(timeout, waitmode)
            return Task(delegateObject = retval)
        except java.lang.Exception, e:
            raise convertException(e)  
#TODO: add object reference to Task. Add in each Method giving a task        
        
    def cancel(self, timeout):
        """
        Cancel all the asynchronous operations in the container.
        @summary:  Cancel all the asynchronous operations in the container.
        @param timeout:              time for freeing resources
        @type timeout: float
        @postcondition: if no timeout occurs, all Tasks in the container are in 'CANCELED' state.
        @raise NotImplemented:
        @raise IncorrectState:
        @raise DoesNotExist:
        @raise Timeout:
        @raise NoSuccess:
        @note: see semantics of Task cancel.
        @note: cancel() MUST cause an 'IncorrectState' exception if any of the Tasks in the container
                  causes that exception on cancel().
        @note: a 'Timeout' or 'NoSuccess' exception indicates that the backend was not able to run one or
                  more Tasks in the container.
        @note: if the TaskContainer is empty, a 'DoesNotExist' exception is raised.
        @note: As the order of execution of the Tasks is undefined, no assumption on the individual
                  Task states can be made after any exception gets raised.
        """
        if type(timeout) is not float or type(timeout) is not int:
            raise BadParameter, "Parameter timeout is not a number. Type: " + str(type(timeout))
        if timeout < 0 and timeout is not -1.0:
            raise BadParameter,"Parameter timeout is a negative number. timeout: " + str(timeout)
        try:
            if timeout is -1.0:
                self.delegateObject.cancel() 
            else:
                self.delegateObject.cancel(timeout)
        except java.lang.Exception, e:
             raise convertException(e)

    def size(self):
        """
        Return the number of Tasks in the TaskContainer.
        @summary:  Return the number of Tasks in the TaskContainer.
        @return: number of Tasks in TaskContainer
        @rtype: int
        @raise NotImplemented:
        @raise Timeout:
        @raise NoSuccess:
        @note: a 'Timeout' or 'NoSuccess' exception indicates that the backend was not able to 
            list the Tasks in the container.
        """
        try:
            return self.delegateObject.size() 
        except java.lang.Exception, e:
             raise convertException(e)       
        
        
    def list_tasks(self):
        """
        List the Tasks in the TaskContainer.
        @summary: List the Tasks in the TaskContainer.
        @return: tuple of cookies for all Tasks in TaskContainer
        @rtype: tuple
        @raise   NotImplemented:
        @raise Timeout:
        @raise NoSuccess:
        @note: a 'Timeout' or 'NoSuccess' exception indicates that the backend was not able to list the
                  Tasks in the container.
        """
        try:
            retval = self.delegateObject.listTasks()
            return tuple(retval) 
        except java.lang.Exception, e:
             raise convertException(e)            
        
    def get_task(self, cookie):
        """
        Get a single Task from the TaskContainer.
        @summary:  Get a single Task from the TaskContainer.
        @param cookie: the cookie identifying the Task to return
        @type cookie: int
        @return: the Task identified by cookie
        @rtype: L{Task}
        @raise NotImplemented:
        @raise DoesNotExist:
        @raise Timeout:
        @raise NoSuccess:
        @note: the returned Task is NOT removed from the TaskContainer.
        @note: if cookie specifies a Task which is not in the  container, a 'DoesNotExist' exception is raised.
        @note: a 'Timeout' or 'NoSuccess' exception indicates that the backend was not able to list the
                  Tasks in the container.
        """
        if type(cookie) is not int:
            raise BadParameter, "Parameter timeout is not an int. Type: " + str(type(timeout))
        try:
            retval = self.delegateObject.getTask()
            return Task(delegateObject = retval)
        except java.lang.Exception, e:
             raise convertException(e)       
    
    def get_tasks(self):
        """
        Get the Tasks in the TaskContainer.
        @summary: Get the Tasks in the TaskContainer.
        @return: list of Tasks in TaskContainer
        @rtype: list
        @raise NotImplemented:
        @raise Timeout:
        @raise NoSuccess:
        @note: the returned Tasks are NOT removed from the TaskContainer.
        @note: if the TaskContainer is empty, an empty list is returned.
        @note: a 'Timeout' or 'NoSuccess' exception indicates that the backend was not able to list the
                 Tasks in the container.
        """
        list = []
        try:
            javaArray = self.delegateObject.getTasks()
        except java.lang.Exception, e:
             raise convertException(e) 
        for i in range (len(javaArray)):
            temp = Task(delegateObject = javaArray[i])
            list.append(temp)
        return list
       
    def get_states(self):
        """
        Get the states of all Tasks in the TaskContainer.
        @summary: Get the states of all Tasks in the TaskContainer.
        @return: list of states for Tasks in TaskContainer
        @rtype: list
        @raise NotImplemented:
        @raise Timeout:
        @raise NoSuccess:
        @note: the returned list is not ordered
        @note: if the TaskContainer is empty, an empty list is returned.
        @note: a 'Timeout' or 'NoSuccess' exception indicates that the backend was not able to obtain the
                 states of the Tasks in the container.
        """
        try:
            javaArray = self.delegateObject.getStates()
        except java.lang.Exception, e:
            raise convertException(e)
        list = []
        for i in range(len(javaArray)):
            list.append(javaArray[i].getValue())
        return list
