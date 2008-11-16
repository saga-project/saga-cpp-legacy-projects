from saga.session import Session
from saga.context import Context
from saga.error import *
from saga.object import ObjectType, Object
from saga.buffer import Buffer
import array.array
from saga.file import File
from saga.url import URL
from saga.namespace import Flags, NSEntry, NSDirectory

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
from java.lang import NullPointerException

def checkObjectMethods(o):
    try:
        print "=== get_id:      "+ str(o.get_id()   )
    except Exception, e:
        print "!!! WARNING !!! get_id(): ", str(e.__class__),str(e)
    try:
        print "=== get_type:    "+ str(o.get_type() ) + \
            ", ObjectType.NSDIRECTORY is " +str(ObjectType.NSDIRECTORY)
    except Exception, e:
        print "!!! WARNING !!!", "get_type:", str(e.__class__),str(e) 
    try:   
        session = o.get_session()
        print "=== get_session: "+ str(session.__class__),"get_id:" \
            + str(session.get_id()),"type: "+str(session.get_type()), \
            "ObjectType.SESSION "+str(ObjectType.SESSION)                 
    except Exception, e:
        print "get_session(): " + str(e.__class__),str(e) 
    try:    
        clone = o.clone()
        print "=== clone:       "+ str(clone.__class__) + " type: "\
            +str(clone.get_type()) + " get_id:" + str(clone.get_id())
    except Exception, e:
        print "!!! WARNING !!!", "clone(): " + str(e.__class__),str(e) 
    try:
        url = URL("/tmp")
        clone.is_dir(url)
    except NullPointerException, e:
        print "!!! WARNING !!!", "clone.is_dir(): " + str(e.__class__),str(e)
    



    
print "========================================================================"
print "== Test of NSDirectory                                                =="
print "========================================================================"

task = None
temp_filename = "/tmp/09_NSDirectoryTest.py.temp"
temp_dir = "/tmp"
temp_dirname = "/tmp/09_NSDirectoryDIRECTORY"
temp_filename_link = "/tmp/08_NSEntryTest.py.link"
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

try:
    os.mkdir(temp_dirname)
except:
    import java.io.File
    f = java.io.File(temp_dirname)
    f.mkdir() 

url_parent = URL(temp_dir)
url_child = URL(temp_dirname)
nsd = NSDirectory(url_parent)
       
#__init__(self, name, session='default', flags=0)
#initialize the object     

print  "=== change_dir()"
nsd.change_dir(url_child)
nsd.change_dir(url_parent)
      
#change_dir(self, url)
#change the working directory     

temp = nsd.list()
print "=== list(): List with", str(len(temp)),
if(len(temp)) == 0:
    print "entries",
else:
    print str(temp[0].__class__), "objects"
#tuple 
#list(self, name_pattern='.', flags=0)
#list entries in this directory     

print "=== find(",temp_dirname,"):", nsd.find(temp_dirname)

#tuple     
#find(self, name_pattern, flags=2)
#find entries in the current directory and below     

url_entry = URL(temp_filename)
print "=== exists(",temp_filename,"):", nsd.exists(url_entry)
#bool     
#exists(self, name)
#checks if entry exists     

print "=== is_dir(",temp_dirname,"):", nsd.is_dir(url_child)
#bool     
#is_dir(self, name)
#tests name for being a directory     

url_entry = URL(temp_filename)
print "=== is_entry(",temp_filename,"):", nsd.is_entry(url_entry)
#bool     
#is_entry(self, name)
#tests name for being an NSEntry     

url_link = URL(temp_filename_link)
try:
    nsd.is_link(url_link)
    print "=== is_link(",temp_filename_link,"):", nsd.is_link(url_link)
except Exception, e:
    print "=== is_link():", str(e.__class__), str(e)
    
#bool     
#is_link(self, name)
#tests name for being a symbolic link     

try:
    temp = nsd.read_link(url_link)
    if not isinstance(temp,URL):
        print "read_link() does not return a URL"
    if temp.get_type() != ObjectType.URL:
        print "read_link.get_type() does not return a ObjectType.URL"
    print "=== read_link() on",url_link.get_string(),"is", temp.get_string()
except Exception,e:
    print "=== read_link():", str(e.__class__), str(e)

#URL     
#read_link(self, name)
#returns the name of the link target     

print "=== get_num_entries():", nsd.get_num_entries()
#int     
#get_num_entries(self)
#gives the number of entries in the directory     

temp = nsd.list()
print "=== get_entry(0):", nsd.get_entry(0).get_string()
#URL     
#get_entry(self, entry)
#gives the name of an entry in the directory     
#      

url1 = URL("file://"+temp_filename)
url2 = URL("file://"+temp_filename+"copy")

nsd.copy(url1, url2, Flags.OVERWRITE)
print "=== copied", url1.get_string(),"to",url2.get_string()
print "=== exists(",url2.get_string(),"):", nsd.exists(url2)

#copy(self, source, target, flags=0)
#copy the entry to another part of the name space     

url_moved = URL("file://"+temp_dirname+"move")
try:
    nsd.move(url_child, url_moved)
    print "=== moved copy to move"
    print "=== exists(",url_child.get_string(),"):", nsd.exists(url_child)
    print "=== exists(",url_moved.get_string(),"):", nsd.exists(url_moved)  
except Exception,e: 
    print "!!! move():", str(e.__class__), str(e)
    
#move(self, source, target, flags=0)
#rename or move target   

url_link = URL("file://"+temp_filename_link)
url_filename = URL("file://"+temp_filename)

try:
    nsd.link(url_filename, URL(temp_filename_link)) 
    print "=== link()"
except Exception,e:
    print "!!! link():", str(e.__class__), str(e)
         
