
def ctime(t=None):
    """Convert time from Epoch seconds to a standard string."""
    import time
    t = time.localtime(t)
    return time.strftime("%Y-%m-%d-%H-%M:%S",t)

if __name__=="__main__":
    import time
    print ctime(),ctime(time.time())
