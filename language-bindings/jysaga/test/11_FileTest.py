from saga.session import Session
from saga.context import Context
from saga.error import *
from saga.object import ObjectType, Object
from saga.buffer import Buffer
import array.array
from saga.file import File, Directory, Iovec, SeekMode
from saga.url import URL
from saga.namespace import Flags, NSEntry, NSDirectory

#import org.ogf.saga.error.AlreadyExistsException
#import org.ogf.saga.error.AuthenticationFailedException 
#import org.ogf.saga.error.AuthorizationFailedException
#import org.ogf.saga.error.BadParameterException 
#import org.ogf.saga.error.DoesNotExistException
#import org.ogf.saga.error.IncorrectStateException
#import org.ogf.saga.error.IncorrectURLException 
#import org.ogf.saga.error.NoSuccessException 
#import org.ogf.saga.error.NotImplementedException
#import org.ogf.saga.error.PermissionDeniedException
#import org.ogf.saga.error.SagaException 
#import org.ogf.saga.error.SagaIOException 
#import org.ogf.saga.error.TimeoutException 
#from java.lang import NullPointerException

def checkObjectMethods(o):
    try:
        print "=== get_id:      "+ str(o.get_id()   )
    except Exception, e:
        print "!!! WARNING !!! get_id(): ", str(e.__class__),str(e)
    try:
        print "=== get_type:    "+ str(o.get_type() ) + ", ObjectType.FILE is "\
        +str(ObjectType.FILE)
    except Exception, e:
        print "!!! WARNING !!!", "get_type:", str(e.__class__),str(e) 
    try:   
        session = o.get_session()
        print "=== get_session: "+ str(session.__class__),"get_id:" + \
            str(session.get_id()),"type: "+str(session.get_type()), \
            "ObjectType.SESSION "+str(ObjectType.SESSION)                 
    except Exception, e:
        print "get_session(): " + str(e.__class__),str(e) 
    try:    
        clone = o.clone()
        print "=== clone:       "+ str(clone.__class__) + " type: "+str(clone.get_type())\
             + " get_id:" + str(clone.get_id())
    except Exception, e:
        print "!!! WARNING !!!", "clone(): " + str(e.__class__),str(e) 
    try:
        clone.is_entry_self()
    except NullPointerException, e:
        print "!!! WARNING !!!", "clone.is_entry(): " + str(e.__class__),str(e)
    

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

def checkMonitorableMethods(o):
    metric_list = o.list_metrics()
    print "=== List metrics", str(metric_list)
    metr = o.get_metric('Task.state')
 
    from time import sleep 

    c = Callb(True)
    cookie =  o.add_callback('Task.state', c)
    print "=== Callback added to taskTask. Cookie was", cookie, "state:",o.get_state()
    print "=== taskTask.run()"
    o.run()                                
    while o.get_state() == 2:              
        for i in range(5):
            sleep(1)
            print "+",    
    print "=== taskTask.get_state():", o.get_state()
    print "=== remove callback",
    o.remove_callback('Task.state', cookie)
    print o.get_result()
    print " done"

    
print "========================================================================"
print "== Test of File                                                       =="
print "========================================================================"

task = None
temp_filename = "/tmp/11_FileTest.py.temp"
temp_filename_link = "/tmp/11_FileTest.py.link"
print "=== create test file(s)", temp_filename
file = open( temp_filename, "w")
file.write("abcdefghijklmnopqrstuvwxyz")
file.flush()
file.close()

import os
try:
    os.symlink(temp_filname,temp_filename_link)
except:
    import java.lang.Runtime
    r = java.lang.Runtime.getRuntime();
    p = r.exec("ln -s " + temp_filename + " " + temp_filename_link)

url = URL(temp_filename)
sess = Session(False)
nse1  = File(url)
nse2 = File(url,sess)
nse3 = File(url, sess, Flags.CREATE)
nse3 = File(url, flags = Flags.CREATE)
file = File(url)

#__init__(self, name, session='default', flags=512)
#initialize the File object     
#int     

temp = file.get_size()
if not isinstance(temp,int):
    print "get_size() does not return an int"
print "=== get_size()", temp
#get_size(self)
#returns the number of bytes in the file     

temp = file.read(30)
if not isinstance(temp,str):
    print "read() does not return an str"
print "=== read( 30 )", temp

