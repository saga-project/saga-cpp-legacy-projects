# Page 195 package saga.namespace
from saga.object import Object, ObjectType
from saga.permissions import Permissions
from saga.task import Async, TaskType
from saga.error import NotImplemented
from org.ogf.saga.namespace import NSFactory
from org.ogf.saga.task import TaskMode

class Flags(object):
    """ 
    The Flags describe the properties of several operations on namespace entries.
    Packages which inherit from the saga.namespace package use the same flag semantics
    unless specified otherwise, but will, in general, add additional flags to some operations
    """
    NONE            =  0
    """ 
    @summary: indicates the absence of flags, and thus also implies that the default flags
        for an operation do not apply, either
    """
    
    OVERWRITE       =  1
    """
    @summary: enforces an operation which creates a new namespace entry to continue
        even if the target entry does already exist. If that flag is not given, an
        'AlreadyExists' exception would result from such an operation
    """
    
    RECURSIVE       =  2
    """
    @summary: enforces an operation to apply recursively on a directory tree. If that flag
        is not given, the same operation would only apply to the given directory,
        and not to its children
    """
    
    DEREFERENCE     =  4
    """
    @summary: enforces an operation to apply not to the entry pointed to by the target
        name, but to the link target of that entry. If that flag is not given,
        the same operation would apply to the entry directly, and its link target
        stays unaffected
    """
    
    CREATE          =  8
    """
    @summary: allows a namespace entry to be created while opening it, if it does not already
        exist. If that flag is not given, the same open operation would cause
        a 'DoesNotExist' exception. If the entry exists, the flag is ignored.
    """
    
    EXCLUSIVE       = 16
    """
    @summary: implies a modification to the meaning of the 'CREATE' flag: if the entry
        already exists, the 'CREATE' flag is is no longer silently ignored, but causes
        an 'AlreadyExists' exception.
    """
    
    LOCK            = 32
    """
    @summary: enforces a lock on the name space entry when it is opened. Locks are
        advisory in SAGA, semantic details for locking are defined in the de-
        scription of the open() call
    """
    
    CREATEPARENTS   = 64
    """
    @summary: An operation which would create a name space entry would normally
        fail if any path element in the targets name does not yet exist. If this
        flag is given, such an operation would not fail, but would imply that the
        missing path elements are created on the fly.
    """
    

class NSEntry(Object, Permissions, Async): 
    """
    NSEntry defines methods which serve the inspection of the entry itself, methods
    which allows to manage the entry (e.g. to copy, move, or remove it), and
    methods to manipulate the entry's access control lists.
    """
    delegateObject = None

    def __init__(self, name, session="default", flags=Flags.NONE, **impl):
        """
        Initialize the the object
        @summary: initialize the the object
        @param session: session handle
        @param name: initial working dir
        @param flags: open mode
        @type session: L{Session}
        @type name: L{URL}
        @type flags: int
        @postcondition: the entry is opened.
        @postcondition: 'Owner' of target is the id of the context use to perform the opereration, if the entry gets created.
        @permission: Exec for parent directory.
        @permission: Write for parent directory if Create is set.
        @permission: Write for name if Write is set.
        @permission: Read for name if Read is set.
        @raise NotImplemented:
        @raise IncorrectURL:
        @raise BadParameter:
        @raise DoesNotExist:
        @raise AlreadyExists:
        @raise PermissionDenied:
        @raise AuthorizationFailed:
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @note:  the default flags are 'NONE' (0)
        @note:  the constructor performs an open of the
              entry - all notes to the respective open
              call (on namespace_directory) apply.
        """
        if "delegateObject" in impl:
            if impl["delegateObject"].__class__ is not org.ogf.saga.namespace.NSEntry:
                raise BadParameter, "Parameter impl[\"delegateObject\"] is not a org.ogf.saga.namespace.NSEntry. Type: " + str(impl["delegateObject"].__class__)
            self.delegateObject = impl["delegateObject"]
        else:
            if type(session) is not Session and session is not "default":
                raise BadParameter, "Parameter session is not a Session. Type: " + str(type(session))
            if type(name) is not URL:
                raise BadParameter, "Parameter name is not a URL. Type: " + str(type(name))
            if type(flags) is not int:
                raise BadParameter, "Parameter flags is not an int. Type: " + str(type(flags))
            try:
                if flags is Flags.NONE and session is not "default":
                    self.delegateObject = NSFactory.createNSEntry(session.delegateObject, name.delegateObject)
                elif flags is not Flags.NONE and session is not "default":
                    self.delegateObject = NSFactory.createNSEntry(session.delegateObject, name.delegateObject, flags)
                elif flags is Flags.NONE and session is "default":
                    self.delegateObject = NSFactory.createNSEntry(name.delegateObject)
                else:
                    self.delegateObject = NSFactory.createNSEntry(name.delegateObject, flags)
            except org.ogf.saga.error.SagaException, e:
                raise self.convertException(e)

