from saga.session import Session
from saga.context import Context
from saga.error import *
from saga.object import ObjectType, Object
from saga.buffer import Buffer
from saga.monitoring import Callback
import array.array
from saga.file import File, SeekMode, Directory, Flags
from saga.url import URL
from saga.task import Task, TaskType, State


import org.ogf.saga.error.AlreadyExistsException
import org.ogf.saga.error.AuthenticationFailedException 
import org.ogf.saga.error.AuthorizationFailedException
import org.ogf.saga.error.BadParameterException 
import org.ogf.saga.error.DoesNotExistException
import org.ogf.saga.error.IncorrectStateException
import org.ogf.saga.error.IncorrectURLException 
import org.ogf.saga.error.NoSuccessException 
import org.ogf.saga.error.NotImplementedException
import org.ogf.saga.error.PermissionDeniedException
import org.ogf.saga.error.SagaException 
import org.ogf.saga.error.SagaIOException 
import org.ogf.saga.error.TimeoutException 

class Callb(Callback):
    returnValue = False
    def __init__(self, keep):
        if keep == True:
            self.returnValue = True
        else:
            self.returnValue = False
    
    def cb(self, monitorable, metric, context):
     import sys
     sys.stdout.write("================ callb.cb() called ===================\n")
     sys.stdout.write("================ State of Task Changed ===============\n")
     sys.stdout.flush()
     return self.returnValue

def checkObjectMethods(o):
    try:
        print "get_id:      "+ str(o.get_id()   )
    except Exception, e:
        print "!!! WARNING !!! get_id(): ", str(e)
    try:
        print "get_type:    "+ str(o.get_type() ) + ", ObjectType.TASK is " +str(ObjectType.TASK)
    except Exception, e:
        print "!!! WARNING !!!", "get_type:", str(e) 
    try:   
        session = o.get_session()
        print "get_session: "+ str(session.__class__),"get_id:" + str(session.get_id()),"type: "+str(session.get_type()), "ObjectType.SESSION "+str(ObjectType.SESSION)                 
    except Exception, e:
        print "!!! WARNING !!!", "get_session(): " + str(e) 
    try:    
        clone = o.clone()
        print "clone:       "+ str(clone.__class__) + " type: "+str(clone.get_type()) + " get_id:" + str(clone.get_id())

    except Exception, e:
        print "!!! WARNING !!!", "clone(): " + str(e) 

def checkMonitorableMethods(o):
    metric_list = o.list_metrics()
    print "=== List metrics", str(metric_list)
    metr = o.get_metric('Task.state')
 
    from time import sleep 

    c = Callb(True)
    cookie =  o.add_callback('Task.state', c)
    print "=== Callback added to taskTask. Cookie was", cookie, "state:",o.get_state()
    print "=== taskTask.run()"
    o.run()                                
    while o.get_state() == 2:              
        for i in range(5):
            sleep(1)
            print "+",    
    print "=== taskTask.get_state():", o.get_state()
    print "=== remove callback",
    o.remove_callback('Task.state', cookie)
    print o.get_result()
    print " done"
    

def printAttributes(o):
    print "Name           Ex\tRO\tREM\tVec\tWri\tValue"
    list =  o.list_attributes()
    for i in list:
        length =  14 - len(str(i)) 
        space = length * '.'
        string = str(i) + space
        print string, o.attribute_exists(i),"\t" , 
        if (o.attribute_is_readonly(i)) is True: print 1,
        else: print 0,
        print "\t" ,str(o.attribute_is_removable(i)), "\t" ,str(o.attribute_is_vector(i)), \
          "\t" ,str(o.attribute_is_writable(i)),"\t", str(o.get_attribute(i))

print "==================================================="
print "== Test of Task                                  =="
print "==================================================="

task = None
temp_filename = "/tmp/05_TaskTest.py.temp"
print "=== create test file(s)", temp_filename
file = open( temp_filename, "w")
file.write("abcdefghijklmnopqrstuvwxyz")
file.flush()
file.close()
print "=== create a Task by an SYNC/ASYNC/TASK call to File.get_size"     
url = URL(temp_filename)
file = File(url)

print"=== State: NEW:"+ str(State.NEW),
print "RUNNING:"+ str(State.RUNNING),
print "DONE:"+ str(State.DONE),
print "CANCELED:"+ str( State.CANCELED),
print "FAILED:"+ str(State.FAILED)

outcome = file.get_size(TaskType.NORMAL)
print "    file.get_size(TaskType.NORMAL)    Outcome was:", outcome
print "=== Create SYNC task",
taskSYNC = file.get_size(TaskType.SYNC)
print "Create ASYNC task,",
taskASYNC = file.get_size(TaskType.ASYNC)
print "Create TASK task. ",
taskTASK = file.get_size(TaskType.TASK)
print "DONE"

print "   SYNC Task.get_state():", taskSYNC.get_state(), "Outcome was:", taskSYNC.get_result()
print "   ASYNC Task.get_state():", taskASYNC.get_state()
from time import sleep
counter = 0
while (taskASYNC.get_state() == 2 and counter < 10):
     sleep (5)
     print "        taskASYNC is still running. Sleeping for 5"
     counter = counter + 1
if counter != 10:
    print "    Outcome was:", taskASYNC.get_result()
else:
    print "    taskASYNC has not finished after 50 seconds"
    
print "    TASK Task.get_state():", taskTASK.get_state(), " Start running the TASK"
if taskTASK.get_state() == State.NEW:
    try:
        print "    Doing get_result on the new TASK:",
        taskTASK.get_result()
    except Exception, e:
        print str(e)
    taskTASK.run()
print "    TASK Task.get_state():", taskTASK.get_state()
counter = 0
while (taskTASK.get_state() == 2 and counter < 10):
     sleep (5)
     print "        taskTASK is still running. Sleeping for 5"
     counter = counter+1
     
if counter != 10:
    print "Outcome was:",
    print str(taskTASK.get_result())
else:
    print "taskTASK has not finished after 50 seconds"
  
#__del__(self)
#Destroy the object.     
print "=== Cancel the already done SYNC task"
taskSYNC.cancel(10)
taskTASK = file.get_size(TaskType.TASK)

print "=== Cancel a new job"
try:
    taskTASK.cancel(0)
except Exception, e:
    print "Exception caught.", e.__class__, str(e)
      
try:
    object = taskTASK.get_object()
except Exception,e:
    print str(e.__class__), str(e)

#Get the object from which this Task was created.     

dir = Directory( URL("file:///tmp/") )
taskSYNC = dir.get_size(name=URL(temp_filename), flags = Flags.RECURSIVE, tasktype=TaskType.SYNC)

print "=== dir.get_size(name=URL(temp_filename), flags = Flags.RECURSIVE, tasktype=TaskType.SYNC)"
print "=== creates an exception. Retrow() "  
try:
    taskSYNC.rethrow()
except Exception, e:
    print "Exception caught.", e.__class__, str(e)

print "=== Test wait()"
taskTASK = file.get_size(TaskType.TASK)
try:
    taskTASK.wait(timeout=0)
except Exception, e:
    print "Exception caught.", e.__class__, str(e)
try:
    taskTASK.run()
    taskTASK.wait(timeout=0)
except Exception, e:
    print "Exception caught.", e.__class__, str(e)

checkObjectMethods(taskTASK)

taskTASK = file.get_size(TaskType.TASK)
checkMonitorableMethods(taskTASK)

print "==================================================="
print "== End Test of Task                              =="
print "==================================================="