from org.ogf.saga.url import URLFactory
from org.ogf.saga.file import FileFactory
from org.ogf.saga.task import TaskFactory , TaskMode, WaitMode
import org.ogf.saga.monitoring.Callback
from time import sleep 

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


print "==================================================="
print "== Start Test                                    =="
print "==================================================="

temp_filename = "/tmp/06_TaskContainer.py.temp"
url = URLFactory.createURL(temp_filename)
file = FileFactory.createFile(url)

taskTASK = file.getSize(TaskMode.TASK)

metric_list = taskTASK.listMetrics()
print "=== List metrics", str(metric_list)
metr = taskTASK.getMetric('Task.state')
 


c = CallbackProxy()
cookie =  taskTASK.addCallback('Task.state', c)
print "=== Callback added to taskTask. Cookie was", cookie, "state:",taskTASK.getState()
print "=== taskTask.run()"
taskTASK.run()                                
while taskTASK.getState() == 2:               
    for i in range(5):
        sleep(1)
        print "+",    
       
print "=== taskTask.get_state():", taskTASK.getState()
print "=== remove callback",
taskTASK.removeCallback('Task.state', cookie)
print taskTASK.getResult()

print "==================================================="
print "== End Test                                      =="
print "==================================================="
  
#print "=== Create TaskContainer"
#tc = TaskFactory.createTaskContainer()
# 
#print "=== Create a SYNC task, an ASYNC task and 2 TASK tasks"
#taskTASK1 = file.getSize(TaskMode.TASK)
#taskTASK2 = file.getSize(TaskMode.TASK)
#
#print "=== Add them to the TaskContainer."
#cookies = []
#cookies.append(tc.add(taskTASK1))
#cookies.append(tc.add(taskTASK2))
#print "=== Cookies are:",str(cookies)
#
#print "=== Listing Tasks:", str(tc.listTasks())
#
#print "=== Taskcontainer size is:", tc.size()
#
#print "=== Run all tasks."
#tc.run()
#
#print "=== Quickly cancel everything"
#tc.cancel()
# 
#print "=== States of Tasks:", str(tc.getStates())
#   
#tup = tc.getTasks()
#print "=== Get tasks:",tup
#
#gt = tc.getTask( tc.listTasks()[0] )
#
#print "=== States of Tasks:", str(tc.getStates())
#print "-result:", tc.waitFor(WaitMode.ANY).getResult()
#print "=== States of Tasks:", str(tc.getStates())
#print "-result:", 
#
#while tc.getStates()[0] == 2:
#    from time import sleep
#    sleep(1)
#    print "+",
#     
#x = tc.waitFor(WaitMode.ALL)
#print "...",
#print x.getResult()
