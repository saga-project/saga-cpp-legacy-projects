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
        print "get_type:    "+ str(o.get_type() ) + ", ObjectType.SESSION is " +str(ObjectType.SESSION)
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
session = Session()
print "=== Listing session contexts"
contexts = session.list_contexts()
print "Contexts:",str(type(contexts)), str(contexts)
print "=== Create context (ftp)"
context = Context("ftp")
print "=== add context"
session.add_context(context)
print "=== Listing session contexts"
contexts = session.list_contexts()
print "Contexts:",str(type(contexts)), str(contexts)
for i in range(len(contexts)):
    print "list_attributes["+str(i)+"]" + str(contexts[i].list_attributes())
print "=== Create context (globus)"
context2 = Context("globus")
print "=== add context"
session.add_context(context2)
contexts = session.list_contexts()
print "Contexts:",str(type(contexts)), str(contexts)
for i in range(len(contexts)):
    print "list_attributes["+str(i)+"]" + str(contexts[i].list_attributes())
print "=== remove first context"
try:
    print contexts[0].delegateObject
    session.remove_context(contexts[0])
except Exception,e:
    print str(e.__class__), str(e), "(remove_context doesn't work yet)"
print "=== Listing session contexts"
contexts = session.list_contexts()
print "Contexts:",str(type(contexts)), str(contexts)
checkObjectMethods(session)

print "==================================================="
print "== End Test of Session                           =="
print "==================================================="