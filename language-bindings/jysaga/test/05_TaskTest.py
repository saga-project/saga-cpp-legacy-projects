from saga.session import Session
from saga.context import Context
from saga.error import *
from saga.object import ObjectType, Object
from saga.buffer import Buffer
import array.array
from saga.file import File
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
#     add_callback, get_metric, list_metrics, remove_callback 
    pass

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
print "=== create a Task by an ASYNC call to File.get_size"     
url = URL(temp_filename)
file = File(url)
task = file.get_size(TaskType.ASYNC)

print"=== State: NEW:",State.NEW,
print "RUNNING:", State.RUNNING,
print "DONE:",State.DONE,
print "CANCELED:", State.CANCELED,
print "FAILED:", State.FAILED

outcome = file.get_size(TaskType.NORMAL)
print "file.get_size(TaskType.NORMAL)    Outcome was:", outcome
print "=== Create SYNC task"
taskSYNC = file.get_size(TaskType.SYNC)
#print "Create ASYNC task,",
#taskASYNC = file.get_size(TaskType.ASYNC)
#print "Create TASK task. ",
#taskTASK = file.get_size(TaskType.TASK)
#print "DONE"
#
print "SYNC Task.get_state():", taskSYNC.get_state(), 
print "  Outcome was:", taskSYNC.get_result()
#print "ASYNC Task.get_state():", taskASYNC.get_state(), "  Outcome was:", taskASYNC.get_result()
#print "TASK Task.get_state():", taskTASK.get_state(), " Start running the TASK"
#if taskTASK.get_state() is State.NEW:
#    taskTASK.run()
#print "TASK Task.get_state():", taskTASK.get_state(), "  Outcome was:", taskTASK.get_result()
  
  
#__del__(self)
#Destroy the object.     
print "=== Cancel the already done SYNC task"
taskSYNC.cancel(10)

      
#cancel(self, timeout=0.0)
#Cancel the asynchronous operation.     
#<object>     
#get_object(self)
#Get the object from which this Task was created.     
#<return value>     
#get_result(self)
#Get the result of the async operation.     
#int     
#get_state(self)
#Get the state of the Task.     
#      
#rethrow(self)
#Re-raise any exception a failed Task caught.     
#      
#run(self)
#Start the asynchronous operation.     
#bool     
#wait(self, timeout=-1.0)
#Wait for the Task to finish.

checkObjectMethods(task)
checkMonitorableMethods(task)


print "=== Doing nothing yet"
 


print "==================================================="
print "== End Test of Task                              =="
print "==================================================="