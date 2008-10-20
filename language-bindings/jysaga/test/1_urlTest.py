from saga.url  import URL

def printParts(url):
    t = url.get_string()
    print "get_string:       " + str(t) + " " + str(type(t)) 
    t =  url.get_scheme() 
    print "get_scheme:       " + str(t) + " " + str(type(t)) 
    t = url.get_host()
    print "get_get_host:     " + str(t) + " " + str(type(t)) 
    t = url.get_port()
    print "get_get_port:     " + str(t) + " " + str(type(t))
    t = url.get_fragment()
    print "get_get_fragment: " + str(t) + " " + str(type(t))
    t = url.get_path()
    print "get_get_path:     " + str(t) + " " + str(type(t))
    t = url.get_query()
    print "get_query:        " + str(t) + " " + str(type(t))  
    t = url.get_userinfo()
    print "get_userinfo:     " + str(t) + " " + str(type(t))   
    
def setParts(url):
    print "set_scheme:       " + url.set_scheme("scheme://")
    print "set_get_host:     " + url.set_host("host.net")  
    print "set_get_port:     " + url.set_port(80)
    print "set_get_fragment: " + url.set_fragment("fragment")
    print "set_get_path:     " + url.set_path("path")
    print "set_query:        " + url.set_query("query")  
    print "set_userinfo:     " + url.set_userinfo("userinfo")       


def checkObjectMethods(o):
    print "- Inherited Methods from Object:"
    print "get_id:      "+ str(o.get_id()   )
    print "get_type:    "+ str(o.get_type() )
    session = o.get_session()
    print "get_session: "+ str(session.__class__) + " type: "+str(sessionType.get_type()) + " get_id:" + str(session.get_id())                 
    clone = o.clone()
    print "clone:       "+ str(clone.__class__) + " type: "+str(clone.get_type()) + " get_id:" + str(clone.get_id())
    print "ObjectType.SESSION is defined as "+str(ObjectType.SESSION) + ", ObjectType.URL is " +str(ObjectType.URL)

print "==================================================="
print "== Test of URL                                   =="
print "==================================================="

temp_filename = "/tmp/1_urlTest.py.temp"

print "- create test file(s)"
file = open( temp_filename, "w")
file.write("abcdefghijklmnopqrstuvwxyz")
file.flush()
file.close()

print "- created " + temp_filename
print "- create an -EMPTY- URL"

url = URL()
printParts(url)
setParts(url)
printParts(url)
set_string(url='ftp://ftp.bla.nl:21/file.txt?query=something')
printParts(url) 

  
print "-Translate, give string, get URL"   
sagaURL = translate(self, temp_filename)
print "translate() output.__class__ :" + sagaURL.__class__ + "get_string: " + sagURL.get_string() 

import os
os.remove(temp_filename)