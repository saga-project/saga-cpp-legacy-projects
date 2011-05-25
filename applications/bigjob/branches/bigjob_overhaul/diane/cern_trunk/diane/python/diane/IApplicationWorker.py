class IApplicationWorker:
    """ Application worker class.
    Application may raise ApplicationCritical exception to signal that there was a critical error which prevents application from
    continuing. 
    Unhandled exceptions will be transformed into ApplicationFailure by the proxy wrapper.
    FIXME: maybe a better strategy is to transform unhandled exception by default into ApplicationCritical exception?
    """
    def __init__(self):
        self._agent = None # worker agent instance (for example wid and other attributes may be retrieved from here)

    def initialize(self,app_init):
        pass

    def finalize(self, cookie):
        pass

    def do_work(self, task_data):
        pass

    #def getShellScriptor(self):
    #    pass
