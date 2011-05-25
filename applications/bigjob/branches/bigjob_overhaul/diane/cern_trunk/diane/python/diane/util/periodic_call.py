import time

class PeriodicCall:
    def __init__(self, period, callable):
        self.callable = callable
        self.period = period
        self.last_time = -1

    def __call__(self, *args, **kwds):
        time_now = time.time()
        if self.last_time == -1 or time.time()-self.last_time > self.period:
            self.last_time = time_now
            return self.callable(*args,**kwds)

if __name__ == "__main__":
    import logging
    N = 10
    logging.basicConfig(level=logging.INFO)
    logging.info('entering loop for %d seconds',N)
    t0 = time.time()

    msg = PeriodicCall(2,logging.info)

    while time.time() - t0 < N:
        msg('This periodic message is called every %d seconds',msg.period)
        time.sleep(0.1)
        
    logging.info('leaving loop after %d seconds',time.time()-t0)
