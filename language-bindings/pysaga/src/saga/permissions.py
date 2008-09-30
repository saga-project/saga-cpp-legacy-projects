# Package: saga
# Module: permissions 
# Description: The module which specifies the classes concerning the permissions used in saga
# Specification and documentation can be found in section 3.7, page 87-100 of the GFD-R-P.90 document
# Author: P.F.A. van Zoolingen, Computer Systems Section, Faculty of Exact Science (FEW), Vrije Universiteit, Amsterdam, The Netherlands.

from error import NotImplemented
from task import Async, TaskType


class Permission(object):
    """
    Permission specifies the available permissions in SAGA
    """
    
    NONE = 0
    """No permission whatsoever. """
    
    QUERY = 1
    """ This permission identifies the ability to access all meta data of an entity, and thus to obtain any information about an entity.  """
    
    READ = 2
    """This permission identifies the ability to access the contents and the output of an entity"""

    WRITE = 4
    """This permission identifies the ability to manipulate the contents of an entity."""
    
    EXEC = 8    
    """This permission identifies the ability to perform an action on an entity. """
    
    OWNER = 16
    """This permission identifies the ability to change permissions and ownership of an entity."""
    
    ALL = 31
    """All the permissions. """

class Permissions(Async):
    """
    Permissions provides a generic interface for applications to allow or deny
    specific operations on SAGA objects or grid entities, such as files, streams, or
    monitorables, and to query and set such permissions
    """   

    def permissions_allow(self, id, perm, type=TaskType.NORMAL):
        """
        Enable permission flags.
        @summary: Enable permission flags.
        @param id: id to set permission for
        @type id: string
        @param perm: permissions to enable
        @type perm: int
        @param type: return the normal return values or a Task object in a final, RUNNING or NEW state. By default, type is L{TaskType.NORMAL}
        @type type: int
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
        @note: if the given id is unknown or not supported, a 'BadParameter' exception is raised.

        """
 
    def permissions_deny(self, id, perm, type=TaskType.NORMAL):
        """
        Disable permission flags.
        @summary: Disable permission flags.
        @param id: id to set permissions for
        @type id: string
        @param perm: permissions to disable
        @param type: return the normal return values or a Task object in a final, RUNNING or NEW state. By default, type is L{TaskType.NORMAL}
        @type type: int 
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
        @note: if the given id is unknown or not supported, a 'BadParameter' exception is raised.

        """
    
    def permissions_check(self, id, perm, type=TaskType.NORMAL):
        """
        Check permissions flags.
        @summary: Check permission flags.
        @param id:  id to check permissions for
        @type id: string
        @param perm: permissions to check
        @type perm: int
        @param type: return the normal return values or a Task object in a final, RUNNING or NEW state. By default, type is L{TaskType.NORMAL}
        @type type: int        
        @return: indicates if, for that id, the permissions are granted (True) or not (False).
        @rtype: bool
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
        @note: if the given id is unknown or not supported, a 'BadParameter' exception is raised.

        """
    
    def get_owner(self,type=TaskType.NORMAL):
        """
        Get the owner of the entity.
        @summary: Get the owner of the entity.
        @return: id of the owner
        @rtype: string
        @param type: return the normal return values or a Task object in a final, RUNNING or NEW state. By default, type is L{TaskType.NORMAL}
        @type type: int        
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
    
    def get_group(self, type=TaskType.NORMAL):
        """
        Get the group owning the entity.
        @summary:  Get the group owning the entity.
        @param type: return the normal return values or a Task object in a final, RUNNING or NEW state. By default, type is L{TaskType.NORMAL}
        @type type: int
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
