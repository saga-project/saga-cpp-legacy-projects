from saga.session import Session
from saga.context import Context
from saga.error import *
from saga.object import ObjectType, Object

def printParts(url):
    t = url.get_string()
    if t is None: print "get_string:    -"
    else: print "get_string:   " + str(t) + " " + str(type(t)) 
    
    t =  url.get_scheme() 
    if t is None: print "get_scheme:    -"
    else: print "get_scheme:   " + str(t) + " " + str(type(t)) 
    
    t = url.get_host()
    if t is None: print "get_host:      -"
    else: print "get_host:     " + str(t) + " " + str(type(t)) 
    
    t = url.get_port()
    if t is None: print "get_port:      -"
    else: print "get_port:     " + str(t) + " " + str(type(t))
    
    t = url.get_fragment()
    if t is None: print "get_fragment:  -"
    else: print "get_fragment: " + str(t) + " " + str(type(t))
    
    t = url.get_path()
    if t is None: print "get_path:      -"
    else: print "get_path:     " + str(t) + " " + str(type(t))
    
    t = url.get_query()
    if t is None: print "get_query:     -"
    else: print "get_query:    " + str(t) + " " + str(type(t))  
    
    t = url.get_userinfo()
    if t is None: print "get_userinfo:  -"
    else: print "get_userinfo: " + str(t) + " " + str(type(t))   
    
def setParts(url):
    print "set_scheme:       ssh"
    url.set_scheme("ssh")
    print "set_host:         host.net"
    url.set_host("host.net")
    print "set_port:         80" 
    url.set_port(80)
    print "set_fragment:     fragment"
    url.set_fragment("fragment")
    print "set_path:         /path"
    url.set_path("/path")           
    print "set_query:        query"
    url.set_query("query")
    print "set_userinfo:     userinfo" 
    url.set_userinfo("userinfo")


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

def checkMethods(context, name):
    print "=== Check all context methods, name and return values"
    print "set_defaults     " + str(context.set_defaults())   
    print "list_attributes  " + str(context.list_attributes())
    print "attribute_exists (" +name+ ")      " + str(context.attribute_exists(name))
    print "attribute_is_readonly (" +name+ ") " + str(context.attribute_is_readonly(name))
    print "attribute_is_removable(" +name+ ") " + str(context.attribute_is_removable(name))
    print "attribute_is_vector (" +name+ ")   " + str(context.attribute_is_vector(name))
    print "attribute_is_writable (" +name+ ") " + str(context.attribute_is_writable(name))
    print "find_attributes (U*)             " + str(context.find_attributes(["U*"]))
    print "get_attribute(" +name+ ")          " + str(context.get_attribute(name))

print "==================================================="
print "== Test of Context                               =="
print "==================================================="
print "=== Create -empty- Context "
context = Context()
print "=== Check all methods"
checkObjectMethods(context)
checkMethods(context, "LifeTime")
print "=== Create Context(RemoteID)  "
context = Context("RemoteID")
checkMethods(context, "RemoteID")

#for i in context.list_attributes():
#    print i, context.attribute_exists(i)
#get_vector_attribute
#
#remove_attribute
#set_attribute
#set_vector_attribute 

#TODO: Test find_attributes with longer lists

print "==================================================="
print "== End Test of Context                           =="
print "==================================================="