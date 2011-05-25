# journal analysis tools

class TimeSlicer:
    def __init__(self):
        self.window_size = 3600
        self.window_left = None
        self.window_first = None # the first event being recorded
        self.points=[]
    def __call__(self):
        return self.window_left
    def next(self):
        self.window_left+=self.window_size
        self.points.append(self.window_left)
    def reset(self,t):
        self.window_left = t
        self.window_first = t
        self.points.append(t)

class UnhandledEventError(Exception):
    pass

class JournalAnalyzer:
    def new_window(self):
        pass

    def event(self,name,t,p):
        try:
            event_method = getattr(self,'event_'+name)
        except AttributeError:
            raise UnhandledEventError()
        return event_method(t,p)

    def postprocess(self):
        pass

    def scan(self,journal):
        unhandled_events = {}

        self.last_t = None #for ordering check

        cnt = 0
        for e in journal: #read_journal(fn):
            time,event,params = e
            params = Params(params)
            #if MAXEVENT:
            #    if cnt > MAXEVENT:
            #        break

            if cnt == 0:
                self.last_t = time
            else:
                if time < self.last_t:
                    raise Exception('journal entries are NOT in order: entry_no=%d time=%s'%(cnt+1,time))
            
            if cnt % 10000 == 0:
                print 'processed %d entries'%cnt
            try:
                if cnt == 0:
                    self.time.reset(time)
                else:
                    next_n_windows= int((time - self.time.window_left)/self.time.window_size)
                    for i in range(next_n_windows):
                        self.time.next()
                        self.new_window()
                try:
                    self.event(event,time,params)
                except UnhandledEventError:
                    unhandled_events.setdefault(event,0)
                    unhandled_events[event] += 1

            except Exception,x:
                print e
                raise
            cnt += 1
        print 'processed %d entries'%cnt
        for event in unhandled_events:
            print 'unhandled event "%s" (%d)'%(event,unhandled_events[event])

        self.postprocess()


from diane.analysis_tools import Params

class MultipleAnalyzer(JournalAnalyzer):
    def __init__(self,time): #FIXME: adding analyzers=[] and updating the corresponding line below causes a strange object-sharing effect (double counted events if mutliple times the main() is called in LatticeQCD2 package)
        self.time = time
        self.analyzers = []

    def add(self,name,analyzer):
        setattr(self,name,analyzer)
        self.analyzers.append(analyzer)

    def new_window(self):
        for a in self.analyzers:
            a.new_window()

    def postprocess(self):
        for a in self.analyzers:
            a.postprocess()

    def event(self,name,t,p):
        handled = False
        for a in self.analyzers:
            try:
                event_method = getattr(a,"event_"+name)
                handled = True
            except AttributeError:
                continue
            else:
                event_method(t,p)
        if not handled:
            raise UnhandledEventError()




def main(analyzer,argv=None,scope={}):
    import sys,os
    import cPickle as pickle
    from diane.analysis_tools import read_journal, Params

    if argv is None:
        argv = sys.argv
    try:
        input_fn = argv[1]
    except IndexError:
        input_fn = 'run.j'

    # MAXEVENT = -1 will force the recreation of cache    
    try:
        MAXEVENT = int(argv[2])
    except:
        MAXEVENT = 0

    cache_fn = "tmp."+input_fn+".history.cache"
    cache_fn = cache_fn.replace(os.sep,'.') # remove slashes

    try:
        if MAXEVENT<0:
            if MAXEVENT == -1:
                MAXEVENT = 0
            else:
                MAXEVENT=-MAXEVENT
            raise IOError('recreating cache file') #force recreation of cache
        print "reading data from cache file",cache_fn
        analyzer = pickle.load(file(cache_fn))
    except IOError,x:
        try:
            print x
            print "scanning journal file",input_fn
            analyzer.scan(read_journal(input_fn, MAXEVENT=MAXEVENT,scope=scope)) #scan_journal(input_fn)
            print "creating cache file",cache_fn
            pickle.dump(analyzer,file(cache_fn,'w'))
        except IOError,x:
            print
            print 'ERROR:',x
            print
            print 'arguments: file.j MAXEVENT'
            print 'MAXEVENT == 0 or -1: scan all events in the journal file'
            print 'MAXEVENT < 0: force recreation of cache'
            sys.exit(-1)

    analyzer.filename = input_fn

    return analyzer