#TODO: cannot be created asynchronous object yet

    def __del__(self):
        """
        Destroy the object
        @summary:destroy the object
        @postcondition: the entry is closed.
        @note:  if the instance was not closed before, the destructor performs a close()
            on the instance, and all notes to close() apply.
        """
        self.close()
        Object.__del__(self)
        Permissions.__del__(self)
        Async.__del__(self)
        
    def get_url(self, tasktype=TaskType.NORMAL):
        #out URL url 
        """
        Obtain the complete url pointing to the entry
        @summary: obtain the complete url pointing to the entry
        @rtype: L{URL} 
        @return: url pointing to the entry
        @raise NotImplemented:
        @raise IncorrectState:
        @raise Timeout:
        @raise NoSuccess:
        """        
        if tasktype is not TaskType.NORMAL and tasktype is not TypeTask.SYNC \
        and tasktype is not TaskType.ASYNC  and tasktype is not TypeTask.TASK:
            raise BadParameter, "Parameter tasktype is not one of the TypeTask values, but " + str(tasktype)
        try:
            if tasktype is TaskType.ASYNC:
                javaObject = self.delegateObject.getURL(TaskMode.ASYNC)
                return Task(delegateObject=javaObject)
            if tasktype is TaskType.SYNC:
                javaObject = self.delegateObject.getURL(TaskMode.SYNC)
                return Task(delegateObject=javaObject)
            if tasktype is TaskType.TASK:
                javaObject = self.delegateObject.getURL(TaskMode.TASK)
                return Task(delegateObject=javaObject)        
            else:
                retval = self.delegateObject.getURL()
                return URL (delegateObject = retval)
        except org.ogf.saga.error.SagaException, e:
            raise self.convertException(e)



    def get_cwd(self, tasktype=TaskType.NORMAL):
        #out URL cwd
        """
        Obtain the current working directory for the entry
        @summary: obtain the current working directory for the entry
        @rtype: L{URL}
        @return: current working directory
        @raise NotImplemented:
        @raise IncorrectState:
        @raise Timeout:
        @raise NoSuccess:
        @note: returns the directory part of the url path element.

        """        
        if tasktype is not TaskType.NORMAL and tasktype is not TypeTask.SYNC \
        and tasktype is not TaskType.ASYNC  and tasktype is not TypeTask.TASK:
            raise BadParameter, "Parameter tasktype is not one of the TypeTask values, but " + str(tasktype)
        try:
            if tasktype is TaskType.ASYNC:
                javaObject = self.delegateObject.getCWD(TaskMode.ASYNC)
                return Task(delegateObject=javaObject)
            if tasktype is TaskType.SYNC:
                javaObject = self.delegateObject.getCWD(TaskMode.SYNC)
                return Task(delegateObject=javaObject)
            if tasktype is TaskType.TASK:
                javaObject = self.delegateObject.getCWD(TaskMode.TASK)
                return Task(delegateObject=javaObject)        
            else:
                retval = self.delegateObject.getCWD()
                return URL (delegateObject = retval)
        except org.ogf.saga.error.SagaException, e:
            raise self.convertException(e)

    def get_name(self, tasktype=TaskType.NORMAL): 
        #out URL name
        """
        Obtain the name part of the url path element
        @summary: obtain the name part of the url path element
        @rtype: L{URL}
        @return: last part of path element
        @raise NotImplemented:
        @raise IncorrectState:
        @raise Timeout:
        @raise NoSuccess:
         """
        if tasktype is not TaskType.NORMAL and tasktype is not TypeTask.SYNC \
        and tasktype is not TaskType.ASYNC  and tasktype is not TypeTask.TASK:
            raise BadParameter, "Parameter tasktype is not one of the TypeTask values, but " + str(tasktype)
        try:
            if tasktype is TaskType.ASYNC:
                javaObject = self.delegateObject.getName(TaskMode.ASYNC)
                return Task(delegateObject=javaObject)
            if tasktype is TaskType.SYNC:
                javaObject = self.delegateObject.getName(TaskMode.SYNC)
                return Task(delegateObject=javaObject)
            if tasktype is TaskType.TASK:
                javaObject = self.delegateObject.getName(TaskMode.TASK)
                return Task(delegateObject=javaObject)   
            else:
                retval = self.delegateObject.getName()
                return URL (delegateObject = retval)
        except org.ogf.saga.error.SagaException, e:
            raise self.convertException(e)
        
    def is_dir_self(self, tasktype=TaskType.NORMAL):
        #out boolean test
        """
        Tests the entry for being a directory
        @summary: tests the entry for being a directory
        @rtype: bool
        @return: indicatator if entry is a directory
        @permission: Query
        @permission: Query for parent directory.
        @raise NotImplemented:
        @raise IncorrectState:
        @raise PermissionDenied:
        @raise AuthorizationFailed:
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @note: returns True if entry is a directory, False otherwise
        @note:  similar to 'test -d' as defined by POSIX.
        """        
        if tasktype is not TaskType.NORMAL and tasktype is not TypeTask.SYNC \
        and tasktype is not TaskType.ASYNC  and tasktype is not TypeTask.TASK:
            raise BadParameter, "Parameter tasktype is not one of the TypeTask values, but " + str(tasktype)
        try:
            if tasktype is TaskType.ASYNC:
                javaObject = self.delegateObject.isDir(TaskMode.ASYNC)
                return Task(delegateObject=javaObject)
            if tasktype is TaskType.SYNC:
                javaObject = self.delegateObject.isDir(TaskMode.SYNC)
                return Task(delegateObject=javaObject)
            if tasktype is TaskType.TASK:
                javaObject = self.delegateObject.isDir(TaskMode.TASK)
                return Task(delegateObject=javaObject)   
            else:       
                retval = self.delegateObject.isDir()
                if retval == 1:
                    return True
                else: 
                    return False
        except org.ogf.saga.error.SagaException, e:
            raise self.convertException(e)

    def is_entry_self(self, tasktype=TaskType.NORMAL):
        #out boolean test
        """
        Tests the entry for being an NSEntry
        @summary: tests the entry for being an NSEntry
        @rtype: bool
        @return: indicator if entry is an NSEntry
        @permission: Query
        @permission: Query for parent directory.
        @raise NotImplemented:
        @raise IncorrectState:
        @raise PermissionDenied:
        @raise AuthorizationFailed:
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @note:  the method returns False if the entry is a
            link or a directory (although an NSDirectory is a NSEntry, False is returned on a test on
            an NSDirectory), otherwise True is returned.
        @note:  similar to 'test -f' as defined by POSIX.

        """        
        if tasktype is not TaskType.NORMAL and tasktype is not TypeTask.SYNC \
        and tasktype is not TaskType.ASYNC  and tasktype is not TypeTask.TASK:
            raise BadParameter, "Parameter tasktype is not one of the TypeTask values, but " + str(tasktype)
        try:
            if tasktype is TaskType.ASYNC:
                javaObject = self.delegateObject.isEntry(TaskMode.ASYNC)
                return Task(delegateObject=javaObject)
            if tasktype is TaskType.SYNC:
                javaObject = self.delegateObject.isEntry(TaskMode.SYNC)
                return Task(delegateObject=javaObject)
            if tasktype is TaskType.TASK:
                javaObject = self.delegateObject.isEntry(TaskMode.TASK)
                return Task(delegateObject=javaObject)   
            else:  
                retval = self.delegateObject.isEntry()
                if retval == 1:
                    return True
                else: 
                    return False
        except org.ogf.saga.error.SagaException, e:
            raise self.convertException(e)

    def is_link_self(self, tasktype=TaskType.NORMAL):
        #out boolean test
        """
        Tests the entry for being a link
        @summary: tests the entry for being a link
        @rtype: bool
        @return: indicator if entry is a link
        @permission: Query
        @permission:  Query for parent directory.
        @raise NotImplemented:
        @raise IncorrectState:
        @raise PermissionDenied:
        @raise AuthorizationFailed:
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @note: returns True if the entry is a link, False otherwise
        @note: similar to 'test -L' as defined by POSIX.

        """        
        if tasktype is not TaskType.NORMAL and tasktype is not TypeTask.SYNC \
        and tasktype is not TaskType.ASYNC  and tasktype is not TypeTask.TASK:
            raise BadParameter, "Parameter tasktype is not one of the TypeTask values, but " + str(tasktype)
        try:
            if tasktype is TaskType.ASYNC:
                javaObject = self.delegateObject.isLink(TaskMode.ASYNC)
                return Task(delegateObject=javaObject)
            if tasktype is TaskType.SYNC:
                javaObject = self.delegateObject.isLink(TaskMode.SYNC)
                return Task(delegateObject=javaObject)
            if tasktype is TaskType.TASK:
                javaObject = self.delegateObject.isLink(TaskMode.TASK)
                return Task(delegateObject=javaObject)   
            else:
                retval = self.delegateObject.isLink()
                if retval == 1:
                    return True
                else: 
                    return False
        except org.ogf.saga.error.SagaException, e:
            raise self.convertException(e)
        
    def read_link_self(self, tasktype=TaskType.NORMAL):
        #out URL link
        """
        Get the name of the link target
        @summary: get the name of the link target
        @rtype: L{URL}
        @return: the resolved name
        @permission: Query
        @permission Query for parent directory.
        @raise NotImplemented:
        @raise IncorrectState:
        @raise PermissionDenied:
        @raise AuthorizationFailed:
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @note: the returned name is sufficient to access the link target entry
        @note: resolves one link level only
        @note: if the entry instance this method is called upon does not point to a link, an
            'IncorrectState' exception is raised.
        @note:  similar to 'ls -L' as defined by POSIX.
        """        
        if tasktype is not TaskType.NORMAL and tasktype is not TypeTask.SYNC \
        and tasktype is not TaskType.ASYNC  and tasktype is not TypeTask.TASK:
            raise BadParameter, "Parameter tasktype is not one of the TypeTask values, but " + str(tasktype)
        try:
            if tasktype is TaskType.ASYNC:
                javaObject = self.delegateObject.readLink(TaskMode.ASYNC)
                return Task(delegateObject=javaObject)
            if tasktype is TaskType.SYNC:
                javaObject = self.delegateObject.readLink(TaskMode.SYNC)
                return Task(delegateObject=javaObject)
            if tasktype is TaskType.TASK:
                javaObject = self.delegateObject.readLink(TaskMode.TASK)
                return Task(delegateObject=javaObject)   
            else:
                retval = self.delegateObject.readLink()
                return URL (delegateObject = retval)
        except org.ogf.saga.error.SagaException, e:
            raise self.convertException(e)
        
    def copy_self(self, target, flags = Flags.NONE, tasktype=TaskType.NORMAL):
        """
        Copy the entry to another part of the name space
        @summary: copy the entry to another part of the name space
        @param target: name to copy to
        @param flags: flags defining the operation modus
        @type target: L{URL}
        @type flags: int 
        @postcondition: an identical copy exists at target.
        @postcondition: 'Owner' of target is the id of the context use to perform the opereration, if target gets created.
        @permission: Query
        @permission: Exec for parent directory.
        @permission: Query for target.
        @permission: Query for target's parent directory.
        @permission: Exec for target's parent directory.
        @permission: Write for target if target does exist.
        @permission: Write for target's parent directory if target does not exist.
        @raise NotImplemented:
        @raise IncorrectURL:
        @raise BadParameter:
        @raise DoesNotExist:
        @raise AlreadyExists:
        @raise IncorrectState:
        @raise PermissionDenied:
        @raise AuthorizationFailed:
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @note: if the target is a directory, the source entry is copied into that directory
        @note: a 'BadParameter' exception is raised if the source is a directory and the 'RECURSIVE' flag is not set.
        @note: a 'BadParameter' exception is raised if the source is not a directory and the 'RECURSIVE' flag is set.
        @note:  if the target lies in a non-existing part of  the name space, a 'DoesNotExist' exception is
            raised, unless the 'CREATEPARENTS' flag is given. Then that part of the name space must be created.
        @note:  if the target already exists, it will be overwritten if the 'OVERWRITE' flag is set,
            otherwise it is an 'AlreadyExists' exception.
        @note: if a directory is to be copied recursively, but the target exists and is not a directory,
             and not a link to a directory, an 'AlreadyExists' exception is raised even if the 'OVERWRITE' flag is set.
        @note: if the instance points at an symbolic link, the source is deeply dereferenced before copy.
             If derefencing is impossible (e.g. on a broken link), an 'IncorrectState' exception is raised.
        @note: other flags are not allowed, and cause a 'BadParameter' exception.
        @note:  the default flags are 'NONE' (0).
        @note:  similar to 'cp' as defined by POSIX.
        """        
        if type(target) is not URL:
            raise BadParameter, "Parameter target is not a URL. Type: " + str(type(target))
        if type(flags) is not int:
            raise BadParameter, "Parameter flags is not an int. Type: " + str(type(flags)) 
        if tasktype is not TaskType.NORMAL and tasktype is not TypeTask.SYNC \
        and tasktype is not TaskType.ASYNC  and tasktype is not TypeTask.TASK:
            raise BadParameter, "Parameter tasktype is not one of the TypeTask values, but " + str(tasktype)
        try:
            javaObject = None
            if tasktype is TaskType.ASYNC:
                javaObject = self.delegateObject.copy(TaskMode.ASYNC, target.delegateObject, flags)
                return Task(delegateObject=javaObject)
            if tasktype is TaskType.SYNC:
                javaObject = self.delegateObject.copy(TaskMode.SYNC, target.delegateObject, flags)
                return Task(delegateObject=javaObject)
            if tasktype is TaskType.TASK:
                javaObject = self.delegateObject.copy(TaskMode.TASK, target.delegateObject, flags)
                return Task(delegateObject=javaObject)
            else:
                self.delegateObject.copy(target.delegateObject, flags)
        except org.ogf.saga.error.SagaException, e:
            raise self.convertException(e)
   
    def link_self(self, target, flags = Flags.NONE, tasktype=TaskType.NORMAL):
        """
        Create a symbolic link from the target entry to the source entry ( this entry) so that any reference
        to the target refers to the source entry
        @summary: create a symbolic link
        @param target: name to link to
        @param flags: flags defining the operation modus
        @type target: L{URL}
        @type flags: int
        @postcondition: - a symbolic link to the entry exists at target.
        @postcondition: 'Owner' of target is the id of the context use to perform the opereration if target gets
                  created.
        @permission: Query
        @permission: Exec for parent directory.
        @permission: Query for target.
        @permission: Query for target's parent directory.
        @permission: Exec for target's parent directory.
        @permission: Write for target if target does exist.
        @permission: Write for target's parent directory if target does not exist.
        @raise NotImplemented:
        @raise IncorrectURL:
        @raise BadParameter:
        @raise DoesNotExist:
        @raise AlreadyExists:
        @raise IncorrectState:
        @raise PermissionDenied:
        @raise AuthorizationFailed:
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @note: if the target is a directory, the source entry is linked into that directory
        @note: if the source is a directory, and the 'RECURSIVE' flag is set, the source directory
              is recursively linked to the target (which must  be a directory as well - otherwise a
              'BadParameter' exception is raised). The method then behaves similar to lndir. If the
              'RECURSIVE' flag is not set, the source entry itself is linked.
        @note: a 'BadParameter' exception is raised if the source is not a directory and the 'RECURSIVE' flag is set.
        @note: if the target lies in a non-existing part of the name space, a 'DoesNotExist' exception is
              raised, unless the 'CREATEPARENTS' flag is given - then that part of the name space must be created.
        @note: if the target already exists, it will be overwritten if the 'OVERWRITE' flag is set,
              otherwise it is an 'AlreadyExists' exception.
        @note: if a directory is to be moved, but the target exists and is not a directory, and not a link
              to a directory, an 'AlreadyExists' exception is raised even if the 'OVERWRITE' flag is set.
        @note: if the instance points at an symbolic link, the source is not dereferenced before linking,
              unless the 'DEREFERENCE' flag is given. If derefencing is impossible (e.g. on a broken
              link), an 'IncorrectState' exception is raised.
        @note: other flags are not allowed, and cause a 'BadParameter' exception.
        @note: the default flags are 'NONE' (0).
        @note: similar to 'ln' as defined by POSIX.
     
        """        
        if type(target) is not URL:
            raise BadParameter, "Parameter target is not a URL. Type: " + str(type(target))
        if type(flags) is not int:
            raise BadParameter, "Parameter flags is not an int. Type: " + str(type(flags)) 
        if tasktype is not TaskType.NORMAL and tasktype is not TypeTask.SYNC \
        and tasktype is not TaskType.ASYNC  and tasktype is not TypeTask.TASK:
            raise BadParameter, "Parameter tasktype is not one of the TypeTask values, but " + str(tasktype)
        try:
            if tasktype is TaskType.ASYNC:
                javaObject = self.delegateObject.link(TaskMode.ASYNC, target.delegateObject, flags)
                return Task(delegateObject=javaObject)
            if tasktype is TaskType.SYNC:
                javaObject = self.delegateObject.link(TaskMode.SYNC, target.delegateObject, flags)
                return Task(delegateObject=javaObject)
            if tasktype is TaskType.TASK:
                javaObject = self.delegateObject.link(TaskMode.TASK, target.delegateObject, flags)
                return Task(delegateObject=javaObject)
            else:
                self.delegateObject.link(target.delegateObject, flags)
        except org.ogf.saga.error.SagaException, e:
            raise self.convertException(e)      
        
    def move_self(self, target, flags = Flags.NONE, tasktype=TaskType.NORMAL):
        """
        Rename source to target, or move source to target if target is a directory.
        @summary: rename or move target
        @param target: name to move to
        @param flags: flags defining the operation modus
        @type target: L{URL}
        @type flags: int
        @postcondition: an identical copy exists at target.
        @postcondition: the original entry is removed.
        @postcondition: 'Owner' of target is the id of the context use to perform the opereration if target gets created.
        @permission: Query
        @permission: Write
        @permission: Exec for parent directory.
        @permission: Write for parent directory.
        @permission: Query for target.
        @permission: Exec for target's parent directory.
        @permission: Write for target if target does exist.
        @permission: Write for target's parent directory if target does not exist.
        @raise NotImplemented:
        @raise IncorrectURL:
        @raise BadParameter:
        @raise DoesNotExist:
        @raise AlreadyExists:
        @raise IncorrectState:
        @raise PermissionDenied:
        @raise AuthorizationFailed:
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @note: if the target is a directory, the source entry is moved into that directory
        @note: a 'BadParameter' exception is raised if the source is a directory and the 'RECURSIVE' flag is not set.
        @note: a 'BadParameter' exception is raised if the source is not a directory and the 'RECURSIVE' flag is set.
        @note: if the target lies in a non-existing part of the name space, a 'DoesNotExist' exception is
                  raised, unless the 'CREATEPARENTS' flag is given. Then that part of the name space must be created.
        @note: if the target already exists, it will be overwritten if the 'OVERWRITE' flag is set,
             otherwise it is an 'AlreadyExists' exception.
        @note: if the instance points at an symbolic link, the source is not dereferenced before moving,
             unless the 'DEREFERENCE' flag is given. If derefencing is impossible (e.g. on a broken
             link), an 'IncorrectState' exception is raised.
        @note: other flags are not allowed, and cause a 'BadParameter' exception.
        @note: the default flags are 'NONE' (0).
        @note: similar to 'mv' as defined by POSIX.
        
        """
        if type(target) is not URL:
            raise BadParameter, "Parameter target is not a URL. Type: " + str(type(target))
        if type(flags) is not int:
            raise BadParameter, "Parameter flags is not an int. Type: " + str(type(flags)) 
        if tasktype is not TaskType.NORMAL and tasktype is not TypeTask.SYNC \
        and tasktype is not TaskType.ASYNC  and tasktype is not TypeTask.TASK:
            raise BadParameter, "Parameter tasktype is not one of the TypeTask values, but " + str(tasktype)
        try:
            if tasktype is TaskType.ASYNC:
                javaObject = self.delegateObject.move(TaskMode.ASYNC, target.delegateObject, flags)
                return Task(delegateObject=javaObject)
            if tasktype is TaskType.SYNC:
                javaObject = self.delegateObject.move(TaskMode.SYNC, target.delegateObject, flags)
                return Task(delegateObject=javaObject)
            if tasktype is TaskType.TASK:
                javaObject = self.delegateObject.move(TaskMode.TASK, target.delegateObject, flags)
                return Task(delegateObject=javaObject)
            else:
                self.delegateObject.move(target.delegateObject, flags)
        except org.ogf.saga.error.SagaException, e:
            raise self.convertException(e)      

    def remove_self(self, flags = Flags.NONE, tasktype=TaskType.NORMAL):
        """
        Removes this entry, and closes it
        @summary: removes this entry, and closes it
        @param flags: the operation modus
        @type flags: int
        @postcondition: the original entry is closed and removed.
        @permission: Query
        @permission: Write
        @permission: Exec for parent directory.
        @permission: Write for parent directory.
        @raise NotImplemented:
        @raise BadParameter:
        @raise IncorrectState:
        @raise PermissionDenied:
        @raise AuthorizationFailed:
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @note: a 'BadParameter' exception is raised if the source is a directory and the 'RECURSIVE' flag
                  is not set.
        @note: a 'BadParameter' exception is raised if the source is not a directory and the 'RECURSIVE'
                  flag is set.
        @note: the source will not be dereferenced unless the 'DEREFERENCE' flag is given. If derefencing is
                  impossible (e.g. on a broken link), an
                  'IncorrectState' exception is raised.
        @note:  other flags are not allowed, and cause a 'BadParameter' exception.
        @note:  the default flags are 'NONE' (0).
        @note:  if the instance was not closed before, this call performs a close() on the instance, and
             all notes to close() apply.
        @note:  similar to 'rm' as defined by POSIX.
        
        """   
        if type(flags) is not int:
            raise BadParameter, "Parameter flags is not an int. Type: " + str(type(flags)) 
        if tasktype is not TaskType.NORMAL and tasktype is not TypeTask.SYNC \
        and tasktype is not TaskType.ASYNC  and tasktype is not TypeTask.TASK:
            raise BadParameter, "Parameter tasktype is not one of the TypeTask values, but " + str(tasktype)
        try:
            if tasktype is TaskType.ASYNC:
                javaObject = self.delegateObject.remove(TaskMode.ASYNC, flags)
                return Task(delegateObject=javaObject)
            if tasktype is TaskType.SYNC:
                javaObject = self.delegateObject.remove(TaskMode.SYNC, flags)
                return Task(delegateObject=javaObject)
            if tasktype is TaskType.TASK:
                javaObject = self.delegateObject.remove(TaskMode.TASK, flags)
                return Task(delegateObject=javaObject)
            else:
                self.delegateObject.remove(flags)
        except org.ogf.saga.error.SagaException, e:
            raise self.convertException(e)      
        
    def close(self, timeout = 0.0, tasktype=TaskType.NORMAL):
        """
        Closes the NSEntry
        @summary: closes the NSEntry
        Format:   close              (in float timeout = 0.0);
        @param timeout: seconds to wait
        @type timeout: float 
        @postcondition: the entry instance is closed.
        @raise NotImplemented:
        @raise IncorrectState:
        @raise NoSuccess:
        @note: any subsequent method call on the object raises an 'IncorrectState' exception
            (apart from __del__() and close()).
        @note:  close() can be called multiple times, with no side effects.
        @note:  if close() is implicitely called in __del__, it will never raise an exception.
        @see: Section 2 of the GFD-R-P.90 document for resource deallocation semantics and timeout semantics.
        
        """        
        if type(timeout) is not float or type(timeout) is not int:
            raise BadParameter, "Parameter timout is wrong type. Type: " + str(type(timeout))
        if tasktype is not TaskType.NORMAL and tasktype is not TypeTask.SYNC \
        and tasktype is not TaskType.ASYNC  and tasktype is not TypeTask.TASK:
            raise BadParameter, "Parameter tasktype is not one of the TypeTask values, but " + str(tasktype)

        try:
            if tasktype is TaskType.ASYNC:
                if timeout is 0.0:
                    javaObject = self.delegateObject.close(TaskMode.ASYNC)
                else:
                    javaObject = self.delegateObject.close(TaskMode.ASYNC,timeout)
                return Task(delegateObject=javaObject)
            if tasktype is TaskType.SYNC:
                if timeout is 0.0:
                    javaObject = self.delegateObject.close(TaskMode.SYNC)
                else:
                    javaObject = self.delegateObject.close(TaskMode.SYNC,timeout)
                return Task(delegateObject=javaObject)
            if tasktype is TaskType.TASK:
                if timeout is 0.0:
                    javaObject = self.delegateObject.close(TaskMode.TASK)
                else:
                    javaObject = self.delegateObject.close(TaskMode.TASK,timeout)
                return Task(delegateObject=javaObject)
            else:
                if timeout is 0.0:
                    self.delegateObject.close()
                else:
                    self.delegateObject.close(timeout)
        except org.ogf.saga.error.SagaException, e:
            raise self.convertException(e)             


    def permissions_allow_self(self, id, perm, flags = Flags.NONE, tasktype=TaskType.NORMAL):
        #in string id, in permission perm, in int flags = None
        """
        Enable a permission
        @summary: enable a permission
        @param id: id to set permission for
        @param perm: permission to enable
        @param flags: mode of operation
        @type id : string
        @type perm: value from L{Permission}
        @type flags: int
        @postcondition: the permissions are enabled.
        @permission:  Owner
        @raise NotImplemented:
        @raise BadParameter:
        @raise IncorrectState:
        @raise PermissionDenied:
        @raise AuthorizationFailed:
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @note: all notes to permissions_allow from the saga.permissions.Permission class apply.
        @note:  allowed flags are: 'RECURSIVE', 'DEREFERENCE'. All other flags cause a 'BadParameter'
               exception.
        @note:  specifying 'RECURSIVE' for a non-directory causes a 'BadParameter' exception.

        """        
        if type(id) is not str:
            raise BadParameter, "Parameter id is not a string. Type: " + str(type(id)) 
        if type(perm) is not int:
            raise BadParameter, "Parameter perm is not an int. Type: " + str(type(perm)) 
        if type(flags) is not int:
            raise BadParameter, "Parameter flags is not an int. Type: " + str(type(flags)) 
        if tasktype is not TaskType.NORMAL and tasktype is not TypeTask.SYNC \
        and tasktype is not TaskType.ASYNC  and tasktype is not TypeTask.TASK:
            raise BadParameter, "Parameter tasktype is not one of the TypeTask values, but " + str(tasktype)
        try:
            if tasktype is TaskType.ASYNC:
                javaObject = self.delegateObject.permissionsAllow(TaskMode.ASYNC ,id, perm, flags)
                return Task(delegateObject=javaObject)
            if tasktype is TaskType.SYNC:
                javaObject = self.delegateObject.permissionsAllow(TaskMode.SYNC ,id, perm, flags)
                return Task(delegateObject=javaObject)
            if tasktype is TaskType.TASK: 
                javaObject = self.delegateObject.permissionsAllow(TaskMode.TASK ,id, perm, flags)
                return Task(delegateObject=javaObject)                                           
            else:
                self.delegateObject.permissionsAllow(id, perm, flags)
        except org.ogf.saga.error.SagaException, e:
            raise self.convertException(e)   

    def get_type(self):
        """
        Query the object type.
        @summary: Query the object type.
        @return: type of the object as an int from ObjectType
        @rtype: int
        """
        return ObjectType.NSENTRY

    def permissions_deny_self(self, id, perm, flags = Flags.NONE, tasktype=TaskType.NORMAL):
        #in string id, in permission perm, in int flags = None
        """
        Disable a permission flag
        @summary: disable a permission flag
        @param id: id to set permission for
        @param perm: permission to disable
        @param flags: mode of operation
        @type id : string
        @type perm: value from L{Permission}
        @type flags: int
        @postcondition: the permissions are disabled.
        @permission: Owner
        @raise NotImplemented:
        @raise BadParameter:
        @raise IncorrectState:
        @raise PermissionDenied:
        @raise AuthorizationFailed:
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @note: all notes to permissions_deny from the saga.permissions.Permission class apply.
        @note: allowed flags are: 'RECURSIVE', 'DEREFERENCE'. All other flags cause a 'BadParameter' exception.
        @note: specifying 'RECURSIVE' for a non-directory causes a 'BadParameter' exception.

        """        
        if type(id) is not str:
            raise BadParameter, "Parameter id is not a string. Type: " + str(type(id)) 
        if type(perm) is not int:
            raise BadParameter, "Parameter perm is not an int. Type: " + str(type(perm)) 
        if type(flags) is not int:
            raise BadParameter, "Parameter flags is not an int. Type: " + str(type(flags)) 
        if tasktype is not TaskType.NORMAL and tasktype is not TypeTask.SYNC \
        and tasktype is not TaskType.ASYNC  and tasktype is not TypeTask.TASK:
            raise BadParameter, "Parameter tasktype is not one of the TypeTask values, but " + str(tasktype)
        try:
            if tasktype is TaskType.ASYNC:
                javaObject = self.delegateObject.permissionsDeny(TaskMode.ASYNC ,id, perm, flags)
                return Task(delegateObject=javaObject)
            if tasktype is TaskType.SYNC:
                javaObject = self.delegateObject.permissionsDeny(TaskMode.SYNC ,id, perm, flags)
                return Task(delegateObject=javaObject)
            if tasktype is TaskType.TASK: 
                javaObject = self.delegateObject.permissionsDeny(TaskMode.TASK ,id, perm, flags)
                return Task(delegateObject=javaObject)                                           
            else:
                self.delegateObject.permissionsDeny(id, perm, flags)
        except org.ogf.saga.error.SagaException, e:
            raise self.convertException(e)   
        
    