#int or string     
#read(self, size=-1, buf=None)
#reads up to size bytes from the file into a buffer     

file.seek(0, SeekMode.START)
print "=== seek()"

iov = Iovec(file.get_size())
temp = file.read(26, iov)
if not isinstance(temp,int):
    print "read() does not return an int"
print "=== read(26)", temp,"\tData read:",iov.get_data()

file.seek(0, SeekMode.START)
print "=== seek()"
#int #seek(self, offset, whence=0)
#reposition the file pointer    

print "=== write()"
file.write("ABCDE")
file.seek(0,SeekMode.START)
temp = file.read(5)
if temp != "ABCDE":
    print "!!! read() does not show what has been written:",temp
 
file.seek(0,SeekMode.START)
iov.set_data( ['a','b','c','d','e'])
file.write(iov)   
file.seek(0,SeekMode.START)
temp = file.read(5)
if temp != "abcde":
    print "!!! read() does not show what has been written:",temp
#int     
#write(self, buf, size=-1)
#writes up to size from buffer into the file at the current file position.     

iov = Iovec(size = 30, len_in = 5)
iov2= Iovec(size = 30, len_in = 5)     
file.seek(0,SeekMode.START)
temp = file.read_v([iov,iov2])
print "=== read_v(): iov1:",iov.get_data()
print "=== read_v(): iov2:",iov2.get_data()      
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





temp = nse3.get_url()
if not isinstance(temp,URL):
    print "get_url() does not return a URL"
if temp.get_type() != ObjectType.URL:
    print "get_url().get_type() does not return a ObjectType.URL"
print "=== get_url()", temp.get_string()
      
#URL     
#get_url(self)
#obtain the complete url pointing to the entry     

temp = nse3.get_cwd()
if not isinstance(temp,URL):
    print "get_cwd() does not return a URL"
if temp.get_type() != ObjectType.URL:
    print "get_cwd().get_type() does not return a ObjectType.URL"
print "=== get_cwd()", temp.get_string()

#URL     
#get_cwd(self)
#obtain the current working directory for the entry     

temp = nse3.get_name()
if not isinstance(temp,URL):
    print "get_name() does not return a URL"
if temp.get_type() != ObjectType.URL:
    print "get_name().get_type() does not return a ObjectType.URL"
print "=== get_name()", temp.get_string()

#URL     
#get_name(self)
#obtain the name part of the url path element     
#bool     

temp = nse3.is_dir_self()
if not isinstance(temp,type(False)):
    print "is_dir() does not return a bool"
print "=== is_dir()", temp

#is_dir_self(self)
#tests the entry for being a directory     

temp = nse3.is_entry_self()
if not isinstance(temp,type(False)):
    print "is_entry_self() does not return a bool"
print "=== is_entry_self()", temp

#bool is_entry_self(self)
#tests the entry for being an NSEntry     

try:
    temp = nse3.is_link_self()
    if not isinstance(temp,bool):
        print "is_link_self() does not return a bool"
        print "=== is_link_self()", temp
except Exception,e:
    print "=== is_link():", str(e.__class__), str(e)

#bool is_link_self(self)
#tests the entry for being a link     

try:
    url_link = URL(temp_filename_link)
    nse4  = File(url_link)
    temp = nse4.read_link_self()
    if not isinstance(temp,URL):
        print "read_link_self() does not return a URL"
    if temp.get_type() != ObjectType.URL:
        print "read_link_self().get_type() does not return a ObjectType.URL"
    print "=== read_link_self() on",url_link.get_string(),"is", temp.get_string()
except Exception,e:
    print "=== read_link():", str(e.__class__), str(e)
#URL     
#read_link_self(self)
#get the name of the link target     

url_copy = URL(temp_filename+"copy")
nse3.copy_self(url_copy, Flags.OVERWRITE)
print "=== copy original to copy"
nse4 = File(url_copy)
temp = nse4.is_entry_self()
if not isinstance(temp,type(False)):
    print "is_entry_self() on the copy does not return a bool"
print "=== is_entry_self() on copy:", temp

#copy_self(self, target, flags=0)
#copy the entry to another part of the name space     

