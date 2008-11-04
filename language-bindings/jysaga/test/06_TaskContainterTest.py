from saga.session import Session
from saga.context import Context
from saga.error import *
from saga.object import ObjectType, Object
from saga.buffer import Buffer
import array.array
from saga.file import File, SeekMode, Directory, Flags
from saga.url import URL
from saga.task import Task, TaskType, State, TaskContainer, WaitMode

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
        print "get_type:    "+ str(o.get_type() ) + ", ObjectType.TASKCONTAINTER is " +str(ObjectType.TASKCONTAINER)
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


def setAttributes(o):
    try:
        o.set_attribute("CertRepository", "/etc/grid-security/certificates/")
    except Exception, e:
        print "!!! WARNING !!!", str(e)
    try:
        o.set_attribute("UserProxy","/tmp/x509up_u01234")
    except Exception, e:
        print "!!! WARNING !!!", str(e)    
    try:
        o.set_attribute("Type","http")
    except Exception, e:
        print "!!! WARNING !!!", str(e)
    try:
        o.set_attribute("UserID","testname")
    except Exception, e:
        print "!!! WARNING !!!", str(e)
    try:    
        o.set_attribute("Server","www.few.vu.nl")
    except Exception, e:
        print "!!! WARNING !!!", str(e)
    try:
        o.set_attribute("UserVO","O=dutchgrid")
    except Exception, e:
        print "!!! WARNING !!!", str(e)    
    try:
        o.set_attribute("RemoteID","/O=dutchgrid/O=users/O=vu/OU=cs/CN=Joe Doe")
    except Exception, e:
        print "!!! WARNING !!!", str(e)
    try:
        o.set_attribute("LifeTime","1247781600")
    except Exception, e:
        print "!!! WARNING !!!", str(e)   
    try:    
        o.set_attribute("RemoteHost","galjas.few.vu.nl")
    except Exception, e:
        print "!!! WARNING !!!", str(e)
    try:    
        o.set_attribute("UserPass","super_secret_password")
    except Exception, e:
        print "!!! WARNING !!!", str(e)
    try:    
        o.set_attribute("RemotePort","80")
    except Exception, e:
        print "!!! WARNING !!!", str(e)
    try:    
        o.set_attribute("UserCert","$HOME/.globus/usercert.pem")
    except Exception, e:
        print "!!! WARNING !!!", str(e)
    try:    
        o.set_attribute("UserKey","$HOME/.globus/userkey.pem")
    except Exception, e:
        print "!!! WARNING !!!", str(e)
#    try:    
#        o.set_attribute("<KEY>","<VALUE>")
#    except Exception, e:
#        print "!!! WARNING !!!", str(e)
#    try:    
#        o.set_vector_attribute("<KEY>",<LIST_OF_VALUES>)
#    except Exception, e:
#        print "!!! WARNING !!!", str(e)        
                        
def checkMethods(o, name):
    print "=== Check all o methods, name and return values"
    print "set_defaults     " + str(o.set_defaults())   
    print "list_attributes  " + str(o.list_attributes())
    print "attribute_exists (" +name+ ")      " + str(o.attribute_exists(name))
    print "attribute_is_readonly (" +name+ ") " + str(o.attribute_is_readonly(name))
    print "attribute_is_removable(" +name+ ") " + str(o.attribute_is_removable(name))
    print "attribute_is_vector (" +name+ ")   " + str(o.attribute_is_vector(name))
    print "attribute_is_writable (" +name+ ") " + str(o.attribute_is_writable(name))
    print "find_attributes ([U*])             " + str(o.find_attributes(["U*"]))
    tup = o.find_attributes(["U*", "*ote*", "*er*"])
    temp = list(tup)
    temp.sort()                               
    print "find_attributes ([U*,*ote*,*erv*]) " + str(temp)
    print "get_attribute(" +name+ ")          " + str(o.get_attribute(name))

def checkMonitorableMethods(o):
#     add_callback, get_metric, list_metrics, remove_callback 
    pass

print "==================================================="
print "== Test of TaskContainer                         =="
print "==================================================="

