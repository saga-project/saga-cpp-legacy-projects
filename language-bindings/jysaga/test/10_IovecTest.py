from saga.session import Session
from saga.context import Context
from saga.error import *
from saga.object import ObjectType, Object
from saga.buffer import Buffer
import array.array
from saga.file import File, Directory, Iovec
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
        print "=== get_id:     "+ str(o.get_id()   )
    except Exception, e:
        print "!!! get_id(): ", str(e)
    try:
        print "=== get_type:   "+ str(o.get_type() ) + ", ObjectType.Iovec is " +str(ObjectType.IOVEC)
    except Exception, e:
        print "!!! get_type:", str(e) 
    try:   
        session = o.get_session()
        print "=== get_session:"+ str(session.__class__),"get_id:" + str(session.get_id()),"type: "+str(session.get_type()), "ObjectType.SESSION "+str(ObjectType.SESSION)                 
    except Exception, e:
        print "!!! get_session():" + str(e) 
    try:    
        clone = o.clone()
        print "=== clone:      ", str(clone.__class__), "type: "+str(clone.get_type()),"get_id:",str(clone.get_id()),"get_size()",str(clone.get_size())
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
print "== Test of Iovec                                                      =="
print "========================================================================"

max_size = 30


Iovec1 = Iovec(max_size, data=None, len_in=2, offset=10)
#__init__(self, size=-1, data=None, len_in=-1, offset=0)
#initialize an Iovec instance     

print "=== set_offset(6)"  
Iovec1.set_offset(6)
     
#set_offset(self, offset)
#set offset     
print "=== set_offset()" ,Iovec1.get_offset()
#int     
#get_offset(self)
#retrieve the current value for offset     

print "=== set_len_in(5)"
Iovec1.set_len_in(5)      
#set_len_in(self, len_in)
#Set len_in     

print "=== get_len_in()", Iovec1.get_len_in()
#int     
#get_len_in(self)
#retrieve the current value for len_in     

print "=== get_len_out()"
Iovec1.get_len_out()
#int     
#get_len_out(self)
#Retrieve the value for len_out

    
print "=== Creating implementation managed Iovec, size",max_size+1
Iovec2 = Iovec(max_size+1)
print "=== get_size():" , str(Iovec2.get_size()),
print "\t\tset_size(",max_size,")", 
Iovec2.set_size(max_size)
print "\t\tget_size():" , str(Iovec2.get_size())

print "=== Creating application managed Iovec, size", max_size,
carray = array.array('c')
Iovec3 = Iovec(max_size,carray)
print "\tget_size():" , str(Iovec3.get_size())

temp_filename = "/tmp/10_IovecTest.py.temp"
print "=== create test file(s)", temp_filename
file = open( temp_filename, "w")
file.write("abcdefghijklmnopqrstuvwxyz")
file.flush()
file.close()

url = URL("file://"+temp_filename)
print "=== Reading from", temp_filename, "in Iovecs"

f = File(url)
#print "f.read(buf=Iovec2)    ",
l2 = (f.read(Iovec2.get_size(),Iovec2))
f = File(url)
#print "f.read(buf=Iovec3)    ",
l3 = (f.read(Iovec3.get_size(),buf=Iovec3))
if l2 != l3:
    print "!!! read sizes are not equal."

t2 =  Iovec2.get_data()
t3 =  carray.tostring() 

carray2 = array.zeros('c', max_size)
Iovec3.set_data(carray2)
try:
    carray3 = array.array('c')
    Iovec3.set_data(carray3)
    print "!!! Should raise a BadParameter!"
except BadParameter,e:
    pass
    
f = File(url)
#print "== Reading from", temp_filename, "in buffer3 (carray2)"
#print "f.read(buf=Iovec3)    ",
l4 = (f.read(Iovec3.get_size(),buf=Iovec3))

t4 = carray2.tostring()
 
for i in range(Iovec2.get_size()):
    if t2[i] != t3[i]:
        print "!!! character on position",i,"differs between Iovec2 and Iovec3"
        break
    if i == Iovec2.get_size()-1:
        print "=== Iovec2 and Iovec 3 are equal"
        break

for i in range(Iovec2.get_size()):
    if t2[i] != t4[i]:
        print "!!! character on position",i,"differs between Iovec2 and Iovec4"
        break
    if i == Iovec2.get_size()-1:
        print "=== Iovec2 and Iovec 4 are equal"
        break

checkObjectMethods(Iovec3)
Iovec1.close()
Iovec2.close(-0.0)
Iovec3.close(10.0)    


print "========================================================================"
print "== End Test of Iovec                                                  =="
print "========================================================================"