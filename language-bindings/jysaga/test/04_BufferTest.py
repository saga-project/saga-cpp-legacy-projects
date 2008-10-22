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
print "== Test of Buffer                                =="
print "==================================================="

print "=== Creating empty implementation managed Buffer"
print "=== buffer1 = Buffer()"
buffer1 = Buffer()
print "get_size:      " , str(buffer1.get_size())
print "get_size(50):  " , str(buffer1.set_size(50))
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


print "=== create test file(s)", temp_filename
temp_filename = "/tmp/1_urlTest.py.temp"
file = open( temp_filename, "w")
file.write("abcdefghijklmnopqrstuvwxyz")
file.flush()
file.close()

url = URL("file://"+temp_filename)
f = File(url) 
print f.read()     
#__init__(self, session, name, flags=512)
#initialize the File object     
#int     
#get_size(self)
#returns the number of bytes in the file     
#int or string     
#read(self, len=-1, buf=None)
#reads up to len bytes from the file into a buffer     
#int     
#write(self, buf, len=-1)
#writes up to len from buffer into the file at the current file position.     
#int     
#seek(self, offset, whence=0)
#reposition the file pointer     
#      
#read_v(self, iovecs)
#gather/scatter read     
#      
#write_v(self, iovecs)
#gather/scatter write     
#int     
#size_p(self, pattern)
#determine the storage size required for a pattern I/O operation     
#int     
#read_p(self, pattern, buf)
#pattern-based read     
#int     
#write_p(self, pattern, buf)
#pattern-based write     
#tuple     
#modes_e(self)
#list the extended modes available in this implementation, and/or on server side     
#int     
#size_e(self, emode, spec)
#determine the storage size required for an extended I/O operation     
#int     
#read_e(self, emode, spec, buf)
#extended read     
#int     
#write_e(self, emode, spec, buf)
#extended write



buffer1.close()
buffer2.close(-0.0)
buffer3.close(10.0)    

  
#set_size(size=-1)
#Set size of buffer.     
#int     
#get_size(self)
#Retrieve the current value for size.     
#      
#set_data(self, data, size=-1)
#Set new buffer data.     
#char array or list     
#get_data(self)
#Retrieve the buffer data.     



print "==================================================="
print "== End Test of Buffer                            =="
print "==================================================="