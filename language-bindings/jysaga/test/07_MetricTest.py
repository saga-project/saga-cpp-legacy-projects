from saga.session import Session
from saga.o import Context
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
        print "get_type:    "+ str(o.get_type() ) + ", ObjectType.METRIC is " +str(ObjectType.METRIC)
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

print "==================================================="
print "== Test of Metric                                =="
print "==================================================="

print "=== Doing nothing yet"
       
#__init__(self, name, desc, mode, unit, mtype, value)
#Initializes the Metric object.     
#int     
#add_callback(self, cb)
#Add asynchronous notifier callback to watch metric changes.     
#      
#remove_callback(self, cookie)
#Remove a callback from a metric.     
#      
#fire(self)
#Push a new metric value to the backend.



print "==================================================="
print "== End Test of Metric                            =="
print "==================================================="