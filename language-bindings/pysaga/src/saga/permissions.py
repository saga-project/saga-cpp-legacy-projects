#package saga.permissions
from error import NotImplemented
from task import Async


class Permission(object):
    """
    Permission specifies the available permissions in SAGA
    """
    NONE      = 0
    QUERY     = 1
    READ      = 2
    WRITE     = 4
    EXEC      = 8    
    OWNER     = 16
    ALL       = 31

class Permissions(Async):
    """
    Permissions provides a generic interface for applications to allow or deny
    specific operations on SAGA objects or grid entities, such as files, streams, or
    monitorables, and to query and set such permissions
    """   

    def permissions_allow(self, id, perm):
        #in string id, in int perm
        """
        Enable permission flags
        @summary: enable permission flags
        @param id: id to set permission for
        @type id: string
        @param perm: permissions to enable
        @type perm: int
        @PostCondition: the permissions are enabled.
        @Permission: Owner
        @raise NotImplemented:
        @raise BadParameter:
        @raise PermissionDenied:
        @raise AuthorizationFailed:
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @Note: an id '*' sets the permissions for all (world)
        @note: the 'Owner' permission can not be set to the id '*' (all).
        @note: if the given id is unknown or not supported, a 'BadParameter' exception is thrown.

        """
        #note: whether an id is interpreted as a group id is up to the implementation. An implementation MUST specify how user and group id's are formed.

        raise NotImplemented, "permissions_allow() is not implemented in this object"
    
    
        
    def permissions_deny(self, id, perm):
        #in string id, in int perm
        """
        Disable permission flags
        @summary: disable permission flags
        @param id: id to set permissions for
        @type id: string
        @param perm: permissions to disable
        @PostCondition: the permissions are disabled.
        @permission: Owner
        @raise NotImplemented:
        @raise BadParameter:
        @raise PermissionDenied:
        @raise AuthorizationFailed:
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @Note: an id '*' sets the permissions for all (world)
        @note: the 'Owner' permission can not be set to the id '*' (all).
        @note: if the given id is unknown or not supported, a 'BadParameter' exception is thrown.

        """
        #note: whether an id is interpreted as a group id is up to the implementation. An implementation MUST specify how user and group id's are formed.
 
        raise NotImplemented, "permissions_deny() is not implemented in this object"
    
    
    def permissions_check(self, id, perm):
        #in string id, in int perm, out bool value
        """
        Check permissions flags
        @summary: check permission flags
        @param id:  id to check permissions for
        @type id: string
        @param perm: permissions to check
        @type perm: int
        @return: indicates if, for that id, the permissions are granted (True) or not (False).
        @rtype: boolean
        @permission:    Query
        @raise NotImplemented:
        @raise BadParameter:
        @raise PermissionDenied:
        @raise AuthorizationFailed:
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @Note: an id '*' gets the permissions for all (world)
        @note: 'True' is only returned when all permissions specified in 'perm' are set for the given id.
        @note: if the given id is unknown or not supported, a 'BadParameter' exception is thrown.

        """

        raise NotImplemented, "permissions_check() is not implemented in this object"
    
    
    def get_owner(self):
        #out string owner
        """
        Get the owner of the entity
        @summary: get the owner of the entity
        @return: id of the owner
        @rtype: string
        @permission: Query
        @raise NotImplemented:
        @raise PermissionDenied:
        @raise AuthorizationFailed:
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @Note: returns the id of the owner of the entity
        @note: an entity which extends Permissions, always has exactly one owner: this
             method does not return an empty string, '*' (all), or a group id.

        """

        raise NotImplemented, "get_owner() is not implemented in this object"
    
    def get_group(self):
        #out string group
        """
        Get the group owning the entity
        @summary:  get the group owning the entity
        @return: id of the group
        @rtype: string
        @permission: Query
        @raise NotImplemented:
        @raise PermissionDenied:
        @raise AuthorizationFailed:
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @note: returns the id of the group owning the entity
        @note: if the implementation does not support groups, the method returns an empty string.
        """
        #Notes: this method MUST NOT return '*' (all), and MUST NOT return a user id.

        raise NotImplemented, "get_group() is not implemented in this object"
    