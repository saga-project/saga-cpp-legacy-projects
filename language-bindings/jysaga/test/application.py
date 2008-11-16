import java.util.List;

import org.ogf.saga.url.URLFactory;
import org.ogf.saga.url.URL;
import org.ogf.saga.error.AlreadyExistsException;
import org.ogf.saga.error.AuthenticationFailedException;
import org.ogf.saga.error.AuthorizationFailedException;
import org.ogf.saga.error.BadParameterException;
import org.ogf.saga.error.DoesNotExistException;
import org.ogf.saga.error.IncorrectStateException;
import org.ogf.saga.error.IncorrectURLException;
import org.ogf.saga.error.NoSuccessException;
import org.ogf.saga.error.NotImplementedException;
import org.ogf.saga.error.PermissionDeniedException;
import org.ogf.saga.error.TimeoutException;
import org.ogf.saga.buffer.Buffer;
import org.ogf.saga.buffer.BufferFactory;
import org.ogf.saga.file.Directory;
import org.ogf.saga.file.File;
import org.ogf.saga.file.FileFactory;
import org.ogf.saga.namespace.Flags;
import org.ogf.saga.session.Session;
import org.ogf.saga.session.SessionFactory;

from org.ogf.saga.url import URLFactory
from org.ogf.saga.file import FileFactory
from org.ogf.saga.task import TaskFactory , TaskMode, WaitMode
import org.ogf.saga.monitoring.Callback
from time import sleep
from org.ogf.saga.namespace import NSFactory
from org.ogf.saga.task import TaskMode

from org.ogf.saga.monitoring import  MonitoringFactory

from org.ogf.saga.namespace import Flags

class CallbackProxy(org.ogf.saga.monitoring.Callback):
    pythonCallbackObject = None

    def cb(self, monitorable, metric, context):
        from saga.task import Task, TaskContainer

        print "CallbackProxy.cb called:"

        if metric is not None:
            print str(metric)
        else:
            print "metric is None"

        if context is not None:
            print str(context)
        else:
            print "context is None"

        if monitorable is not None:
            print str(monitorable)
        else:
            print "monitorable is None"

        print "End of CallbackProxy.cb call"
        return

url_source = URLFactory.createURL("file:///tmp/09_NSDirectoryDIRECTORY")
url_target = URLFactory.createURL("file:///tmp/09_NSDirectoryDIRECTORYcopy")
nsd = NSFactory.createNSDirectory(url_source)
try:
    print "NSDirectory.copy(src,target):"
    nsd.copy( url_source, url_target, Flags.RECURSIVE.getValue())
except org.ogf.saga.error.SagaException, e:
    print "NSDirectory.copy(src,target):" , str(e.__class__), str(e) 
#
#try:
#    print "NSDirectory.copy(target):"
#    nsd.copy(url_target,2)
#except org.ogf.saga.error.SagaException, e:
#    print "NSDirectory.copy(target):" , str(e.__class__), str(e)
    
#try:
#    nsd.copy( url_source, url_target, Flags.RECURSIVE)
#except org.ogf.saga.error.SagaException, e:
#    print "2.NSDirectory.copy(RECURSIVE):" , str(e.__class__), str(e)
    
#    
#nse = None
#try:
#    nse = NSFactory.createNSEntry(url_source)
#except org.ogf.saga.error.SagaException, e:
#    print "NSEntry():" , str(e.__class__), str(e)
#
#if nse != None:
#    nse.copy(url_target)
#    NONE            =  0
#    OVERWRITE       =  1
#    RECURSIVE       =  2
#    DEREFERENCE     =  4
#    CREATE          =  8
#    EXCLUSIVE       = 16
#    LOCK            = 32
#    CREATEPARENTS   = 64

#temp_filename = "/tmp/08_NSEntryTest.py.temp"
#url =  URLFactory.createURL(temp_filename)
#nse = NSFactory.createNSEntry(url)
#print "NSEntry.isEntry():",nse.isEntry()
#clone = nse.clone()
#print "Clone.isclone():",clone.isEntry()


#m = MonitoringFactory.createMetric("TotalCPUCount","total number of cpus requested for this job",\
#           "ReadWrite","1","Int","1")      
#     
#print "=== add_callback(c)",
#c = CallbackProxy()
#cookie = m.addCallback(c)
#print "Cookie is",cookie
#
#      
##fire(self)
##Push a new metric value to the backend.
#
#print "=== fire()"
#m.fire()
#
##remove_callback(self, cookie)
##Remove a callback from a metric.     
#
#print "=== remove_callback "
#m.removeCallback(cookie) 

##application.py
#import sagaContext
#import sagaSession
#     
#def main():
#   c = sagaContext.Context("ftp")
#   s = sagaSession.Session()
#   s.add_context(c)
#   
#   cont = s.list_contexts()
#   print cont
#
#if __name__ == "__main__":
#    main()

#################################################################
   #for x in cont:
   #     print x
   
 
   #s = SessionFactory.createSession()
   # print "session:", s
   #sp = s.clone()
   # print "sessionClone:", sp
   #rij = s.listContexts()
   # print "rij:", rij, " len:", len(rij)
   #s.addContext(ContextFactory.createContext("ftp"))
    
   #rij = s.listContexts()
   # print "rij:", rij, " len:", len(rij)
   # print rij[0]
   #s.__class__
   #b = Blar()
   #b.session = s
   #bp = b.cloner()
   #print b.session, b
   #print bp.session, bp
   #f= FileFactory.createFile(session, url, flags)

#def cloner(obj):
#    return obj.clone() 
#
#class Blar:
#    session = None
#    
#    def cloner(self):
#        if not self.session==None:
#            b = Blar()
#            b.session = self.session.clone()
#            return b    

