from saga.url  import URL
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
        print "get_type:    "+ str(o.get_type() ) + ", ObjectType.URL is " +str(ObjectType.URL)
    except Exception, e:
        print "!!! WARNING !!!", str(e) 
    try:   
        session = o.get_session()
        print "get_session: "+ str(session.__class__) + " type: "+str(session.get_type()) + " get_id:" + str(session.get_id())                 
    except Exception, e:
        print "!!! WARNING !!!", str(e) 
    try:    
        clone = o.clone()
        print "clone:       "+ str(clone.__class__) + " type: "+str(clone.get_type()) + " get_id:" + str(clone.get_id())
        print "ObjectType.SESSION "+str(ObjectType.SESSION)
    except Exception, e:
        print "!!! WARNING !!!", str(e) 

print "==================================================="
print "== Test of URL                                   =="
print "==================================================="

temp_filename = "/tmp/1_urlTest.py.temp"
translate_url = "http://host.net/~user/tmp/file"
#"gopher://userinfo@floodgate.net:80/path/to/water.txt?query#info"

#print "- create test file(s)"
#file = open( temp_filename, "w")
#file.write("abcdefghijklmnopqrstuvwxyz")
#file.flush()
#file.close()

#print "=== created " + temp_filename
print "=== create an -EMPTY- URL"

url = URL()
printParts(url)

print "=== create an URL of " + temp_filename
url = URL("file://"+temp_filename)
printParts(url)

print "=== create an URL of ftp://ftp.is.co.za/rfc/rfc1808.txt"
url = URL("ftp://ftp.is.co.za/rfc/rfc1808.txt")
printParts(url)

print "=== set new values for the URL"
setParts(url)
print "=== show new values"
printParts(url)
print "=== set string to ftp://ftp.bla.nl:21/file.txt?query=something"
url.set_string(url='ftp://ftp.bla.nl:21/file.txt?query=something')
printParts(url) 

  
print "=== Translate, give scheme string, get URL"   
sagaURL = url.translate("gopher")
print "translate() output class:      " + str(sagaURL.__class__ )
print "translate() output get_string: " + sagaURL.get_string() 

print "=== Check inherited Object methods"
checkObjectMethods(sagaURL)

#import os
#os.remove(temp_filename)

print "==================================================="
print "== End Test of URL                               =="
print "==================================================="