class NSDirectory(NSEntry, Async):
    """
    NSDirectory inherits all navigation and manipulation methods from NSEntry,
    but adds some more methods to these sets: instead of dir.copy (target)
    they allow, for example, to do dir.copy (source, target). Other methods
    added allow to change the cwd of the instance (which changes the values re-
    turned by the get_name(), get_cwd() and get_url() inspection methods),
    and others allow to open new NSEntry and NSDirectory instances (open()
    and open_dir()).
    """
    
    def __init__(self, name, session="default", flags = Flags.NONE, **impl):
        """
        Initialize the object
        @summary: initialize the object
        @param name: initial working dir
        @param flags: open mode
        @param session: session handle for object creation
        @type name : L{URL}
        @type session: L{Session}
        @type flags: int
        @postcondition: the directory is opened.
        @postcondition: 'Owner' of target is the id of the context use to perform the opereration, if the
                  directory gets created.
        @permission: Exec for parent directory.
        @permission: Write for parent directory if CREATE is set.
        @permission: Write for name if WRITE is set.
        @permission: Read for name if READ is set.
        @raise NotImplemented:
        @raise IncorrectURL:
        @raise BadParameter:
        @raise DoesNotExist:
        @raise PermissionDenied:
        @raise AuthorizationFailed:
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @note:  the semantics of the inherited constructors apply
        @note:  the constructor performs an open of the entry - all notes to the respective open call apply.
        @note:  the default flags are 'NONE' (0).

        """
        if "delegateObject" in impl:
            if impl["delegateObject"].__class__ is not org.ogf.saga.namespace.NSDirectory:
                raise BadParameter, "Parameter impl[\"delegateObject\"] is not a org.ogf.saga.namespace.NSDirectory. Type: " + str(impl["delegateObject"].__class__)
            self.delegateObject = impl["delegateObject"]
        else:
            if type(session) is not Session and session is not "default":
                raise BadParameter, "Parameter session is not a Session. Type: " + str(type(session))
            if type(name) is not URL:
                raise BadParameter, "Parameter name is not a URL. Type: " + str(type(name))
            if type(flags) is not int:
                raise BadParameter, "Parameter flags is not an int. Type: " + str(type(flags))
            try:
                if flags is Flags.NONE and session is not "default":
                    self.delegateObject = NSFactory.createNSDirectory(session.delegateObject, name.delegateObject)
                elif flags is not Flags.NONE and session is not "default":
                    self.delegateObject = NSFactory.createNSDirectory(session.delegateObject, name.delegateObject, flags)
                elif flags is Flags.NONE and session is "default":
                    self.delegateObject = NSFactory.createNSDirectory(name.delegateObject)
                else:
                    self.delegateObject = NSFactory.createNSDirectory(name.delegateObject, flags)
            except org.ogf.saga.error.SagaException, e:
                raise self.convertException(e)

