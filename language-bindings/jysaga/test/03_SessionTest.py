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
        print "get_id:      "+ str(o.get_id()   )
    except Exception, e:
        print "!!! WARNING !!! get_id(): ", str(e)
    try:
        print "get_type:    "+ str(o.get_type() ) + ", ObjectType.BUFFER is " +str(ObjectType.BUFFER)
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

print "==================================================="
print "== Test of Session                               =="
print "==================================================="

print "=== Creating empty session"
print "=== buffer1 = Buffer()"
buffer1 = Buffer()
print "get_size:      " , str(buffer1.get_size())
print "set_size(50):  " , str(buffer1.set_size(50))
print "get_size:      " , str(buffer1.get_size())
checkObjectMethods(buffer1)
    
print "=== Creating implementation managed Buffer, size 100"
print "=== buffer2 = Buffer(100)"
buffer2 = Buffer(100)
print "get_size:      " , str(buffer2.get_size())
print "get_size(119): " , str(buffer2.set_size(119))
print "get_size:      " , str(buffer2.get_size())
checkObjectMethods(buffer2)

print "=== Creating application managed Buffer, size 101"
print "=== buffer3 = Buffer(101,carray)"
carray = array.array('c')
buffer3 = Buffer(101,carray)
print "get_size:" , str(buffer3.get_size())
checkObjectMethods(buffer3)

temp_filename = "/tmp/03_SessionTest.py.temp"
print "=== create test file(s)", temp_filename
file = open( temp_filename, "w")
file.write("abcdefghijklmnopqrstuvwxyz")
file.flush()
file.close()




print "==================================================="
print "== End Test of Session                           =="
print "==================================================="