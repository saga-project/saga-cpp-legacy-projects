class Attributes:
    pass

class Buffer:
    pass

class NSEntry(object):
    def copy(self, url_target, flags=0):
        raise NotImplemented, "copy method is not implemented in this object" 


#class ns_entry : implements saga::object,implements saga::async implements saga::permissions
#CONSTRUCTOR (in session s, in saga::url name, in int flags = None); out ns_entry obj );
#DESTRUCTOR (in ns_entry obj );
#// basic properties
#get_url (out saga::url url );
#get_cwd (out saga::url cwd );
#get_name (out saga::url name );
#// navigation/query methods
#is_dir (out boolean test );
#is_entry (out boolean test );
#is_link (out boolean test );
#read_link (out saga::url link );
#// management methods
       
#link (in saga::url target, in int flags = None);
#move (in saga::url target, in int flags = None);
#remove (in int flags = None);
#close (in float timeout = 0.0);
#// permissions with flags
#permissions_allow (in string id, in permission perm, in int flags = None);
#permissions_deny (in string id, in permission perm, in int flags = None);

class NSDirectory:
    pass

class Async:
    pass

class Permissions:
    pass