#TODO: async task creation
    
    #navigation/query methods    
    def change_dir(self, url, tasktype=TaskType.NORMAL):
        """
        Change the working directory
        @summary: change the working directory
        @param url : directory to change to
        @type url: L{URL}
        @postcondition: dir is the directory the instance represents.
        @permission: Exec for dir.
        @raise NotImplemented:
        @raise IncorrectURL:
        @raise BadParameter:
        @raise DoesNotExist:
        @raise IncorrectState:
        @raise PermissionDenied:
        @raise AuthorizationFailed:
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @note:  if 'dir' can be parsed as URL, but contains an invalid directory name, a 'BadParameter'
              exception is raised.
        @note:  if 'dir' does not exist, a 'DoesNotExist' exception is raised.
        @note:  similar to the 'cd' command in the POSIX shell.
       
        """
        if type(url) is not URL:
            raise BadParameter, "Parameter url is not a URL. Type: " + str(type(url))
        if tasktype is not TaskType.NORMAL and tasktype is not TypeTask.SYNC \
        and tasktype is not TaskType.ASYNC  and tasktype is not TypeTask.TASK:
            raise BadParameter, "Parameter tasktype is not one of the TypeTask values, but " + str(tasktype)
        try:
            if tasktype is TaskType.ASYNC:
                javaObject = self.delegateObject.changeDir(TaskMode.ASYNC, url.delegateObject)
                return Task(delegateObject=javaObject)
            if tasktype is TaskType.SYNC:
                javaObject = self.delegateObject.changeDir(TaskMode.SYNC, url.delegateObject)
                return Task(delegateObject=javaObject)
            if tasktype is TaskType.TASK:
                javaObject = self.delegateObject.changeDir(TaskMode.TASK, url.delegateObject)
                return Task(delegateObject=javaObject)
            else:
                self.delegateObject.changeDir(url.delegateObject)
        except org.ogf.saga.error.SagaException, e:
            raise self.convertException(e)
        

 
    #navigation/query methods
    def list(self, name_pattern = ".", flags = Flags.NONE, tasktype=TaskType.NORMAL):
        #in string name_pattern = ".", in int flags = None, out array<saga::url> names
        """
        List entries in this directory
        @summary: list entries in this directory
        @param flags: flags defining the operation modus
        @param name_pattern: name or pattern to list
        @type flags: int
        @type name_pattern: string
        @rtype: tuple
        @return: tuple of names matching the name_pattern
        @permission:  Query for entries specified by name_pattern.
        @permission: Exec for parent directories of these entries.
        @permission: Query for parent directories of these entries.
        @permission: Read for directories specified by name_pattern.
        @permission: Exec for directories specified by name_pattern.
        @permission: Exec for parent directories of these directories.
        @permission: Query for parent directories of these directories.
        @raise NotImplemented:
        @raise IncorrectURL:
        @raise BadParameter:
        @raise IncorrectState:
        @raise PermissionDenied:
        @raise AuthorizationFailed:
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @note: if name_pattern is not given (i.e. is an empty string), all entries in the current working directory are listed.
        @note: if name_pattern is given and points to a directory, the contents of that directory are listed.
        @note: the name_pattern follows the standard POSIX  shell wildcard specification, as described above.
        @note:  list does not follow symbolically linked directories, unless the 'DEREFERENCE' flag is specified - otherwise list lists symbolic link entries with a matching name.
        @note:  if the 'DEREFERENCE' flag is set, list returns the name of link targets, not of the link entry itself.
        @note:  the default flags are 'NONE' (0).
        @note:  other flags are not allowed, and cause a 'BadParameter' exception.
        @note:  if the name_pattern cannot be parsed, a 'BadParameter' exception with a descriptive error message is raised.
        @note:  if the name_pattern does not match any entry, an empty list is returned, but no exception is raised.
        @note:  similar to 'ls' as defined by POSIX.
        """  
        if type(name_pattern) is not str:
            raise BadParameter, "Parameter name_pattern is not a string. Type: " + str(type(name_pattern))
        if type(flags) is not int:
            raise BadParameter, "Parameter flags is not an int. Type: " + str(type(int))
        if tasktype is not TaskType.NORMAL and tasktype is not TypeTask.SYNC \
        and tasktype is not TaskType.ASYNC  and tasktype is not TypeTask.TASK:
            raise BadParameter, "Parameter tasktype is not one of the TypeTask values, but " + str(tasktype)
        try:
            if tasktype is TaskType.ASYNC:
                javaObject = self.delegateObject.list(TaskMode.ASYNC, name_pattern, flags)
                return Task(delegateObject=javaObject)
            if tasktype is TaskType.SYNC:
                javaObject = self.delegateObject.list(TaskMode.SYNC, name_pattern, flags)
                return Task(delegateObject=javaObject)
            if tasktype is TaskType.TASK:
                javaObject = self.delegateObject.list(TaskMode.TASK, name_pattern, flags)
                return Task(delegateObject=javaObject)
            else:
                javaArray = self.delegateObject.list(name_pattern, flags)                
                retval = []
                for i in range(len(javaArray)):
                    ret.append(URL(delegateObject = javaArray[i]))
                return retval
        except org.ogf.saga.error.SagaException, e:
            raise self.convertException(e)

    def find(self, name_pattern, flags = Flags.RECURSIVE, tasktype=TaskType.NORMAL):
        #in string name_pattern, in  int flags = Recursive, out array<saga::url> names
        """
        Find entries in the current directory and below
        @summary: find entries in the current directory and below
        @param name_pattern: pattern for names of entries to be found
        @param flags: flags defining the operation modus
        @type name_pattern: string
        @type flags: int
        @rtype: tuple 
        @return: tuple of names matching the name_pattern
        @permission: Read for cwd.
        @permission: Query for entries specified by name_pattern.
        @permission: Exec for parent directories of these entries.
        @permission: Query for parent directories of these entries.
        @permission: Read for directories specified by name_pattern.
        @permission: Exec for directories specified by name_pattern.
        @permission: Exec for parent directories of these directories.
        @permission: Query for parent directories of these directories.
        @raise NotImplemented:
        @raise BadParameter:
        @raise IncorrectState:
        @raise PermissionDenied:
        @raise AuthorizationFailed:
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @note:  find operates recursively below the current working directory if the 'RECURSIVE' flag is specified (default)
        @note:  find does not follow symbolically linked directories, unless the 'DEREFERENCE' flag is specified - otherwise find lists symbolic link entries with a matching name.
        @note:  the default flags are 'RECURSIVE' (1).
        @note:  other flags are not allowed, and cause a 'BadParameter' exception.
        @note:  the name_pattern follows the standard POSIX shell wildcard specification, as described above.
        @note:  the matching entries returned are path names relative to cwd.
        @note:  similar to 'find' as defined by POSIX, but limited to the -name option.

        """
        if type(name_pattern) is not str:
            raise BadParameter, "Parameter name_pattern is not a string. Type: " + str(type(name_pattern))
        if type(flags) is not int:
            raise BadParameter, "Parameter flags is not an int. Type: " + str(type(int))
        if tasktype is not TaskType.NORMAL and tasktype is not TypeTask.SYNC \
        and tasktype is not TaskType.ASYNC  and tasktype is not TypeTask.TASK:
            raise BadParameter, "Parameter tasktype is not one of the TypeTask values, but " + str(tasktype)
        try:
            if tasktype is TaskType.ASYNC:
                javaObject = self.delegateObject.find(TaskMode.ASYNC, name_pattern, flags)
                return Task(delegateObject=javaObject)
            if tasktype is TaskType.SYNC:
                javaObject = self.delegateObject.find(TaskMode.SYNC, name_pattern, flags)
                return Task(delegateObject=javaObject)
            if tasktype is TaskType.TASK:
                javaObject = self.delegateObject.find(TaskMode.TASK, name_pattern, flags)
                return Task(delegateObject=javaObject)
            else:
                javaArray = self.delegateObject.find(name_pattern, flags)                
                retval = []
                for i in range(len(javaArray)):
                    ret.append(URL(delegateObject = javaArray[i]))
                return retval
        except org.ogf.saga.error.SagaException, e:
            raise self.convertException(e)

    #navigation/query methods
    def exists (self, name, tasktype=TaskType.NORMAL):
        #in  URL name, out boolean exists
        """
        Checks if entry exists
        @summary: checks if entry exists
        @param name: name to be tested for existence
        @type name : L{URL}
        @rtype: bool
        @return: bool indicating existence of name
        @permission: Query for name.
        @permission: Exec for name's parent directory.
        @permission: Read for name's parent directory.
        @raise NotImplemented:
        @raise IncorrectURL:
        @raise BadParameter:
        @raise IncorrectState:
        @raise PermissionDenied:
        @raise AuthorizationFailed:
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @note:  if 'name' can be parsed as URL, but contains an invalid entry name, an 'BadParameter' exception is raised.
        @note:  note that no exception is raised if the entry does not exist. The method just returns
              'False' in this case.
        @note:  similar to 'test -e' as defined by POSIX.

        """
        if type(name) is not URL:
            raise BadParameter, "Parameter name is not an URL. Type: " + str(type(name))
        if tasktype is not TaskType.NORMAL and tasktype is not TypeTask.SYNC \
        and tasktype is not TaskType.ASYNC  and tasktype is not TypeTask.TASK:
            raise BadParameter, "Parameter tasktype is not one of the TypeTask values, but " + str(tasktype)
        try:
            if tasktype is TaskType.ASYNC:
                javaObject = self.delegateObject.exists(TaskMode.ASYNC, name.delegateObject)
                return Task(delegateObject=javaObject)
            if tasktype is TaskType.SYNC:
                javaObject = self.delegateObject.exists(TaskMode.SYNC, name.delegateObject)
                return Task(delegateObject=javaObject)
            if tasktype is TaskType.TASK:
                javaObject = self.delegateObject.exists(TaskMode.TASK, name.delegateObject)
                return Task(delegateObject=javaObject)
            else:
                retval = self.delegateObject.exists(name.delegateObject)                
                if reval is 1: return True
                else: return False
        except org.ogf.saga.error.SagaException, e:
            raise self.convertException(e)

