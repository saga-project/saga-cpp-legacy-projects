from saga.session import Session
from saga.context import Context
from saga.error import *
from saga.object import ObjectType, Object
from saga.buffer import Buffer
import array.array
from saga.file import File
from saga.url import URL

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
        print "=== get_id:      "+ str(o.get_id()   )
    except Exception, e:
        print "!!! get_id(): ", str(e)
    try:
        print "=== get_type:    "+ str(o.get_type() ) + ", ObjectType.BUFFER is " +str(ObjectType.BUFFER)
    except Exception, e:
        print "!!! get_type:", str(e) 
    try:   
        session = o.get_session()
        print "=== get_session: "+ str(session.__class__),"get_id:" + str(session.get_id()),"type: "+str(session.get_type()), "ObjectType.SESSION "+str(ObjectType.SESSION)                 
    except Exception, e:
        print "!!! get_session(): " + str(e) 
    try:    
        clone = o.clone()
        print "=== clone:       "+ str(clone.__class__) + " type: "+str(clone.get_type()) + " get_id:" + str(clone.get_id())
        clone.get_size()
    except Exception, e:
        print "!!! clone(): " + str(e) 

def printAttributes(context):
    print "Name           Ex\tRO\tREM\tVec\tWri\tValue"
    list =  context.list_attributes()
    for i in list:
        length =  14 - len(str(i)) 
        space = length * '.'
        string = str(i) + space
        print string, context.attribute_exists(i),"\t" , 
        if (context.attribute_is_readonly(i)) is True: print 1,
        else: print 0,
        print "\t" ,str(context.attribute_is_removable(i)), "\t" ,str(context.attribute_is_vector(i)), \
          "\t" ,str(context.attribute_is_writable(i)),"\t", str(context.get_attribute(i))

print "========================================================================"
print "== Test of Buffer                                                     =="
print "========================================================================"

max_size = 30

print "=== Creating empty implementation managed Buffer"
buffer1 = Buffer()
print "=== get_size():" , str(buffer1.get_size()),
print "\t\tset_size(",max_size,")", 
buffer1.set_size(max_size)
print "\t\tget_size():" , str(buffer1.get_size())
checkObjectMethods(buffer1)
    
print "=== Creating implementation managed Buffer, size",max_size
buffer2 = Buffer(max_size)
print "=== get_size():" , str(buffer2.get_size()),
print "\t\tset_size(",max_size+1,")", 
buffer2.set_size(max_size+1)
print "\t\tget_size():" , str(buffer2.get_size())

print "=== Creating application managed Buffer, size", max_size,
carray = array.array('c')
buffer3 = Buffer(max_size,carray)
print "\tget_size():" , str(buffer3.get_size())
checkObjectMethods(buffer3)

temp_filename = "/tmp/04_BufferTest.py.temp"
print "=== create test file(s)", temp_filename
file = open( temp_filename, "w")
file.write("abcdefghijklmnopqrstuvwxyz")
file.flush()
file.close()

url = URL("file://"+temp_filename)
#print "== Reading from", temp_filename, "in buffers"

f = File(url)
#print "f.read(buf=buffer1)    ",
l1 = f.read(buffer1.get_size(),buffer1)
f = File(url)
#print "f.read(buf=buffer2)    ",
l2 = (f.read(buffer2.get_size(),buffer2))
f = File(url)
#print "f.read(buf=buffer3)    ",
l3 = (f.read(buffer3.get_size(),buf=buffer3))
if l1 != l2 or l1 != l3:
    print "!!! read sizes are not equal."


t1 =  buffer1.get_data()
t2 =  buffer2.get_data()
t3 =  carray.tostring() 

carray2 = array.zeros('c', max_size)
buffer3.set_data(carray2)
try:
    carray3 = array.array('c')
    buffer3.set_data(carray3)
    print "!!! Should raise a BadParameter!"
except BadParameter,e:
    pass
    
f = File(url)
#print "== Reading from", temp_filename, "in buffer3 (carray2)"
#print "f.read(buf=buffer3)    ",
l4 = (f.read(buffer3.get_size(),buf=buffer3))

t4 = carray2.tostring()

for i in range(buffer1.get_size()):
    if t1[i] != t2[i]:
        print "!!! character on position",i,"differs between buffer1 and buffer2"
        break
    if i == buffer1.get_size()-1:
        print "=== buffer1 and buffer 2 are equal"
        break
 
 
for i in range(buffer1.get_size()):
    if t1[i] != t3[i]:
        print "!!! character on position",i,"differs between buffer1 and buffer3"
        break
    if i == buffer1.get_size()-1:
        print "=== buffer1 and buffer 3 are equal"
        break

for i in range(buffer1.get_size()):
    if t1[i] != t4[i]:
        print "!!! character on position",i,"differs between buffer1 and buffer4"
        break
    if i == buffer1.get_size()-1:
        print "=== buffer1 and buffer 4 are equal"
        break

buffer1.close()
buffer2.close(-0.0)
buffer3.close(10.0)    


print "========================================================================"
print "== End Test of Buffer                                                 =="
print "========================================================================"