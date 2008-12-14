from saga.session import Session
from saga.context import Context
from saga.error import *
from saga.object import ObjectType, Object
from saga.job import JobDescription

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
    except NotImplemented, e:
        print "!!! WARNING !!! get_id() not implemented"
    try:
        print "get_type:    "+ str(o.get_type() ) + ", ObjectType.CONTEXT is " +str(ObjectType.CONTEXT)
    except Exception, e:
        print "!!! WARNING !!!", str(e) 
    try:   
        session = o.get_session()
        print "get_session: "+ str(session.__class__) + " type: "+str(session.get_type()) + " get_id:" + str(session.get_id())                 
    except Exception, e:
        print "!!! WARNING !!!", "Context.get_session(): " + str(e) 
#    try:    
    clone = o.clone()
    print "clone:       "+ str(clone.__class__) + " type: "+str(clone.get_type()) + " get_id:" + str(clone.get_id())
    print "ObjectType.SESSION "+str(ObjectType.SESSION)
#    except Exception, e:
#        print "!!! WARNING !!!", "Context.clone(): " + str(e) 

def printAttributes(context):
    print "Name                Ex\tRO\tREM\tVec\tWri\tValue"
    list =  context.list_attributes()
    for i in list:
        length =  20 - len(str(i)) 
        space = length * '.'
        string = str(i) + space
        print string, 
        print context.attribute_exists(i),"\t" , 
        if (context.attribute_is_readonly(i)) == True: 
            print 1,
        else: 
            print 0,
        print "\t" ,str(context.attribute_is_removable(i)), 
        print "\t" ,str(context.attribute_is_vector(i)), 
        print "\t" ,str(context.attribute_is_writable(i)), 
        if not context.attribute_is_vector(i):  
            print "\t", str(context.get_attribute(i))
        else:
            print "\t", str(context.get_vector_attribute(i))     

def setAttributes(context):
    try:
        context.set_attribute("CertRepository", "/etc/grid-security/certificates/")
    except Exception, e:
        print "!!! WARNING !!!", str(e)
    try:
        context.set_attribute("UserProxy","/tmp/x509up_u01234")
    except Exception, e:
        print "!!! WARNING !!!", str(e)    
    try:
        context.set_attribute("Type","http")
    except Exception, e:
        print "!!! WARNING !!!", str(e)
    try:
        context.set_attribute("UserID","testname")
    except Exception, e:
        print "!!! WARNING !!!", str(e)
    try:    
        context.set_attribute("Server","www.few.vu.nl")
    except Exception, e:
        print "!!! WARNING !!!", str(e)
    try:
        context.set_attribute("UserVO","O=dutchgrid")
    except Exception, e:
        print "!!! WARNING !!!", str(e)    
    try:
        context.set_attribute("RemoteID","/O=dutchgrid/O=users/O=vu/OU=cs/CN=Joe Doe")
    except Exception, e:
        print "!!! WARNING !!!", str(e)
    try:
        context.set_attribute("LifeTime","1247781600")
    except Exception, e:
        print "!!! WARNING !!!", str(e)   
    try:    
        context.set_attribute("RemoteHost","galjas.few.vu.nl")
    except Exception, e:
        print "!!! WARNING !!!", str(e)
    try:    
        context.set_attribute("UserPass","super_secret_password")
    except Exception, e:
        print "!!! WARNING !!!", str(e)
    try:    
        context.set_attribute("RemotePort","80")
    except Exception, e:
        print "!!! WARNING !!!", str(e)
    try:    
        context.set_attribute("UserCert","$HOME/.globus/usercert.pem")
    except Exception, e:
        print "!!! WARNING !!!", str(e)
    try:    
        context.set_attribute("UserKey","$HOME/.globus/userkey.pem")
    except Exception, e:
        print "!!! WARNING !!!", str(e)
#    try:    
#        context.set_attribute("<KEY>","<VALUE>")
#    except Exception, e:
#        print "!!! WARNING !!!", str(e)
#    try:    
#        context.set_vector_attribute("<KEY>",<LIST_OF_VALUES>)
#    except Exception, e:
#        print "!!! WARNING !!!", str(e)        
                        
def checkMethods(context, name):
    print "=== Check all context methods, name and return values"
    print "set_defaults     " + str(context.set_defaults())   
    print "list_attributes  " + str(context.list_attributes())
    print "attribute_exists (" +name+ ")      " + str(context.attribute_exists(name))
    print "attribute_is_readonly (" +name+ ") " + str(context.attribute_is_readonly(name))
    print "attribute_is_removable(" +name+ ") " + str(context.attribute_is_removable(name))
    print "attribute_is_vector (" +name+ ")   " + str(context.attribute_is_vector(name))
    print "attribute_is_writable (" +name+ ") " + str(context.attribute_is_writable(name))
    print "find_attributes ([U*])             " + str(context.find_attributes(["U*"]))
    tup = context.find_attributes(["U*", "*ote*", "*er*"])
    temp = list(tup)
    temp.sort()                               
    print "find_attributes ([U*,*ote*,*erv*]) " + str(temp)
    print "get_attribute(" +name+ ")          " + str(context.get_attribute(name))

print "==================================================="
print "== Test of JobDescription                        =="
print "==================================================="

j = JobDescription()
printAttributes(j)


print "==================================================="
print "== End Test of JobDescription                    =="
print "==================================================="