import threading
import traceback

import diane
logger = diane.getLogger('BaseThread')


class BaseThread(threading.Thread):
    """
    Application thread base class.
    """

    def __init__(self,name=None,auto_register=True):
        """ Create a DIANE service thread (and register it as such into the singleton program unless auto_register == False)."""
        if name is None:
            name = ''
        name = '.'.join(['diane.BaseThread',name])
        threading.Thread.__init__(self,name=name)
        self.setDaemon(1) # we can exit entire application at any time
        self.__should_stop_flag = False
        logger.debug("BaseThread object created:%s",self.__class__.__name__)
        if auto_register:
            import diane.CORBAProgram
            diane.CORBAProgram.theProgramInstance.addServiceThread(self)
    def should_stop(self):
        return self.__should_stop_flag
    
    def stop(self):
        if not self.__should_stop_flag:
            logger.debug("Stopping: %s",self.__class__.__name__)
            self.__should_stop_flag = True       
