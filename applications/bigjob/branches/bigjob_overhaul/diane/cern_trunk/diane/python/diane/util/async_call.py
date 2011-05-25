from BaseThread import BaseThread
import time

class AsyncCallThread(BaseThread):
    """Asynchronous function call thread.
    The result of call func(*args,**kwds) is stored in the result attribute.
    If an exception is raised the exception object is stored in the exception attribute. Otherwise the exception attribute is None.
    The caller is notified with the callback(t) where t is this thread object.
    """
    def __init__(self, callback, func, *args, **kwds):
        BaseThread.__init__(self,'async_call_%s'%func.__name__)
        self.callback = callback
        self.func = func
        self.args = args
        self.kwds = kwds
        self.result = None
        self.exception = None
        self.start_time = 0
        self.stop_time = 0
        
    def run(self):
        self.start_time = time.time()

        try:
            self.result = self.func(*self.args,**self.kwds)
        except Exception,x:
            self.exception = x

        self.stop_time = time.time()
        self.callback(self)

        
def async_call(callback,func, *args, **kwds):
    """Call func asynchronously in a separate thread. See AsyncCallThread class for details."""
    t = AsyncCallThread(callback,func,*args,**kwds)
    t.start()
    return t

#################
## TO BE REVIEWED 

## def timeout_call(func, timeout, *args, **kwds):
##     def callback(t):
##         print 'completed'
##     t = AsyncCallThread(callback,func,*args,**kwds)
##     t.start()
##     time.sleep(1)
##     while time.time()-t.start_time < timeout:
##         if t.stop_time:
##             if t.exception:
##                 raise t.exception
##             else:
##                 return t.result
##         time.sleep(1)
##     raise Exception('timeout!')

## timeout_call(time.sleep,3,1)
## timeout_call(time.sleep,3,5) # raises timeout exception
#################

if __name__ == '__main__':

    cnt = 2
    def wait(n,fail=False):
        import time
        print 'wait:',n,'raise:',fail
        time.sleep(n)
        if fail:
            raise Exception('failed on purpose')
        return n

    def callback(t):
        global cnt
        cnt -= 1
        print 'callback: result %s, raised %s, duration %fs'%(repr(t.result), repr(t.exception), t.stop_time-t.start_time)

    async_call(callback, wait, 5)
    async_call(callback, wait, 3, True)

    print 'main thread sleeping'
    while cnt > 0:
        time.sleep(0.1)
    print 'main thread finishing'

        

