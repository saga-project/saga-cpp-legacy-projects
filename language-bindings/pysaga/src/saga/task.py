# GFD-R-P.90
#package saga.task
from object import Object, ObjectType
from monitoring import Monitorable

class State(object):
    new      = 1
    running  = 2
    done     = 3
    canceled = 4
    failed   = 5
    
class WaitMode(object):
    all = 0
    any = 1


class Async(object):
    """
    This interface is empty on purpose, and is used only for tagging of SAGA 
    classes which implement the SAGA task model.
    """
    pass

class Task(Object, Monitorable):
    """ """
    def __init__(self):
    #no constructor
        super(Task,self).__init__()
        pass
    
    def run(self):
        raise NotImplemented, "run() is not implemented in this object"
    
    def cancel(self, timeout=0.0):
        #in float timeout = 0.0
        raise NotImplemented, "cancel() is not implemented in this object"
    
    def wait(self, timeout=-1.0):
        #in float timeout = -1.0, out boolean finished
        raise NotImplemented, "wait() is not implemented in this object"
      
    def get_state(self):
        #out state state
        raise NotImplemented, "get_state() is not implemented in this object"
      
    def get_result(self):
        """The first out parameter, which is the return value in the synchronous
        method version, is, in the task and asynchronous version, accessed by call-
        ing task.get result <return type> (void);, which is a templetized
        member method. That call implies a call to wait(). For language bind-
        ings where templetized member functions are not available, a language
        specific mechanism MUST be found, which MAY use type casting.
        
        B{TO ADD STUFF}
        """
        

    def get_result(self):
          """ <type> (out type             result);"""
          
    def get_object(self):
          """<type> (out type             object);"""
      
      # error handling
    def rethrow(self):
        """B{TO ADD STUFF}
          // Metric:
          //   name:  task.state
          //   desc:  fires on task state change, and
          //          has the literal value of the task
          //          state enum.
          //   mode:  ReadOnly
          //   unit:  1
          //   type:  Enum
          //   value: 0
        """
    
    
    
class TaskContainer(Object, Monitorable):
    """

     CONSCTRUCTOR       (out task_container    obj);
     DESCTRUCTOR        (in task_container     obj);
     // task management
     add                (in   task             task,
                         out  int              cookie);
     remove             (in   int              cookie,
                         out  task             task);
     // state management
     run                (void);
     cancel             (in float              timeout = 0.0);
     wait               (in wait_mode          mode     = All,
                         in float              timeout = -1.0,
                         out task              finished);
     // inspection
     size               (out  int              n);
     list_tasks         (out  array<int>       cookies);
     get_task           (in   int              cookie,
                         out  task             t);
     get_tasks          (out  array<task>      tasks);
     get_states         (out  array<state>     states);
     // Metric:
     //    name:  task_container.state
     //    desc:  fires on state changes of any task in
     //           container, and has the value of that
     //           task"s cookie.
     //    mode:  ReadOnly
     //    unit:  1
     //    type:  Int
     //    value: -
     """
