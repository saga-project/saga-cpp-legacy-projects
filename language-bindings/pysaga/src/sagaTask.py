  package saga.task
  {
    enum state
    {
      New       = 1,
      Running   = 2,
      Done      = 3,
      Canceled = 4,
      Failed    = 5
    }
    enum wait_mode
    {
      All       = 0,
                     145
saga-core-wg@ogf.org
GFD-R-P.90               SAGA Task Model             January 15, 2008
      Any       =  1
    }
    interface async
    {
      // this interface is empty on purpose, and is used only
      // for tagging of SAGA classes which implement the SAGA
      // task model.
    }
    class task : implements    saga::object
                 implements    saga::monitorable
              // from object saga::error_handler
    {
      // no constructor
      DESCTRUCTOR       (in task              obj);
      // state management
      run               (void);
      cancel            (in float             timeout = 0.0);
      wait              (in float             timeout = -1.0,
                          out boolean         finished);
      // inspection
      get_state         (out state            state);
      get_result <type> (out type             result);
      get_object <type> (out type             object);
      // error handling
      rethrow           (void);
      // Metric:
      //   name:  task.state
      //   desc:  fires on task state change, and
      //          has the literal value of the task
      //          state enum.
      //   mode:  ReadOnly
      //   unit:  1
      //   type:  Enum
      //   value: 0
    }
    class task_container : implements    saga::object
                                                                  146
saga-core-wg@ogf.org
GFD-R-P.90               SAGA Task Model              January 15, 2008
                            implements    saga::monitorable
                        // from object    saga::error_handler
   {
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
     //           task’s cookie.
     //    mode:  ReadOnly
     //    unit:  1
     //    type:  Int
     //    value: -
   }
 }
