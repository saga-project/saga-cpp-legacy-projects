class IApplicationManager:
    """ The application manager defines the application-specific actions of
    the Run Master.

    Separation between application manager and task scheduler enables to
    separate scheduling aspects from application callbacks.

    However for some application models this may not be possible and the
    application actions must be embedded in the specific implementation of the
    scheduler.  In such cases the application manager does not perform any
    function.

    The specific implementations of application manager may (but does not have
    to) be derived from BaseThread. In this case the additional thread to
    control application actions is spawned.  Such a thread may be used, for
    example, to monitor the application progress or to add new tasks to the
    scheduler (refer to the specific scheduler implementation such as
    SimpleTaskScheduler).
    """
    def __init__(self):
        ## the scheduler assigned to this manager
        self.scheduler = None
        
    def __getstate__(self):
        state = self.__dict__.copy()
        # we do not pickle the scheduler
        state['scheduler']=None
        return state