temp_filename = "/tmp/06_TaskContainer.py.temp"
print "=== Create test file(s)", temp_filename
file = open( temp_filename, "w")
file.write("abcdefghijklmnopqrstuvwxyz")
file.flush()
file.close()
 
url = URL(temp_filename)
file = File(url)

#__init__(self)
#Initialize the TaskContainer.     
print "=== Create TaskContainer"
tc = TaskContainer()
 
print "=== Create a SYNC task, an ASYNC task and 2 TASK tasks"
taskSYNC = file.get_size(TaskType.SYNC)
taskASYNC = file.get_size(TaskType.ASYNC)
taskTASK1 = file.get_size(TaskType.TASK)
taskTASK2 = file.get_size(TaskType.TASK)

print "=== Add them to the TaskContainer."
cookies = []
#int     
#add(self, task)
#Add a Task to a TaskContainer.     
cookies.append(tc.add(taskSYNC))
cookies.append(tc.add(taskASYNC))
cookies.append(tc.add(taskTASK1))
cookies.append(tc.add(taskTASK2))
print "=== Cookies are:",str(cookies)

#Task     
#remove(self, cookie)
#Remove a Task from a TaskContainer.     
#
print "=== Remove 3rd Task"
tr1 = tc.remove( cookies[2] ) 
if tr1.get_type() != ObjectType.TASK:
    print "=== Removed task was no Task!"

#tuple     
#list_tasks(self)
#List the Tasks in the TaskContainer. 
print "=== Listing Tasks:", str(tc.list_tasks())

#size(self)
#Return the number of Tasks in the TaskContainer.  
print "=== Taskcontainer size is:", tc.size()

tr2 = tc.remove( cookies[3] ) 
if tr2.get_type() != ObjectType.TASK:
    print "=== Removed task was no Task!"


#get_states(self)
#Get the states of all Tasks in the TaskContainer.
print "=== States of Tasks:", str(tc.get_states())
print "=== State: NEW:"+ str(State.NEW),
print "RUNNING:"+ str(State.RUNNING),
print "DONE:"+ str(State.DONE),
print "CANCELED:"+ str( State.CANCELED),
print "FAILED:"+ str(State.FAILED)

#Task     
#wait(self, mode=0, timeout=-1.0)
#Wait for one or more of the Tasks to finish.     
print "=== Wait for all the Tasks to be FINAL"
print "=== States of Tasks:", str(tc.get_states())
t1 = tc.wait(WaitMode.ANY, 5)
print "-result for done task:", t1.get_result()
print "=== Listing Tasks:", str(tc.list_tasks())

print "=== States of Tasks:", str(tc.get_states())
t2 = tc.wait(WaitMode.ANY)
print "-result for done task:", t2.get_result()
print "=== Listing Tasks:", str(tc.list_tasks())

print "=== Add the removed tasks"
tc.add(tr1)
tc.add(tr2)
#run(self)
#Start all asynchronous operations in the TaskContainer. 
print "=== Run all tasks."
tc.run()

      
#cancel(self, timeout)
#Cancel all the asynchronous operations in the container.     
#int     
print "=== Quickly cancel everything"
tc.cancel()
 
print "=== States of Tasks:", str(tc.get_states())
   
#tuple     
#list_tasks(self)
#List the Tasks in the TaskContainer.     


#list     
#get_tasks(self)
#Get the Tasks in the TaskContainer. 
tup = tc.get_tasks()
print "=== Get tasks:",tup

#Task     
#get_task(self, cookie)
#Get a single Task from the TaskContainer.     
gt = tc.get_task( tc.list_tasks()[0] )

print "=== States of Tasks:", str(tc.get_states())
print "-result:", tc.wait(WaitMode.ANY).get_result()
print "=== States of Tasks:", str(tc.get_states())
print "-result:", 

while tc.get_states()[0] == 2:
    from time import sleep
    sleep(1)
    print tc.get_states()[0],
     
x = tc.wait(WaitMode.ALL)
print "...",
print x.get_result()

checkObjectMethods(tc)


#checkMonitorableMethods(taskTASK)   

       




print "==================================================="
print "== End Test of TaskContainer                     =="
print "==================================================="