#DOCUMENT: difference type False/True -> bool-int
#TODO: document Overridden methods. Make method include NSEntry and NSDirectory versions x_self.

    
    #navigation/query methods
    def is_dir(self, name, tasktype=TaskType.NORMAL):
        #in URL name, out boolean test 
        """
        Tests url for being a directory
        @summary: tests name for being a directory
        @param name: URL to be tested
        @type name: L{URL}
        @rtype: bool
        @return: bool indicating if name is a directory
        @permission:    Query for name.
        @permission: Exec for name's parent directory.
        @permission: Read for name's parent directory.
        @raise NotImplemented:
        @raise IncorrectURL:
        @raise BadParameter:
        @raise DoesNotExist:
        @raise IncorrectState:
        @raise PermissionDenied:
        @raise AuthorizationFailed:
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @note:  returns True if the instance represents a directory entry, False otherwise
        @note:  all notes to the NSEntry.is_dir() method apply.
        @note:  if 'name' can be parsed as URL, but contains an invalid entry name, an 'BadParameter'
            exception is raised.
        @note:  if 'name' is a valid entry name but the entry does not exist, a 'DoesNotExist' exception is raised.
        @note:  similar to 'test -d' as defined by POSIX.

        """
        if type(name) is not URL:
            raise BadParameter, "Parameter name is not an URL. Type: " + str(type(name))
        if tasktype is not TaskType.NORMAL and tasktype is not TypeTask.SYNC \
        and tasktype is not TaskType.ASYNC  and tasktype is not TypeTask.TASK:
            raise BadParameter, "Parameter tasktype is not one of the TypeTask values, but " + str(tasktype)
        try:
            if tasktype is TaskType.ASYNC:
                javaObject = self.delegateObject.isDir(TaskMode.ASYNC, name.delegateObject)
                return Task(delegateObject=javaObject)
            if tasktype is TaskType.SYNC:
                javaObject = self.delegateObject.isDir(TaskMode.SYNC, name.delegateObject)
                return Task(delegateObject=javaObject)
            if tasktype is TaskType.TASK:
                javaObject = self.delegateObject.isDir(TaskMode.TASK, name.delegateObject)
                return Task(delegateObject=javaObject)   
            else:       
                retval = self.delegateObject.isDir(name.delegateObject)
                if retval == 1:
                    return True
                else: 
                    return False
        except org.ogf.saga.error.SagaException, e:
            raise self.convertException(e)

    #navigation/query methods
    def is_entry (self, name, tasktype=TaskType.NORMAL):
        #in URL name, out boolean test 
        """
        Tests name for being an NSEntry
        @summary: tests name for being an NSEntry
        @param name: name to be tested
        @type name: L{URL}
        @rtype: bool
        @return: bool indicating if name is a non-directory entry
        @permission: Query for name.
        @permission: Exec for name's parent directory.
        @permission: Read for name's parent directory.
        @raise NotImplemented:
        @raise IncorrectURL:
        @raise BadParameter:
        @raise DoesNotExist:
        @raise IncorrectState:
        @raise PermissionDenied:
        @raise AuthorizationFailed:
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @note: all notes to the NSEntry.is_entry() method apply.
        @note: if 'name' can be parsed as URL, but contains an invalid entry name, a 'BadParameter' exception is raised.
        @note: if 'name' is a valid entry name but the entry does not exist, a 'DoesNotExist' exception is raised.
        @note:  similar to 'test -f' as defined by POSIX.

        """
        if type(name) is not URL:
            raise BadParameter, "Parameter name is not an URL. Type: " + str(type(name))
        if tasktype is not TaskType.NORMAL and tasktype is not TypeTask.SYNC \
        and tasktype is not TaskType.ASYNC  and tasktype is not TypeTask.TASK:
            raise BadParameter, "Parameter tasktype is not one of the TypeTask values, but " + str(tasktype)
        try:
            if tasktype is TaskType.ASYNC:
                javaObject = self.delegateObject.isEntry(TaskMode.ASYNC, name.delegateObject)
                return Task(delegateObject=javaObject)
            if tasktype is TaskType.SYNC:
                javaObject = self.delegateObject.isEntry(TaskMode.SYNC, name.delegateObject)
                return Task(delegateObject=javaObject)
            if tasktype is TaskType.TASK:
                javaObject = self.delegateObject.isEntry(TaskMode.TASK, name.delegateObject)
                return Task(delegateObject=javaObject)   
            else:  
                retval = self.delegateObject.isEntry(name.delegateObject)
                if retval == 1:
                    return True
                else: 
                    return False
        except org.ogf.saga.error.SagaException, e:
            raise self.convertException(e)
 
    #navigation/query methods
    def is_link(self, name, tasktype=TaskType.NORMAL):
        #in saga::url name, out boolean test
        """
        Tests name for being a symbolic link
        @summary: tests name for being a symbolic link
        @param name: name to be tested
        @type name: L{URL}
        @rtype: bool
        @return: bool indicating if name is a link
        @permission:    Query for name.
        @permission: Exec for name's parent directory.
        @permission: Read for name's parent directory.
        @raise NotImplemented:
        @raise IncorrectURL:
        @raise BadParameter:
        @raise IncorrectState:
        @raise DoesNotExist:
        @raise PermissionDenied:
        @raise AuthorizationFailed:
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @note: all notes to the NSEntry.is_link() method apply.
        @note: if 'name' can be parsed as URL, but contains an invalid entry name, a 'BadParameter' exception is raised.
        @note: if 'name' is a valid entry name but the entry does not exist, a 'DoesNotExist' exception is raised.
        @note: similar to 'test -L' as defined by POSIX.

        """
        if type(name) is not URL:
            raise BadParameter, "Parameter name is not an URL. Type: " + str(type(name))
        if tasktype is not TaskType.NORMAL and tasktype is not TypeTask.SYNC \
        and tasktype is not TaskType.ASYNC  and tasktype is not TypeTask.TASK:
            raise BadParameter, "Parameter tasktype is not one of the TypeTask values, but " + str(tasktype)
        try:
            if tasktype is TaskType.ASYNC:
                javaObject = self.delegateObject.isLink(TaskMode.ASYNC, name.delegateObject)
                return Task(delegateObject=javaObject)
            if tasktype is TaskType.SYNC:
                javaObject = self.delegateObject.isLink(TaskMode.SYNC, name.delegateObject)
                return Task(delegateObject=javaObject)
            if tasktype is TaskType.TASK:
                javaObject = self.delegateObject.isLink(TaskMode.TASK, name.delegateObject)
                return Task(delegateObject=javaObject)   
            else:
                retval = self.delegateObject.isLink(name.delegateObject)
                if retval == 1:
                    return True
                else: 
                    return False
        except org.ogf.saga.error.SagaException, e:
            raise self.convertException(e)
        
    def read_link(self, name, tasktype=TaskType.NORMAL):
        #in URL name, out URL link
        """
        Returns the name of the link target
        @summary: returns the name of the link target
        @param name: name to be resolved
        @type name: L{URL}
        @rtype: L{URL}
        @return: resolved name
        @permission:    Query for name.
        @permission: Exec for name's parent directory.
        @permission: Read for name's parent directory.
        @raise NotImplemented:
        @raise IncorrectURL:
        @raise BadParameter:
        @raise DoesNotExist:
        @raise IncorrectState:
        @raise PermissionDenied:
        @raise AuthorizationFailed:
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @note:  all notes to ns_entry::read_link() apply
        @note:  if 'name' can be parsed as URL, but contains an invalid entry name, a 'BadParameter'
                  exception is raised.
        @note:  if 'name' does not exist, a 'DoesNotExist' exception is raised.

        """
        if type(name) is not URL:
            raise BadParameter, "Parameter name is not an URL. Type: " + str(type(name))
        if tasktype is not TaskType.NORMAL and tasktype is not TypeTask.SYNC \
        and tasktype is not TaskType.ASYNC  and tasktype is not TypeTask.TASK:
            raise BadParameter, "Parameter tasktype is not one of the TypeTask values, but " + str(tasktype)
        try:
            if tasktype is TaskType.ASYNC:
                javaObject = self.delegateObject.readLink(TaskMode.ASYNC, name.delegateObject)
                return Task(delegateObject=javaObject)
            if tasktype is TaskType.SYNC:
                javaObject = self.delegateObject.readLink(TaskMode.SYNC, name.delegateObject)
                return Task(delegateObject=javaObject)
            if tasktype is TaskType.TASK:
                javaObject = self.delegateObject.readLink(TaskMode.TASK,name.delegateObject)
                return Task(delegateObject=javaObject)   
            else:
                retval = self.delegateObject.readLink(name.delegateObject)
                return URL (delegateObject = retval)
        except org.ogf.saga.error.SagaException, e:
            raise self.convertException(e)
 
    # manage entries by number
    def get_num_entries (self, tasktype=TaskType.NORMAL):
        #out int num
        """
        Gives the number of entries in the directory
        @summary:gives the number of entries in the directory
        @rtype: int
        @return: number of entries in the directory
        @permission: Query for cwd.
        @permission: Exec for cwd.
        @permission: Read for cwd.
        @raise NotImplemented:
        @raise IncorrectState:
        @raise PermissionDenied:
        @raise AuthorizationFailed:
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @note: at the time of using the result of this call, the actual number of entries may already have
            changed (no locking is implied)
        @note: vaguely similar to 'opendir'/'readdir' (2) as defined by POSIX.

        """
        if tasktype is not TaskType.NORMAL and tasktype is not TypeTask.SYNC \
        and tasktype is not TaskType.ASYNC  and tasktype is not TypeTask.TASK:
            raise BadParameter, "Parameter tasktype is not one of the TypeTask values, but " + str(tasktype)
        try:
            if tasktype is TaskType.ASYNC:
                javaObject = self.delegateObject.getNumEntries(TaskMode.ASYNC)
                return Task(delegateObject=javaObject)
            if tasktype is TaskType.SYNC:
                javaObject = self.delegateObject.getNumEntries(TaskMode.SYNC)
                return Task(delegateObject=javaObject)
            if tasktype is TaskType.TASK:
                javaObject = self.delegateObject.getNumEntries(TaskMode.TASK)
                return Task(delegateObject=javaObject)   
            else:
                return self.delegateObject.getNumEntries()
        except org.ogf.saga.error.SagaException, e:
            raise self.convertException(e)

    # manage entries by number
    def get_entry(self, entry, tasktype=TaskType.NORMAL):
        #in int entry, out URL name 
        """
        Gives the name of an entry in the directory based upon the enumeration defined by get_num_entries
        @summary:gives the name of an entry in the directory
        @param entry: index of entry to get
        @type entry: int
        @rtype: L{URL}
        @return: name of entry at index
        @permission:    Query for cwd.
        @permission: Exec for cwd.
        @permission: Read for cwd.
        @raise NotImplemented:
        @raise IncorrectState:
        @raise DoesNotExist:
        @raise PermissionDenied:
        @raise AuthorizationFailed:
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @note: '0' is the first entry
        @note: there is no sort order implied by the enumeration, however an underlying
               implementation MAY choose to sort the entries
        @note:  subsequent calls to get_entry and/or get_num_entries may return inconsistent data,
               i.e. no locking or state tracking is implied. In particular, an index may be invalid - a
               'DoesNotExist' exception is then raised (not a 'BadParameter' exception).
        @note:  vaguely similar to 'opendir'/'readdir' (2) as defined by POSIX.

        """       
        if type(entry) is not int:
            raise BadParameter, "Parameter entry is not an int. Type: " + str(type(entry))
        if tasktype is not TaskType.NORMAL and tasktype is not TypeTask.SYNC \
        and tasktype is not TaskType.ASYNC  and tasktype is not TypeTask.TASK:
            raise BadParameter, "Parameter tasktype is not one of the TypeTask values, but " + str(tasktype)
        try:
            if tasktype is TaskType.ASYNC:
                javaObject = self.delegateObject.getEntry(TaskMode.ASYNC, entry)
                return Task(delegateObject=javaObject)
            if tasktype is TaskType.SYNC:
                javaObject = self.delegateObject.getNumEntries(TaskMode.SYNC, entry)
                return Task(delegateObject=javaObject)
            if tasktype is TaskType.TASK:
                javaObject = self.delegateObject.getNumEntries(TaskMode.TASK, entry)
                return Task(delegateObject=javaObject)   
            else:
                javaObject = self.delegateObject.getEntry(entry)
                return URL(delegateObject = javaObject)
        except org.ogf.saga.error.SagaException, e:
            raise self.convertException(e)

    # management methods + management methods - wildcard versions
    def copy(self, source, target, flags = Flags.NONE, tasktype=TaskType.NORMAL):
        #in URL source,   in URL target, in int flags = None
        #in string source,in URL target, in int flags = None
        """
        Copy the entry to another part of the name space
        @summary: copy the entry to another part of the name space
        @param source: name to copy
        @param target: name to copy to
        @param flags: flags defining the operation modus
        @type source: L{URL} or string
        @type target: L{URL}
        @type flags: int
        @postcondition: an identical copy of source exists at target.
        @postcondition:  'Owner' of target is the id of the context used to perform the opereration if target gets
                  created.
        @permission: Query for source.
        @permission: Exec for source's parent directory.
        @permission: Query for target.
        @permission: Query for target's parent directory.
        @permission: Exec for target's parent directory.
        @permission: Write for target if target does exist.
        @permission: Write for target's parent directory if target does not exist.
        @raise NotImplemented:
        @raise IncorrectURL:
        @raise BadParameter:
        @raise AlreadyExists:
        @raise DoesNotExist:
        @raise IncorrectState:
        @raise PermissionDenied:
        @raise AuthorizationFailed:
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @note: all notes to the NSEntry.copy() method apply.
        @note: the default flags are 'NONE' (0).
        @note: if 'name' can be parsed as URL, but contains an invalid entry name, a 'BadParameter' exception is raised.
        @note: if 'name' is a valid entry name but the entry does not exist, a 'DoesNotExist' exception is raised.
        @note: when 'source' as a string is given as parameter, it can contain wildcards.
        @note:  on error conditions on any of the expanded
           list of source entries, the respective error is raised. The state of the operations on the
           other elements of the expanded entry list is undefined.
        @note:  if source expands to multiple entries, then the
           target URL specifies a directory, otherwise a 'BadParameter' exception is raised.

        """
        if type(source) is not URL or type(source) is not str:
            raise BadParameter, "Parameter source is not a URL or string. Type: " + str(type(source))
        if type(target) is not URL:
            raise BadParameter, "Parameter target is not a URL. Type: " + str(type(target))      
        if type(flags) is not int:
            raise BadParameter, "Parameter flags is not an int. Type: " + str(type(flags)) 
        if tasktype is not TaskType.NORMAL and tasktype is not TypeTask.SYNC \
        and tasktype is not TaskType.ASYNC  and tasktype is not TypeTask.TASK:
            raise BadParameter, "Parameter tasktype is not one of the TypeTask values, but " + str(tasktype)
        if type(source) is str:
            source_parameter = source
        else:
            source_parameter = source.delegateObject
        try:
            if tasktype is TaskType.ASYNC:
                javaObject = self.delegateObject.copy(TaskMode.ASYNC, source_parameter, target.delegateObject, flags)
                return Task(delegateObject=javaObject)
            if tasktype is TaskType.SYNC:
                javaObject = self.delegateObject.copy(TaskMode.SYNC, source_parameter, target.delegateObject, flags)
                return Task(delegateObject=javaObject)
            if tasktype is TaskType.TASK:
                javaObject = self.delegateObject.copy(TaskMode.TASK, source_parameter, target.delegateObject, flags)
                return Task(delegateObject=javaObject)
            else:
                self.delegateObject.copy( source_parameter, target.delegateObject, flags)
        except org.ogf.saga.error.SagaException, e:
            raise self.convertException(e)
        

    # management methods + management methods - wildcard versions
    def link(self, source, target, flags = Flags.NONE, tasktype=TaskType.NORMAL):
        #in URL source,    in URL target, in int flags = None
        #in string source, in URL target, in int flags = None
        """
        Create a symbolic link from the target entry to the source entry so that any reference to the
        target refers to the source entry
        @summary: create a symbolic link from the target entry.
        @param source: name of link
        @param target: name to link to
        @param flags: flags defining the operation modus
        @type source:  L{URL} or string
        @type target: L{URL}
        @type flags: int
        @postcondition: - a symbolic link to source exists at target.
        @postcondition:  'Owner' of target is the id of the context used to perform the opereration if target gets
                  created.
        @permission:  Query for source.
        @permission: Exec  for source's parent directory.
        @permission: Query for target.
        @permission: Query for target's parent directory.
        @permission: Exec  for target's parent directory.
        @permission: Write for target if target does exist.
        @permission: Write for target's parent directory if target does not exist.
        @raise NotImplemented:
        @raise IncorrectURL:
        @raise BadParameter:
        @raise AlreadyExists:
        @raise DoesNotExist:
        @raise IncorrectState:
        @raise PermissionDenied:
        @raise AuthorizationFailed:
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @note: all notes to the NSEntry.link() method apply.
        @note:  if the 'RECURSIVE' flag is defined, the source is recursively linked if it is a directory;
               otherwise this flag is ignored.
        @note:  if the 'DEREFERENCE' flag is specified, the method applies to the link target of source.
               The flag causes a 'BadParameter' exception if source is not a link.
        @note:  if the the target already exists, the 'OVERWRITE' flag must be specified, otherwise
               an 'AlreadyExists' exception is raised.
        @note:  the default flags are 'NONE' (0).
        @note:  other flags are not allowed on this method, and cause a 'BadParameter' exception.
        @note:  if 'source' can be parsed as URL, but contains an invalid entry name, a 'BadParameter'
               exception is raised.
        @note: if 'source' is a valid entry name but the entry does not exist, a 'DoesNotExist' exception is raised.
        @note: the 'source' parameter as string can contain wildcards.
        @note: on error conditions on any of the expanded list of source entries, the respective error
               is raised - the state of the operations on the other elements of the expanded entry list is undefined.
        @note: if source expands to multiple entries, then the
             target URL specifies a directory. Otherwise a 'BadParameter' exception is raised.

        """
        if type(source) is not URL or type(source) is not str:
            raise BadParameter, "Parameter source is not a URL or string. Type: " + str(type(source))
        if type(target) is not URL:
            raise BadParameter, "Parameter target is not a URL. Type: " + str(type(target))
        if type(flags) is not int:
            raise BadParameter, "Parameter flags is not an int. Type: " + str(type(flags)) 
        if tasktype is not TaskType.NORMAL and tasktype is not TypeTask.SYNC \
        and tasktype is not TaskType.ASYNC  and tasktype is not TypeTask.TASK:
            raise BadParameter, "Parameter tasktype is not one of the TypeTask values, but " + str(tasktype)
        if type(source) is str:
            source_parameter = source
        else:
            source_parameter = source.delegateObject
        try:
            if tasktype is TaskType.ASYNC:
                javaObject = self.delegateObject.link(TaskMode.ASYNC,source_parameter, target.delegateObject, flags)
                return Task(delegateObject=javaObject)
            if tasktype is TaskType.SYNC:
                javaObject = self.delegateObject.link(TaskMode.SYNC, source_parameter, target.delegateObject, flags)
                return Task(delegateObject=javaObject)
            if tasktype is TaskType.TASK:
                javaObject = self.delegateObject.link(TaskMode.TASK, source_parameter, target.delegateObject, flags)
                return Task(delegateObject=javaObject)
            else:
                self.delegateObject.link(target.delegateObject, source_parameter, flags)
        except org.ogf.saga.error.SagaException, e:
            raise self.convertException(e)      

  
   # management methods + management methods - wildcard versions
    def move (self, source, target, flags = Flags.NONE, tasktype=TaskType.NORMAL):
       #in URL source,    in URL target, in int flags = None
       #in string source, in URL target, in int flags = None
        """
        Rename source to target, or move source to target if target is a directory.
        @summary: rename or move target
        @param source: name to move
        @param target: name to move to
        @param flags: flags defining the operation modus
        @type source: L{URL} or string
        @type target: L{URL}
        @type flags: int
        @postcondition: an identical copy of source exists at target.
        @postcondition: source is removed.
        @postcondition: 'Owner' of target is the id of the context used to perform the opereration if target gets
                  created.
        @permission: Query for source.
        @permission: Write for source.
        @permission: Exec for source's parent directory.
        @permission: Write for source's parent directory.
        @permission: Query for target.
        @permission: Exec for target's parent directory.
        @permission: Write for target if target does exist.
        @permission: Write for target's parent directory if target does not exist.
        @raise NotImplemented:
        @raise IncorrectURL:
        @raise BadParameter:
        @raise AlreadyExists:
        @raise DoesNotExist:
        @raise IncorrectState:
        @raise PermissionDenied:
        @raise AuthorizationFailed:
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @note: all notes to the NSEntry.move() method apply.
        @note:  if the 'RECURSIVE' flag is defined, the source is recursively copied if it is a directory;
                  otherwise this flag is ignored.
        @note:  if the 'DEREFERENCE' flag is specified, the method applies to the link target of source.
                  The flag causes a 'BadParameter' exception if source is not a link.
        @note:  if the the target already exists, the 'OVERWRITE' flag must be specified, otherwise
             an 'AlreadyExists' exception is raised.
        @note:  the default flags are 'NONE' (0).
        @note:  other flags are not allowed on this method, and cause a 'BadParameter' exception.
        @note:  if 'source' can be parsed as URL, but contains an invalid entry name, a 'BadParameter' exception is raised.
        @note:  if 'source' is a valid entry name but the entry does not exist, a 'DoesNotExist' exception is raised.
        @note:  moving any parent or the current directoy (e.g. '.', '..' etc.) is not allowed, and raises a 'BadParameter' exception
        @note:  the 'source' string as parameter can contain wildcards.
        @note:  on error conditions on any of the expanded list of source entries, the respective error
           is raised - the state of the operations on the other elements of the expanded entry list is undefined.
        @note:  if source expands to multiple entries, then the target URL specifies a directory -
           otherwise a 'BadParameter' exception is raised.

        """
        if type(source) is not URL or type(source) is not str:
            raise BadParameter, "Parameter source is not a URL or string. Type: " + str(type(source))
        if type(target) is not URL:
            raise BadParameter, "Parameter target is not a URL. Type: " + str(type(target))
        if type(flags) is not int:
            raise BadParameter, "Parameter flags is not an int. Type: " + str(type(flags)) 
        if tasktype is not TaskType.NORMAL and tasktype is not TypeTask.SYNC \
        and tasktype is not TaskType.ASYNC  and tasktype is not TypeTask.TASK:
            raise BadParameter, "Parameter tasktype is not one of the TypeTask values, but " + str(tasktype)
        if type(source) is str:
            source_parameter = source
        else:
            source_parameter = source.delegateObject
        try:
            if tasktype is TaskType.ASYNC:
                javaObject = self.delegateObject.move(TaskMode.ASYNC, source_parameter, target.delegateObject, flags)
                return Task(delegateObject=javaObject)
            if tasktype is TaskType.SYNC:
                javaObject = self.delegateObject.move(TaskMode.SYNC, source_parameter, target.delegateObject, flags)
                return Task(delegateObject=javaObject)
            if tasktype is TaskType.TASK:
                javaObject = self.delegateObject.move(TaskMode.TASK, source_parameter, target.delegateObject, flags)
                return Task(delegateObject=javaObject)
            else:
                self.delegateObject.move(source_parameter, target.delegateObject, flags)
        except org.ogf.saga.error.SagaException, e:
            raise self.convertException(e)    
  
   # management methods + management methods - wildcard versions
    def remove(self, target, flags =  Flags.NONE, tasktype=TaskType.NORMAL):
        #in URL    target, in int flags = None
        #in string target, in int flags = None
        """
        Removes the entry
        @summary: removes the entry
        @param target: entry to be removed
        @param flags: defining the operation modus
        @type target: L{URL} or string
        @type flags: int
        @postcondition: source is removed.
        @postcondition: source is closed if it refers to the cwd.
        @permission: Query for source.
        @permission: Write for source.
        @permission: Exec for source's parent directory.
        @permission: Write for source's parent directory.
        @raise NotImplemented:
        @raise IncorrectURL:
        @raise BadParameter:
        @raise AlreadyExists:
        @raise DoesNotExist:
        @raise IncorrectState:
        @raise PermissionDenied:
        @raise AuthorizationFailed:
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @note: all notes to the NSEntry.remove() method apply.
        @note: if the 'RECURSIVE' flag is defined, the source is recursively removed if it is a directory;
                  otherwise this flag is ignored.
        @note:  if the 'DEREFERENCE' flag is specified, the method applies to the link target of source.
             The flag causes a 'BadParameter' exception if source is not a link.
        @note:  the default flags are 'NONE' (0).
        @note:  other flags are not allowed on this method, and cause a 'BadParameter' exception.
        @note:  if 'source' can be parsed as URL, but contains an invalid entry name, a 'BadParameter' exception is raised.
        @note:  if 'source' is a valid entry name but the entry does not exist, a 'DoesNotExist' exception is raised.
        @note:  removing any parent or the current directoy (e.g. '.', '..' etc.) is not allowed, and
             raises a 'BadParameter' exception
        @note:  the 'source' string as parameter can contain wildcards
        @note:  on error conditions on any of the expanded list of source entries, the respective error
            is raised - the state of the operations on the other elements of the expanded entry list is undefined.

        """
        if type(source) is not URL or type(source) is not str:
            raise BadParameter, "Parameter source is not a URL or string. Type: " + str(type(source))
        if type(flags) is not int:
            raise BadParameter, "Parameter flags is not an int. Type: " + str(type(flags)) 
        if tasktype is not TaskType.NORMAL and tasktype is not TypeTask.SYNC \
        and tasktype is not TaskType.ASYNC  and tasktype is not TypeTask.TASK:
            raise BadParameter, "Parameter tasktype is not one of the TypeTask values, but " + str(tasktype)
        if type(source) is str:
            source_parameter = source
        else:
            source_parameter = source.delegateObject
        try:
            if tasktype is TaskType.ASYNC:
                javaObject = self.delegateObject.remove(TaskMode.ASYNC, source_parameter, flags)
                return Task(delegateObject=javaObject)
            if tasktype is TaskType.SYNC:
                javaObject = self.delegateObject.remove(TaskMode.SYNC, source_parameter, flags)
                return Task(delegateObject=javaObject)
            if tasktype is TaskType.TASK:
                javaObject = self.delegateObject.remove(TaskMode.TASK, source_parameter, flags)
                return Task(delegateObject=javaObject)
            else:
                self.delegateObject.remove(source_parameter, flags)
        except org.ogf.saga.error.SagaException, e:
            raise self.convertException(e)             
  
   # management methods
    def make_dir(self, target, flags = Flags.NONE, tasktype=TaskType.NORMAL):
        """
        Creates a new directory
        @summary: creates a new directory
        @param target: directory to create
        @param flags: flags defining the operation modus
        @type target: L{URL}
        @type flags: int
        @postcondition: 'Owner' of target is the id of the context used to perform the opereration if target gets
                  created.
        @permission:    Exec for target's parent directory.
        @permission: Write for target's parent directory.
        @permission: Write for target if Write is set.
        @permission: Read for target if Read is set.
        @raise NotImplemented:
        @raise IncorrectURL:
        @raise BadParameter:
        @raise AlreadyExists:
        @raise DoesNotExist:
        @raise IncorrectState:
        @raise PermissionDenied:
        @raise AuthorizationFailed:
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @note:  if the parent directory or directories do not exist, the 'CREATEPARENTS' flag must be set
              or a 'DoesNotExist' exception is raised. If set, the parent directories are created as well.
        @note:  an 'AlreadyExists' exception is raised if the directory already exists and the 'EXCLUSIVE'
             flag is given.
        @note:  the default flags are 'NONE' (0).
        @note:  other flags are not allowed on this method, and cause a 'BadParameter' exception.
        @note:  if 'target' can be parsed as URL, but contains an invalid entry name, a 'BadParameter'
             exception is raised.
        @note:  similar to 'mkdir' (2) as defined by POSIX.
        
        """
        #in saga::url target, in int flags =  None
        if type(target) is not URL:
            raise BadParameter, "Parameter target is not a URL. Type: " + str(type(target))
        if type(flags) is not int:
            raise BadParameter, "Parameter flags is not an int. Type: " + str(type(flags)) 
        if tasktype is not TaskType.NORMAL and tasktype is not TypeTask.SYNC \
        and tasktype is not TaskType.ASYNC  and tasktype is not TypeTask.TASK:
            raise BadParameter, "Parameter tasktype is not one of the TypeTask values, but " + str(tasktype)
        try:
            if tasktype is TaskType.ASYNC:
                javaObject = self.delegateObject.makeDir(TaskMode.ASYNC, target.delegateObject, flags)
                return Task(delegateObject=javaObject)
            if tasktype is TaskType.SYNC:
                javaObject = self.delegateObject.makeDir(TaskMode.SYNC, target.delegateObject, flags)
                return Task(delegateObject=javaObject)
            if tasktype is TaskType.TASK:
                javaObject = self.delegateObject.makeDir(TaskMode.TASK, target.delegateObject, flags)
                return Task(delegateObject=javaObject)
            else:
                self.delegateObject.makeDir(target.delegateObject, flags)
        except org.ogf.saga.error.SagaException, e:
            raise self.convertException(e)
  
    #factory methods
    def open(self, name, flags = Flags.NONE, tasktype=TaskType.NORMAL ):
        #in  saga::url name, in  int flags = None, out ns_entry entry
        """
        Creates a new NSEntry instance
        @summary: creates a new NSEntry instance
        @param name:  entry to open
        @param flags: flags defining the operation modus
        @type name: L{URL}
        @type flags: int
        @rtype: L{NSEntry}
        @return: opened entry instance
        @postcondition: the session of the returned instance is that of the calling instance.
        @postcondition: 'Owner' of name is the id of the context used to perform the opereration if name gets created.
        @postcondition: the namespace entry is created if it does not yet exist, and the CREATE flag is specified.
        @permission: Exec for name's parent directory.
        @permission: Write for name's parent directory if Create is set.
        @permission: Write for name if Write is set.
        @permission: Read for name if Read is set.
        @raise NotImplemented:
        @raise IncorrectURL:
        @raise BadParameter:
        @raise AlreadyExists:
        @raise DoesNotExist:
        @raise IncorrectState:
        @raise PermissionDenied:
        @raise AuthorizationFailed:
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @note: a 'BadParameter' exception is raised if 'name' points to a directory, or is an invalid entry name.
        @note: a 'DoesNotExist' exception is raised if 'name' does not exist, and the 'CREATE' flag is not given.
        @note: a 'AlreadyExists' exception is raised if 'name' does exist, and the 'CREATE' and 'EXCLUSIVE'
                 flags are given.
        @note: 'name' is always deeply dereferenced, the cwd, however, is not changed to the link targets cwd.
        @note: parent directories are created on the fly if the 'CREATEPARENTS' and 'CREATE' flag are both
            given, if they don't exist.
        @note: the entry is locked on open if the 'LOCK' flag is given. If the entry is already in a locked
            state, the open will fail and a descriptive error will be issued. If a entry is opened in
            locked mode, any other open on that entry fails with a 'NoSuccess' exception if the 'LOCK'
            flag is given. Note that a entry can be opened in unlocked mode, and then in locked mode,
            without an error getting raised. The application programmer must take precautions
            to avoid such situations. The lock will get removed on destruction of the entry object, and
            also on close. If an implementation does not support locking, a descriptive 'BadParameter'
            exception is raised if the 'LOCK' flag is given. Read-locks and Write-locks are not distinguished.
        @note: the default flags are 'NONE' (0).
        @note: other flags are not allowed on this method, and cause a 'BadParameter' exception.
        @note: similar to 'open' (2) as defined by POSIX.

        """        
        if type(name) is not URL:
            raise BadParameter, "Parameter name is not a URL. Type: " + str(type(name))
        if type(flags) is not int:
            raise BadParameter, "Parameter flags is not an int. Type: " + str(type(flags)) 
        if tasktype is not TaskType.NORMAL and tasktype is not TypeTask.SYNC \
        and tasktype is not TaskType.ASYNC  and tasktype is not TypeTask.TASK:
            raise BadParameter, "Parameter tasktype is not one of the TypeTask values, but " + str(tasktype)
        try:
            if tasktype is TaskType.ASYNC:
                javaObject = self.delegateObject.open(TaskMode.ASYNC, target.delegateObject, flags)
                return Task(delegateObject=javaObject)
            if tasktype is TaskType.SYNC:
                javaObject = self.delegateObject.open(TaskMode.SYNC, target.delegateObject, flags)
                return Task(delegateObject=javaObject)
            if tasktype is TaskType.TASK:
                javaObject = self.delegateObject.open(TaskMode.TASK, target.delegateObject, flags)
                return Task(delegateObject=javaObject)
            else:
                javaObject = self.delegateObject.open(target.delegateObject, flags)
                return NSEntry(delegateObject = javaObject)
        except org.ogf.saga.error.SagaException, e:
            raise self.convertException(e)

 
    #factory methods
    def open_dir(self, name, flags = Flags.NONE, tasktype=TaskType.NORMAL):
        #in  saga::url name, in  int flags = None, out ns_directory dir
        """
        Creates a new NSDirectory instance
        @summary: creates a new NSDirectory instance
        @param name: directory to open
        @param flags: flags defining the operation modus
        @type name: L{URL}
        @type flags: int 
        @rtype: L{NSDirectory}
        @return: opened directory instance
        @postcondition: - the session of the returned instance is that of the calling instance.
        @postcondition: 'Owner' of name is the id of the context used to perform the opereration if name gets created.
        @postcondition: the namespace directory is created if it does not yet exist, and the Create is set.
        @permission: Exec for name's parent directory.
        @permission: Write for name's parent directory if Create is set.
        @permission: Write for name if Write is set.
        @permission: Read for name if Read is set.
        @raise NotImplemented:
        @raise IncorrectURL:
        @raise BadParameter:
        @raise AlreadyExists:
        @raise DoesNotExist:
        @raise IncorrectState:
        @raise PermissionDenied:
        @raise AuthorizationFailed:
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @note: the cwd of the new dir object instance is set to 'name'
        @note: a 'DoesNotExist' exception is raised if 'name' does not exist and the 'CREATE' flag is not given.
        @note: a 'AlreadyExist' exception is raised if 'name' does exist and the 'CREATE' flag and the
             'EXCLUSIVE' flag are given.
        @note: no exception is raised if 'name' does exist and the 'CREATE' flag is given, and the 'EXCLUSIVE'
              flag is not given.
        @note: if the 'CREATE' flag is given, all notes to the NSDirectory.make_dir() method apply.
        @note: the default flags are 'NONE' (0).
        @note: other flags are not allowed on this method, and cause a 'BadParameter' exception.
        @note: 'name' is always deeply dereferenced, however, the cwd is still set to 'name', and not to the
              value of the link target.
        @note: parent directories are created on the fly if the 'CREATEPARENTS' and 'CREATE' flag are both
              given, if they don't exist.
        @note: if 'name' can be parsed as URL, but contains an invalid directory name, a 'BadParameter' exception is raised

        """
        if type(name) is not URL:
            raise BadParameter, "Parameter name is not a URL. Type: " + str(type(name))
        if type(flags) is not int:
            raise BadParameter, "Parameter flags is not an int. Type: " + str(type(flags)) 
        if tasktype is not TaskType.NORMAL and tasktype is not TypeTask.SYNC \
        and tasktype is not TaskType.ASYNC  and tasktype is not TypeTask.TASK:
            raise BadParameter, "Parameter tasktype is not one of the TypeTask values, but " + str(tasktype)
        try:
            if tasktype is TaskType.ASYNC:
                javaObject = self.delegateObject.openDir(TaskMode.ASYNC, target.delegateObject, flags)
                return Task(delegateObject=javaObject)
            if tasktype is TaskType.SYNC:
                javaObject = self.delegateObject.openDir(TaskMode.SYNC, target.delegateObject, flags)
                return Task(delegateObject=javaObject)
            if tasktype is TaskType.TASK:
                javaObject = self.delegateObject.openDir(TaskMode.TASK, target.delegateObject, flags)
                return Task(delegateObject=javaObject)
            else:
                javaObject = self.delegateObject.openDir(target.delegateObject, flags)
                return NSDirectory(delegateObject = javaObject)
        except org.ogf.saga.error.SagaException, e:
            raise self.convertException(e)


    #permissions with flags + permissions with flags - wildcard version
    def permissions_allow(self, target, id, perm, flags = Flags.NONE, tasktype=TaskType.NORMAL):
        #in URL target,    in string id, in int perm, in int flags = None
        #in string target, in string id, in int perm, in int flags = None
        """
        Enable a permission
        @summary:enable a permission
        @param target: entry to set permissions for
        @param id: id to set permission for
        @param perm: permission to enable
        @param flags: mode of operation
        @type target: L{URL} or string
        @type id : string
        @type perm: value from L{Permission}
        @type flags: int
        @postcondition: the permissions are enabled.
        @permission: Owner of target
        @raise NotImplemented:
        @raise IncorrectURL:
        @raise BadParameter:
        @raise IncorrectState:
        @raise PermissionDenied:
        @raise AuthorizationFailed:
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @note:  all notes to permissions_allow from the saga.permissions.Permission class apply.
        @note:  allowed flags are: 'RECURSIVE', 'DEREFERENCE'. All other flags cause a 'BadParameter' exception.
        @note:  specifying 'RECURSIVE' for a non-directory causes a 'BadParameter' exception.
        @note: the 'source' string can contain wildcards.
        @note: on error conditions on any of the expanded list of source entries, the respective error
             is raised - the state of the operations on the other elements of the expanded entry list is undefined. 
        """ 
        if type(target) is not URL or type(source) is not str:
            raise BadParameter, "Parameter target is not a URL or string. Type: " + str(type(target))
        if type(id) is not str:
            raise BadParameter, "Parameter id is not a string. Type: " + str(type(id)) 
        if type(perm) is not int:
            raise BadParameter, "Parameter perm is not an int. Type: " + str(type(perm)) 
        if type(flags) is not int:
            raise BadParameter, "Parameter flags is not an int. Type: " + str(type(flags)) 
        if tasktype is not TaskType.NORMAL and tasktype is not TypeTask.SYNC \
        and tasktype is not TaskType.ASYNC  and tasktype is not TypeTask.TASK:
            raise BadParameter, "Parameter tasktype is not one of the TypeTask values, but " + str(tasktype)
        if type(source) is str: 
            target_parameter = target
        else: 
            target_parameter = target.delegateObject
        try:
            if tasktype is TaskType.ASYNC:
                javaObject = self.delegateObject.permissionsAllow(TaskMode.ASYNC, target_parameter, id, perm, flags)
                return Task(delegateObject=javaObject)
            if tasktype is TaskType.SYNC:
                javaObject = self.delegateObject.permissionsAllow(TaskMode.SYNC, target_parameter, id, perm, flags)
                return Task(delegateObject=javaObject)
            if tasktype is TaskType.TASK: 
                javaObject = self.delegateObject.permissionsAllow(TaskMode.TASK, target_parameter, id, perm, flags)
                return Task(delegateObject=javaObject)                                           
            else:
                self.delegateObject.permissionsAllow(target_parameter, id, perm, flags)
        except org.ogf.saga.error.SagaException, e:
            raise self.convertException(e)   


 
    #permissions with flags + permissions with flags - wildcard version
    def permissions_deny(self, target, id, perm, flags = Flags.NONE, tasktype=TaskType.NORMAL):
        #in URL target,    in string id, in int perm, in int flags = None
        #in string target, in string id, in int perm, in int flags = None
        """
        Disable a permission flag
        @summary: disable a permission flag
        @param target: entry to set permissions for
        @param id: id to set permission for
        @param perm: permission to disable
        @param flags: mode of operation
        @type target: L{URL} or string
        @type id : string
        @type perm: value from L{Permission}
        @type flags: int
        @postcondition: - the permissions are disabled.
        @permission:    Owner of target
        @raise NotImplemented:
        @raise IncorrectURL:
        @raise BadParameter:
        @raise IncorrectState:
        @raise PermissionDenied:
        @raise AuthorizationFailed:
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @note: all notes to permissions_deny from the saga.permissions.Permissions class apply.
        @note: allowed flags are: 'RECURSIVE', 'DEREFERENCE'. All other flags cause a 'BadParameter'
              exception.
        @note: specifying 'RECURSIVE' for a non-directory causes a 'BadParameter' exception.
        @note: the 'source' string can contain wildcards.
        @note: on error conditions on any of the expanded list of source entries, the respective error
             is raised - the state of the operations on the other elements of the expanded entry list is
             undefined. 
        """       
        if type(target) is not URL or type(source) is not str:
            raise BadParameter, "Parameter target is not a URL or string. Type: " + str(type(target))
        if type(id) is not str:
            raise BadParameter, "Parameter id is not a string. Type: " + str(type(id)) 
        if type(perm) is not int:
            raise BadParameter, "Parameter perm is not an int. Type: " + str(type(perm)) 
        if type(flags) is not int:
            raise BadParameter, "Parameter flags is not an int. Type: " + str(type(flags)) 
        if tasktype is not TaskType.NORMAL and tasktype is not TypeTask.SYNC \
        and tasktype is not TaskType.ASYNC  and tasktype is not TypeTask.TASK:
            raise BadParameter, "Parameter tasktype is not one of the TypeTask values, but " + str(tasktype)
        if type(source) is str: 
            target_parameter = target
        else: 
            target_parameter = target.delegateObject
        try:
            if tasktype is TaskType.ASYNC:
                javaObject = self.delegateObject.permissionsDeny(TaskMode.ASYNC, target_parameter, id, perm, flags)
                return Task(delegateObject=javaObject)
            if tasktype is TaskType.SYNC:
                javaObject = self.delegateObject.permissionsDeny(TaskMode.SYNC, target_parameter, id, perm, flags)
                return Task(delegateObject=javaObject)
            if tasktype is TaskType.TASK: 
                javaObject = self.delegateObject.permissionsDeny(TaskMode.TASK, target_parameter, id, perm, flags)
                return Task(delegateObject=javaObject)                                           
            else:
                self.delegateObject.permissionsDeny(target_parameter, id, perm, flags)
        except org.ogf.saga.error.SagaException, e:
            raise self.convertException(e)    

    def get_type(self):
        """
        Query the object type.
        @summary: Query the object type.
        @return: type of the object as an int from ObjectType
        @rtype: int
        """
        return ObjectType.NSDIRECTORY    
 