try:
    url_link_copy = URL(temp_filename+"link_copy")
    nse3.link_self(url_link_copy, Flags.OVERWRITE)
    print "=== link link_copy to original"
    nse5 = File(url_link_copy)
    temp = nse5.is_link_self()
    if not isinstance(temp,type(False)):
        print "is_link_self() on the copy does not return a bool"
    print "=== is_link_self() on link_copy:", temp
except Exception,e:
    print "=== link_self():", str(e.__class__), str(e)
  
#link_self(self, target, flags=0)
#create a symbolic link     

url_copy = URL(temp_filename+"moved")
try:
    nse6 = File(url_copy)
    nse6.remove_self()
except:
    pass

nse4.move_self(url_copy)
print "=== moved original to copy"
nse5 = File(url_copy)
temp = nse5.is_entry_self()
if not isinstance(temp,type(False)):
    print "is_entry_self() on the copy does not return a bool"
print "=== is_entry_self() on moved:", temp
nse4.close()

#move_self(self, target, flags=0)
#rename or move target     

url_copy = URL(temp_filename+"moved")
nse6 = File(url_copy)
nse6.remove_self()
print "=== remove_self() on moved"
try:
    nse6 = File(url_copy)
except Exception, e:
    print "=== is_entry() on removed:" , str(e.__class__), str(e)
 
      
#remove_self(self, flags=0)
#removes this entry, and closes it     
#  

try: nse1.close()
except Exception, e: print "=== close():" , str(e.__class__), str(e)
try: nse2.close(1.0)
except Exception, e: print "=== close():" , str(e.__class__), str(e)
try: nse3.close(10)
except Exception, e: print "=== close():" , str(e.__class__), str(e)
try: nse4.close(0)
except Exception, e: print "=== close():" , str(e.__class__), str(e)
try: nse5.close()
except Exception, e: print "=== close():" , str(e.__class__), str(e)
try: nse6.close()
except Exception, e: print "=== close():" , str(e.__class__), str(e)

print "=== close() on all NSEntries"

#close(self, timeout=0.0)
#closes the NSEntry Format: close (in float timeout = 0.0);     
# 

url = URL(temp_filename)
nse1  = File(url) 
from saga.permissions import Permission
try:
    nse1.permissions_allow_self("uhmmm",Permission.ALL, 0)
except Exception ,e:
    print "=== permission_allow_self():" , str(e.__class__), str(e)   

#permissions_allow_self(self, id, perm, flags=0)
#enable a permission     

try:
    nse1.permissions_deny_self("uhmmm",Permission.QUERY, 0)
except Exception ,e:
    print "=== permission_deny_self():" , str(e.__class__), str(e)
      
#permissions_deny_self(self, id, perm, flags=0)
#disable a permission flag     
#


checkObjectMethods(nse1)
#Inherited from object.Object: clone, get_id, get_session, get_type


try:
#if True:
    temp = nse1.get_group()
    if type(temp) is not str:
        print "=== get_group() does not return a string"
    print "=== get_group():", temp
except  Exception ,e:
    print "=== get_group():" , str(e.__class__), str(e)

#string get_group(self, tasktype=0)
#Get the group owning the entity.

try:
    temp = nse1.get_owner()
    if type(temp) is not str:
        print "=== get_owner() does not return a string"
    print "=== get_owner():", temp
except  Exception ,e:
    print "=== get_owner():" , str(e.__class__), str(e)

#get_owner(self, tasktype=0)
#Get the owner of the entity. 



try:
    nse1.permissions_allow("uhmmm",Permission.ALL, 0)
except Exception ,e:
    print "=== permission_allow():" , str(e.__class__), str(e) 

#permissions_allow(self, id, perm, tasktype=0)
#Enable permission flags.     

try:
    nse1.permissions_deny("uhmmm",Permission.QUERY, 0)
except Exception ,e:
    print "=== permission_deny(): " , str(e.__class__), str(e)
      
#permissions_deny(self, id, perm, tasktype=0)
#Disable permission flags.     

#bool     
#permissions_check(self, id, perm, tasktype=0)
#Check permission flags.     
#string     
    
try:
    nse1.permissions_check("uhmmm",Permission.QUERY, 0)
except Exception ,e:
    print "=== permission_check():" , str(e.__class__), str(e)

nse1.close()
del nse1

#__del__(self)
#destroy the object     


# Everything task.Async

print "========================================================================"
print "== End Test of File                                                   =="
print "========================================================================"