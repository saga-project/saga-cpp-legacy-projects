# Package: saga
# Module: permissions 
# Description: The module which specifies the classes concerning the 
#    permissions used in saga
# Specification and documentation can be found in section 3.7, page 87-100 
#    of the GFD-R-P.90 document
# Author: P.F.A. van Zoolingen, Computer Systems Section, Faculty of 
#    Exact Science (FEW), Vrije Universiteit, Amsterdam, The Netherlands.

from saga.task import Async
from saga.error import NotImplemented
from org.ogf.saga.task import TaskMode
from saga.task import TaskType

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

class Permission(object):
    """
    Permission specifies the available permissions in SAGA
    """
    
    NONE = 0
    """
    No permission whatsoever. 
    """
    
    QUERY = 1
    """
    This permission identifies the ability to access all meta data of an entity, 
    and thus to obtain any information about an entity.  
    """
    
    READ = 2
    """
    This permission identifies the ability to access the contents and the 
    output of an entity
    """

    WRITE = 4
    """
    This permission identifies the ability to manipulate the contents of an 
    entity.
    """
    
    EXEC = 8    
    """
    This permission identifies the ability to perform an action on an entity. 
    """
    
    OWNER = 16
    """
    This permission identifies the ability to change permissions and ownership 
    of an entity.
    """
    
    ALL = 31
    """
    All the permissions. 
    """

