#package saga.permissions
from error import NotImplemented
from task import Async


class Permission(object):
    none      = 0
    query     = 1
    read      = 2
    write     = 4
    execute   = 8    # exec is a keyword, so not usable :(
    owner     = 16
    all       = 31

class Permissions(Async):
    """ """   

    def permissions_allow(self, id, perm):
        #in string id, in int perm
        raise NotImplemented, "permissions_allow() is not implemented in this object"
    
        
    def permissions_deny(self, id, perm):
        #in string id, in int perm
        raise NotImplemented, "permissions_deny() is not implemented in this object"
    
    def permissions_check(self, id, perm):
        #in string id, in int perm, out bool value
        raise NotImplemented, "permissions_check() is not implemented in this object"
    
    def get_owner(self):
        #out string owner
        raise NotImplemented, "get_owner() is not implemented in this object"
    
    def get_group(self):
        #out string group
        raise NotImplemented, "get_group() is not implemented in this object"
    