import diane
logger = diane.getLogger('StandingCall')

import time

class StandingCallFailed(Exception):
    pass

class StandingCallStopped(Exception):
    pass

class StandingCall:
    """ Invoke methods of a remote object as standing calls.

    Standing call is a mechanism which provides automatic retry in case
    of (transient) network problems, so when the standing call ultimately fails
    the client may be pretty sure that the server cannot be reached.

    Additionally standing call implements the handling of the XHangup
    andXRepeatCall exceptions so that the server may explicitly request to
    have the call repeated or to terminate the converation with the client.

    The StandingCall class is a wrapper for a remote object reference. Each
    method is implicitly invoked as a standing call.  The configuration
    parameters of the standing call are defined in the constructor.
    
    """
    def __init__(self, obj, default_delay, max_repeat=-1, should_stop=None, success_callback=None):
        """Create a standing call wrapper for obj.

        Parameters:
        
        If remote server raises XRepeatCall then the call is repeated until
        the server returns a valid result.  The delay between the repetitions
        is defined by default_delay, the repetition number by max_repeat (-1
        == indefinite repetitions).
        
        The should_stop is a callable which returns True or False. If
        should_stop() evaluates to True, then the (repeated) call is not
        attempted and StandingCallStopped exception is raised.
        
        """
        self.__params = [default_delay,max_repeat,should_stop,obj]
        self.__success_callback = success_callback

    class MethodProxy:
        def __init__(self,sc,method):
            self.__sc = sc
            self.__method = method

        def __call__(self,*args,**kwds):
            params = self.__sc._StandingCall__params + [self.__method,args,kwds]
            val = _standing_call(*params)
            try:
                success_callback = self.__sc._StandingCall__success_callback
                if success_callback:
                    success_callback(val)
            except Exception,x:
                logger.exception('StandingCall.success_callback() raised an exception')
            return val

    def __getattr__(self,name):
        return StandingCall.MethodProxy(self,name)
    
import DIANE_CORBA
from omniORB import CORBA

def _standing_call(default_delay,max_repeat,should_stop,obj,method,args,kwds):
    repeat = 0
    while 1:
        delay = default_delay
        try:
            if should_stop and should_stop():
                raise StandingCallStopped()
            logger.debug('standing call invoking: %s.%s(args=%s,kwds=%s)',repr(obj),method,args,kwds)
            return getattr(obj,method)(*args,**kwds)
        except DIANE_CORBA.XHangup,x:
            logger.info('XHangup: %s closed the session while %s() call',obj.__class__.__name__,method)
            raise
        except DIANE_CORBA.XRepeatCall,x:
            if x.delay:
                delay = x.delay
            logger.info('XRepeatCall: %s asks to repeat the %s() call, repeated in %d seconds'%(obj.__class__,method,delay))
        except CORBA.TRANSIENT:
            logger.warning('Unable to contact the %s while trying %s() remote call (CORBA.TRANSIENT)'%(obj.__class__.__name__,method))
        except CORBA.Exception:
            logger.exception('Unable to contact the %s while trying %s() remote call'%(obj.__class__.__name__,method))
            raise StandingCallFailed()
        if max_repeat>=0:
            if repeat>=max_repeat: raise StandingCallFailed()
            repeat += 1
        time.sleep(delay)