class Permissions(Async):
    """
    Permissions provides a generic interface for applications to allow or deny
    specific operations on SAGA objects or grid entities, such as files, 
    streams, or monitorables, and to query and set such permissions
    
    @undocumented: __get_group
    @undocumented: __get_owner
    """   

    def permissions_allow(self, id, perm, tasktype=TaskType.NORMAL):
        """
        Enable permission flags.
        @summary: Enable permission flags.
        @param id: id to set permission for
        @type id: string
        @param perm: permissions to enable
        @type perm: int
        @param tasktype: return the normal return values or a Task object in a 
            final, RUNNING or NEW state. By default, tasktype 
            is L{TaskType.NORMAL}
        @type tasktype: int
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
        @note: if the given id is unknown or not supported, a 'BadParameter' 
            exception is raised.
        """
        if type(id) is not str:
            raise BadParameter, "Parameter id is not a string. Type: " + str(type(id))
        if type(perm) is not int:
            raise BadParameter, "Parameter perm is not an int. Type: " + str(type(perm))
        if tasktype is not TaskType.NORMAL and tasktype is not TaskType.SYNC \
        and tasktype is not TaskType.ASYNC  and tasktype is not TaskType.TASK:
            raise BadParameter, "Parameter tasktype is not one of the TaskType values, but " + str(tasktype)
        try:
            if tasktype is TaskType.ASYNC:
                javaObject = self.delegateObject.permissionsAllow(TaskMode.ASYNC, id, perm)
                return Task(delegateObject=javaObject)
            if tasktype is TaskType.SYNC:
                javaObject = self.delegateObject.permissionsAllow(TaskMode.SYNC, id, perm)
                return Task(delegateObject=javaObject)
            if tasktype is TaskType.TASK:
                javaObject = self.delegateObject.permissionsAllow(TaskMode.TASK, id, perm)
                return Task(delegateObject=javaObject)                
            else:
                self.delegateObject.permissionsAllow(id, perm)
        except org.ogf.saga.error.SagaException, e:
            raise self.convertException(e)
            
    def permissions_deny(self, id, perm, tasktype=TaskType.NORMAL):
        """
        Disable permission flags.
        @summary: Disable permission flags.
        @param id: id to set permissions for
        @type id: string
        @param perm: permissions to disable
        @param tasktype: return the normal return values or a Task object in a 
            final, RUNNING or NEW state. By default, tasktype 
            is L{TaskType.NORMAL}
        @type tasktype: int 
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
        @note: if the given id is unknown or not supported, a 'BadParameter' 
            exception is raised.
        """
        if type(id) is not str:
            raise BadParameter, "Parameter id is not a string. Type: " + str(type(id))
        if type(perm) is not int:
            raise BadParameter, "Parameter perm is not an int. Type: " + str(type(perm))
        if tasktype is not TaskType.NORMAL and tasktype is not TaskType.SYNC \
        and tasktype is not TaskType.ASYNC  and tasktype is not TaskType.TASK:
            raise BadParameter, "Parameter tasktype is not one of the TaskType values, but " + str(tasktype)
        try:
            if tasktype is TaskType.ASYNC:
                javaObject = self.delegateObject.permissionsDeny(TaskMode.ASYNC, id, perm)
                return Task(delegateObject=javaObject)
            if tasktype is TaskType.SYNC:
                javaObject = self.delegateObject.permissionsDeny(TaskMode.SYNC, id, perm)
                return Task(delegateObject=javaObject)
            if tasktype is TaskType.TASK:
                javaObject = self.delegateObject.permissionsDeny(TaskMode.TASK, id, perm)
                return Task(delegateObject=javaObject)                
            else:
                self.delegateObject.permissionsDeny(id, perm)
        except org.ogf.saga.error.SagaException, e:
            raise self.convertException(e)

    def permissions_check(self, id, perm, tasktype=TaskType.NORMAL):
        """
        Check permissions flags.
        @summary: Check permission flags.
        @param id:  id to check permissions for
        @type id: string
        @param perm: permissions to check
        @type perm: int
        @param tasktype: return the normal return values or a Task object in a 
            final, RUNNING or NEW state. By default, tasktype 
            is L{TaskType.NORMAL}
        @type tasktype: int        
        @return: indicates if, for that id, the permissions are granted (True) 
            or not (False).
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
        @note: 'True' is only returned when all permissions specified in 'perm' 
            are set for the given id.
        @note: if the given id is unknown or not supported, a 'BadParameter' 
            exception is raised.
        """
        if type(id) is not str:
            raise BadParameter, "Parameter id is not a string. Type: " + str(type(id))
        if type(perm) is not int:
            raise BadParameter, "Parameter perm is not an int. Type: " + str(type(perm))
        if tasktype is not TaskType.NORMAL and tasktype is not TaskType.SYNC \
        and tasktype is not TaskType.ASYNC  and tasktype is not TaskType.TASK:
            raise BadParameter, "Parameter tasktype is not one of the TaskType values, but " + str(tasktype)
        try:
            if tasktype is TaskType.ASYNC:
                javaObject = self.delegateObject.permissionsCheck(TaskMode.ASYNC, id, perm)
                return Task(delegateObject=javaObject)
            if tasktype is TaskType.SYNC:
                javaObject = self.delegateObject.permissionsCheck(TaskMode.SYNC, id, perm)
                return Task(delegateObject=javaObject)
            if tasktype is TaskType.TASK:
                javaObject = self.delegateObject.permissionsCheck(TaskMode.TASK, id, perm)
                return Task(delegateObject=javaObject)                
            else:
                retval = self.delegateObject.permissionsCheck(id, perm)
                if retval == 1:
                    return True
                else:
                    return False
        except org.ogf.saga.error.SagaException, e:
            raise self.convertException(e)       
    
    def get_owner(self,tasktype=TaskType.NORMAL):
        """
        Get the owner of the entity.
        @summary: Get the owner of the entity.
        @return: id of the owner
        @rtype: string
        @param tasktype: return the normal return values or a Task object in a 
            final, RUNNING or NEW state. By default, tasktype 
            is L{TaskType.NORMAL}
        @type tasktype: int        
        @permission: Query
        @raise NotImplemented:
        @raise PermissionDenied:
        @raise AuthorizationFailed:
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @Note: returns the id of the owner of the entity
        @note: an entity which extends Permissions, always has exactly one 
            owner: this method does not return an empty string, '*' (all), or 
            a group id.
        """
        if tasktype is not TaskType.NORMAL and tasktype is not TaskType.SYNC \
        and tasktype is not TaskType.ASYNC  and tasktype is not TaskType.TASK:
            raise BadParameter, "Parameter tasktype is not one of the TaskType values, but " + str(tasktype)
        try:
            if tasktype is TaskType.ASYNC:
                javaObject = self.delegateObject.getOwner(TaskMode.ASYNC)
                return Task(delegateObject=javaObject)
            if tasktype is TaskType.SYNC:
                javaObject = self.delegateObject.getOwner(TaskMode.SYNC)
                return Task(delegateObject=javaObject)
            if tasktype is TaskType.TASK:
                javaObject = self.delegateObject.getOwner(TaskMode.TASK)
                return Task(delegateObject=javaObject)                
            else:
                return self.delegateObject.getOwner()
        except org.ogf.saga.error.SagaException, e:
            raise self.convertException(e)     
    
    def get_group(self, tasktype=TaskType.NORMAL):
        """
        Get the group owning the entity.
        @summary:  Get the group owning the entity.
        @param tasktype: return the normal return values or a Task object in a 
            final, RUNNING or NEW state. By default, tasktype 
            is L{TaskType.NORMAL}
        @type tasktype: int
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
        @note: if the implementation does not support groups, the method 
            returns an empty string.
        """
        if tasktype is not TaskType.NORMAL and tasktype is not TaskType.SYNC \
        and tasktype is not TaskType.ASYNC  and tasktype is not TaskType.TASK:
            raise BadParameter, "Parameter tasktype is not one of the TaskType values, but " + str(tasktype)
        try:
            if tasktype is TaskType.ASYNC:
                javaObject = self.delegateObject.getGroup(TaskMode.ASYNC)
                return Task(delegateObject=javaObject)
            if tasktype is TaskType.SYNC:
                javaObject = self.delegateObject.getGroup(TaskMode.SYNC)
                return Task(delegateObject=javaObject)
            if tasktype is TaskType.TASK:
                javaObject = self.delegateObject.getGroup(TaskMode.TASK)
                return Task(delegateObject=javaObject)                
            else:
                return self.delegateObject.getGroup()
        except org.ogf.saga.error.SagaException, e:
            raise self.convertException(e)     

    def __get_group(self):
        return self.get_group()

    group = property(__get_group,
            doc="""The id of the group owning the entity.\n@type: string""")

    def __get_owner(self):
        return self.get_owner()

    owner = property(__get_owner,
            doc="""The id of the owner of the entity.\n@type: string""")
        