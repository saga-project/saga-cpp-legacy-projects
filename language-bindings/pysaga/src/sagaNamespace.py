# Page 195 package saga.namespace
from sagaObject import Object
from sagaPermission import Permissions
from sagaTask import Async
from sagaError import NotImplemented

class Flags(object):
    """ 
    
    
    """
    none            =  0
    overwrite       =  1
    recursive       =  2
    dereference     =  4
    create          =  8
    exclusive       = 16
    lock            = 32
    createParents   = 64
    

class NSEntry(Object, Async, Permissions):
    """
    
     
    """

    def __init__(self, session = None, url = None, flags=Flags.none):
        super(NSEntry,self).__init__()
    
    def copy(self, url, flags=Flags.none):
        raise NotImplemented, "copy() method is not implemented in this object" 

    def get_url(self):
        #out URL url );
        raise NotImplemented, "get_url() method is not implemented in this object"

    def get_cwd(self):
        #out URL cwd
        raise NotImplemented, "get_cwd() method is not implemented in this object"

    def get_name(self): 
        #out URL name
        raise NotImplemented, "get_name() method is not implemented in this object"

    def is_dir(self):
        #out boolean test
        raise NotImplemented, "is_dir() method is not implemented in this object"

    def is_entry(self):
        #out boolean test
        raise NotImplemented, "is_entry() method is not implemented in this object"

    def is_link(self):
        #out boolean test
        raise NotImplemented, "is_link() method is not implemented in this object"

    def read_link(self):
        #out URL link
        raise NotImplemented, "read_link() method is not implemented in this object"

    def link(self, url, flags = Flags.none):
        raise NotImplemented, "link() method is not implemented in this object"

    def move(self, url, flags = Flags.none):
        raise NotImplemented, "move() method is not implemented in this object"

    def remove(self, flags = Flags.none):
        raise NotImplemented, "remove() method is not implemented in this object"

    def close(self, timeout = 0.0):
        raise NotImplemented, "close() method is not implemented in this object"

    def permissions_allow(self, id, perm, flags = Flags.none):
        #in string id, in permission perm, in int flags = None
        raise NotImplemented, "permissions_allow() method is not implemented in this object"

    def permissions_deny(self, id, perm, flags = Flags.none):
        #in string id, in permission perm, in int flags = None
        raise NotImplemented, "permissions_deny() method is not implemented in this object"

    
class NSDirectory(NSEntry):
    """
    
    """
    
    def __init__(self, session = None, url = None, flags = Flags.none):
        super(NSDirectory,self).__init__()
    
    #navigation/query methods    
    def change_dir(self, url):
        raise NotImplemented, "change_dir() method is not implemented in this object"
 
    #navigation/query methods
    def list(self, name_pattern = ".", flags = Flags.none):
        #in string name_pattern = ".", in int flags = None, out array<saga::url> names
        raise NotImplemented, "list() method is not implemented in this object"
 
    #navigation/query methods
    def find(self, name_pattern, flags = Flags.recursive):
        #in string name_pattern, in  int flags = Recursive, out array<saga::url> names
        raise NotImplemented, "find() method is not implemented in this object"
    
    #navigation/query methods
    def read_link(self, name):
        #in URL name, out URL link
        raise NotImplemented, "read_link() method is not implemented in this object"
 
    #navigation/query methods
    def exists (self,name):
        #in  URL name, out boolean exists
        raise NotImplemented, "exist() method is not implemented in this object"
  
    #navigation/query methods
    def is_dir(self, name):
        #in URL name, out boolean test 
        raise NotImplemented, "is_dir() method is not implemented in this object"
 
    #navigation/query methods
    def is_entry (self, name):
        #in URL name, out boolean test 
        raise NotImplemented, "is_entry() method is not implemented in this object"
 
    #navigation/query methods
    def is_link(self, name):
        #in saga::url name, out boolean test
        raise NotImplemented, "is_link() method is not implemented in this object"
 
    # manage entries by number
    def get_num_entries (self):
        #out int num
        raise NotImplemented, "get_num_entries() method is not implemented in this object"
    
    # manage entries by number
    def get_entry(self, entry):
        #in int entry, out URL name 
        raise NotImplemented, "get_entry() method is not implemented in this object"
 
    # management methods + management methods - wildcard versions
    def copy(self, source, target, flags = Flags.none):
        #in URL source,   in URL target, in int flags = None
        #in string source,in URL target, in int flags = None
        raise NotImplemented, "copy() method is not implemented in this object"
 
    # management methods + management methods - wildcard versions
    def link(self, source, target, flags = Flags.none):
        #in URL source,    in URL target, in int flags = None
        #in string source, in URL target, in int flags = None
        raise NotImplemented, "link() method is not implemented in this object"
  
   # management methods + management methods - wildcard versions
    def move (self, source, target, flags = Flags.none):
       #in URL source,    in URL target, in int flags = None
       #in string source, in URL target, in int flags = None
       raise NotImplemented, "move() method is not implemented in this object"
  
   # management methods + management methods - wildcard versions
    def remove(self, target, flags =  Flags.none):
       #in URL    target, in int flags = None
       #in string target, in int flags = None
       raise NotImplemented, "remove() method is not implemented in this object"
  
   # management methods
    def make_dir(self, target, flags = Flags.none):
      #in saga::url target, in int flags =  None
       raise NotImplemented, "make_dir() method is not implemented in this object"
  
    #factory methods
    def open():
        #in  saga::url name, in  int flags = None, out ns_entry entry
        raise NotImplemented, "open() method is not implemented in this object"
 
    #factory methods
    def open_dir():
        #in  saga::url name, in  int flags = None, out ns_directory dir
        raise NotImplemented, "open_dir() method is not implemented in this object"
 
    #permissions with flags + permissions with flags - wildcard version
    def permissions_allow(self, target, id, perm, flags = Flags.none):
        #in URL target,    in string id, in int perm, in int flags = None
        #in string target, in string id, in int perm, in int flags = None
        raise NotImplemented, "permissions_allow() method is not implemented in this object"
 
    #permissions with flags + permissions with flags - wildcard version
    def permissions_deny(self, target, id, perm, flags = Flags.none):
        #in URL target,    in string id, in int perm, in int flags = None
        #in string target, in string id, in int perm, in int flags = None
        raise NotImplemented, "permissions_deny() method is not implemented in this object"
 