#link(self, source, target, flags=0)
#create a symbolic link from the target entry.     

try:
    nsd.remove(url_moved)
    print "=== remove moved file"
    print "=== exists(",url_moved.to_string(),"):", nsd.exists(url_moved)
except Exception,e: 
    print "!!! remove():", str(e.__class__), str(e)

#remove(self, target, flags=0)
#removes the entry     

url_temp_dirs = URL(temp_dirname+"/low/lower/lowest")
nsd.make_dir(url_temp_dirs, Flags.CREATEPARENTS)  
print "=== make_dir()"
try:
    nsd.remove(url_temp_dirs, Flags.RECURSIVE)
    print "=== remove(temp_dirs)"
except Exception,e: 
    print "!!! remove():", str(e.__class__), str(e)

  
    
#make_dir(self, target, flags=0)
#creates a new directory     

url_entry = URL(temp_filename)
temp = nsd.open(url_entry)
if not isinstance(temp, NSEntry):
    print "!!! open() does not return a NSEntry"
print "=== open(): returned value is an entry:", temp.is_entry_self()

#NSEntry     
#open(self, name, flags=0)
#creates a new NSEntry instance     


temp = nsd.open_dir(url_parent)
if not isinstance(temp, NSDirectory):
    print "!!! open_dir() does not return a NSDirectory"
print "=== open_dir(): returned value exists:", nsd.exists(url_parent)
#NSDirectory     
#open_dir(self, name, flags=0)
#creates a new NSDirectory instance     


from saga.permissions import Permission
try:
    nsd.permissions_allow(url_entry, "uhmmm",Permission.ALL, 0)
except Exception ,e:
    print "!!! permission_allow():" , str(e.__class__), str(e)   

#permissions_allow(self, target, id, perm, flags=0)
#enable a permission     

try:
    nsd.permissions_deny(url_entry,"uhmmm",Permission.QUERY, 0)
except Exception ,e:
    print "!!! permission_deny():" , str(e.__class__), str(e)     
  
#permissions_deny(self, target, id, perm, flags=0)
#disable a permission flag

print "=== Inherited from Object =============================================="
checkObjectMethods(nsd)
#Inherited from object.Object: clone, get_id, get_session, get_type

print "=== Inherited from NSEntry ============================================="


url = URL(temp_filename)
sess = Session(False)
nse1 = NSDirectory(url)
nse2 = NSDirectory(url,sess)
nse3 = NSDirectory(url, sess, Flags.CREATE)
nse3 = File(url, flags = Flags.CREATE)

#__init__(self, name, session= Session(), flags=0)
#initialize the the object     

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
    nse4  = NSDirectory(url_link)
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
nse4 = NSDirectory(url_copy)
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
    nse5 = NSDirectory(url_link_copy)
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
    nse6 = NSDirectory(url_copy)
    nse6.remove_self()
except:
    pass

nse4.move_self(url_copy)
print "=== moved original to copy"
nse5 = NSDirectory(url_copy)
temp = nse5.is_entry_self()
if not isinstance(temp,type(False)):
    print "is_entry_self() on the copy does not return a bool"
print "=== is_entry_self() on moved:", temp
nse4.close()

#move_self(self, target, flags=0)
#rename or move target     

url_copy = URL(temp_filename+"moved")
nse6 = NSDirectory(url_copy)
nse6.remove_self()
print "=== remove_self() on moved"
try:
    nse6 = NSDirectory(url_copy)
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
nsd  = NSDirectory(url) 
from saga.permissions import Permission
try:
    nsd.permissions_allow_self("uhmmm",Permission.ALL, 0)
except Exception ,e:
    print "!!! permission_allow_self():" , str(e.__class__), str(e)   

#permissions_allow_self(self, id, perm, flags=0)
#enable a permission     

try:
    nsd.permissions_deny_self("uhmmm",Permission.QUERY, 0)
except Exception ,e:
    print "!!! permission_deny_self():" , str(e.__class__), str(e)
      
#permissions_deny_self(self, id, perm, flags=0)
#disable a permission flag     
#


print "=== Inherited from Permissions=========================================="


try:
#if True:
    temp = nsd.get_group()
    if type(temp) is not str:
        print "=== get_group() does not return a string"
    print "=== get_group():", temp
except  Exception ,e:
    print "!!! get_group():" , str(e.__class__), str(e)

#string get_group(self, tasktype=0)
#Get the group owning the entity.

try:
    temp = nsd.get_owner()
    if type(temp) is not str:
        print "=== get_owner() does not return a string"
    print "=== get_owner():", temp
except  Exception ,e:
    print "!!! get_owner():" , str(e.__class__), str(e)

#get_owner(self, tasktype=0)
#Get the owner of the entity. 



try:
    nsd.permissions_allow_self("uhmmm",Permission.ALL, 0)
except Exception ,e:
    print "!!! permission_allow_self():" , str(e.__class__), str(e) 

#permissions_allow(self, id, perm, tasktype=0)
#Enable permission flags.     

try:
    nsd.permissions_deny_self("uhmmm",Permission.QUERY, 0)
except Exception ,e:
    print "!!! permission_deny_self():" , str(e.__class__), str(e)
      
#permissions_deny(self, id, perm, tasktype=0)
#Disable permission flags.     

#bool     
#permissions_check(self, id, perm, tasktype=0)
#Check permission flags.     
#string     
    
try:
    nsd.permissions_check("uhmmm",Permission.QUERY, 0)
except Exception ,e:
    print "!!! permission_check():" , str(e.__class__), str(e)

nsd.close()
del nsd

#__del__(self)
#destroy the object     


# Everything task.Async

print "======================================================================="
print "== End Test of NSDirectory                                           =="
print "======================================================================="