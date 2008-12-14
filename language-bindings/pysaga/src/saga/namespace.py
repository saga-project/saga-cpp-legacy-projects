# Package: saga
# Module: namespace
# Description: The module which specifies the classes concerning namespaces
# Specification and documentation can be found in section 4.2, page 195-239 of 
#    the GFD-R-P.90 document
# Author: P.F.A. van Zoolingen, Computer Systems Section, Faculty of 
#    Exact Science (FEW), Vrije Universiteit, Amsterdam, The Netherlands.

from object import Object
from permissions import Permissions
from task import Async, TaskType, Task
from error import NotImplemented
from session import Session

class Flags(object):
    """ 
    The Flags describe the properties of several operations on namespace entries.
    Packages which inherit from the saga.namespace package use the same flag 
    semantics unless specified otherwise, but will, in general, add additional 
    flags to some operations
    """
    NONE            =  0
    """ 
    @summary: indicates the absence of flags, and thus also implies that the 
        default flags for an operation do not apply, either
    """
    
    OVERWRITE       =  1
    """
    @summary: enforces an operation which creates a new namespace entry to 
        continue even if the target entry does already exist. If that flag is 
        not given, an 'AlreadyExists' exception would result from such an 
        operation
    """
    
    RECURSIVE       =  2
    """
    @summary: enforces an operation to apply recursively on a directory tree. 
        If that flag is not given, the same operation would only apply to the 
        given directory, and not to its children
    """
    
    DEREFERENCE     =  4
    """
    @summary: enforces an operation to apply not to the entry pointed to by the 
        target name, but to the link target of that entry. If that flag is not 
        given, the same operation would apply to the entry directly, and its 
        link target stays unaffected
    """
    
    CREATE          =  8
    """
    @summary: allows a namespace entry to be created while opening it, if it 
        does not already exist. If that flag is not given, the same open 
        operation would cause a 'DoesNotExist' exception. If the entry exists, 
        the flag is ignored.
    """
    
    EXCLUSIVE       = 16
    """
    @summary: implies a modification to the meaning of the 'CREATE' flag: if the 
        entry already exists, the 'CREATE' flag is is no longer silently ignored, 
        but causes an 'AlreadyExists' exception.
    """
    
    LOCK            = 32
    """
    @summary: enforces a lock on the name space entry when it is opened. Locks 
        are advisory in SAGA, semantic details for locking are defined in the 
        description of the open() call
    """
    
    CREATEPARENTS   = 64
    """
    @summary: An operation which would create a name space entry would normally
        fail if any path element in the targets name does not yet exist. If this
        flag is given, such an operation would not fail, but would imply that 
        the missing path elements are created on the fly.
    """
    

class NSEntry(Object, Permissions, Async): 
    """
    NSEntry defines methods which serve the inspection of the entry itself, 
    methods which allows to manage the entry (e.g. to copy, move, or remove it), 
    and methods to manipulate the entry's access control lists.
    """

    def __init__(self, name, session= Session(), flags=Flags.NONE, tasktype=TaskType.NORMAL):
        """
        Initialize the the object
        @summary: initialize the the object
        @param session: session handle
        @param name: initial working dir
        @param flags: open mode
        @type session: L{Session}
        @type name: L{URL}
        @type flags: int
        @param tasktype: return a normal NSEntry or a Task object that creates a
            NSEntry in a final, RUNNING or NEW state. By default, type 
            is L{TaskType.NORMAL}
        @type tasktype: int
        @postcondition: the entry is opened.
        @postcondition: 'Owner' of target is the id of the context use to 
            perform the opereration, if the entry gets created.
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

    def __del__(self):
        """
        Destroy the object
        @summary:destroy the object
        @postcondition: the entry is closed.
        @note:  if the instance was not closed before, the destructor performs 
            a close() on the instance, and all notes to close() apply.
        """

    def get_url(self, tasktype=TaskType.NORMAL):
        """
        Obtain the complete url pointing to the entry
        @summary: obtain the complete url pointing to the entry
        @param tasktype: return the normal return value or a Task object in a 
            final, RUNNING or NEW state. By default, tasktype is L{TaskType.NORMAL}
        @type tasktype: int
        @rtype: L{URL} 
        @return: url pointing to the entry
        @raise NotImplemented:
        @raise IncorrectState:
        @raise Timeout:
        @raise NoSuccess:
        """        

    def get_cwd(self, tasktype=TaskType.NORMAL):
        """
        Obtain the current working directory for the entry
        @summary: obtain the current working directory for the entry
        @type tasktype: int 
        @param tasktype: return the normal return value or a Task object in a 
            final, RUNNING or NEW state. By default, tasktype is L{TaskType.NORMAL}
        @rtype: L{URL}
        @return: current working directory
        @raise NotImplemented:
        @raise IncorrectState:
        @raise Timeout:
        @raise NoSuccess:
        @note: returns the directory part of the url path element.

        """        

    def get_name(self, tasktype=TaskType.NORMAL): 
        """
        Obtain the name part of the url path element
        @summary: obtain the name part of the url path element
        @type tasktype: int
        @param tasktype: return the normal return value or a Task object in a 
            final, RUNNING or NEW state. By default, tasktype is L{TaskType.NORMAL}
        @rtype: L{URL}
        @return: last part of path element
        @raise NotImplemented:
        @raise IncorrectState:
        @raise Timeout:
        @raise NoSuccess:
         """

    def is_dir_self(self, tasktype=TaskType.NORMAL):
        """
        Tests the entry for being a directory
        @summary: tests the entry for being a directory
        @type tasktype: int
        @param tasktype: return the normal return value or a Task object in a 
            final, RUNNING or NEW state. By default, tasktype is L{TaskType.NORMAL}
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

    def is_entry_self(self, tasktype=TaskType.NORMAL):
        """
        Tests the entry for being an NSEntry
        @summary: tests the entry for being an NSEntry
        @type tasktype: int
        @param tasktype: return the normal return value or a Task object in a 
            final, RUNNING or NEW state. By default, tasktype is L{TaskType.NORMAL}
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
        @note:  the method returns False if the entry is a link or a directory 
            (although an NSDirectory is a NSEntry, False is returned on a test 
            on an NSDirectory), otherwise True is returned.
        @note:  similar to 'test -f' as defined by POSIX.

        """        

    def is_link_self(self, tasktype=TaskType.NORMAL):
        """
        Tests the entry for being a link
        @summary: tests the entry for being a link
        @type tasktype: int 
        @param tasktype: return the normal return value or a Task object in a 
            final, RUNNING or NEW state. By default, tasktype is L{TaskType.NORMAL}
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

    def read_link_self(self, tasktype=TaskType.NORMAL):
        """
        Get the name of the link target
        @summary: get the name of the link target
        @type tasktype: int 
        @param tasktype: return the normal return value or a Task object in a 
            final, RUNNING or NEW state. By default, tasktype is L{TaskType.NORMAL}
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
        @note: if the entry instance this method is called upon does not point 
            to a link, an 'IncorrectState' exception is raised.
        @note:  similar to 'ls -L' as defined by POSIX.
        """        
    
    def copy_self(self, target, flags = Flags.NONE, tasktype=TaskType.NORMAL):
        """
        Copy the entry to another part of the name space
        @summary: copy the entry to another part of the name space
        @param target: name to copy to
        @param flags: flags defining the operation modus
        @type target: L{URL}
        @type flags: int 
        @type tasktype: int
        @param tasktype: return the normal return value or a Task object in a 
            final, RUNNING or NEW state. By default, tasktype is L{TaskType.NORMAL}
        @postcondition: an identical copy exists at target.
        @postcondition: 'Owner' of target is the id of the context use to 
            perform the opereration, if target gets created.
        @permission: Query
        @permission: Exec for parent directory.
        @permission: Query for target.
        @permission: Query for target's parent directory.
        @permission: Exec for target's parent directory.
        @permission: Write for target if target does exist.
        @permission: Write for target's parent directory if target does not 
            exist.
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
        @note: if the target is a directory, the source entry is copied into 
            that directory
        @note: a 'BadParameter' exception is raised if the source is a directory 
            and the 'RECURSIVE' flag is not set.
        @note: a 'BadParameter' exception is raised if the source is not a 
            directory and the 'RECURSIVE' flag is set.
        @note:  if the target lies in a non-existing part of  the name space, a 
            'DoesNotExist' exception is raised, unless the 'CREATEPARENTS' flag 
            is given. Then that part of the name space must be created.
        @note:  if the target already exists, it will be overwritten if the 
            'OVERWRITE' flag is set, otherwise it is an 'AlreadyExists' 
            exception.
        @note: if a directory is to be copied recursively, but the target exists 
            and is not a directory, and not a link to a directory, an 
            'AlreadyExists' exception is raised even if the 'OVERWRITE' flag is 
            set.
        @note: if the instance points at an symbolic link, the source is deeply 
            dereferenced before copy. If derefencing is impossible (e.g. on a 
            broken link), an 'IncorrectState' exception is raised.
        @note: other flags are not allowed, and cause a 'BadParameter' exception.
        @note:  the default flags are 'NONE' (0).
        @note:  similar to 'cp' as defined by POSIX.
        """        
    
    def link_self(self, target, flags = Flags.NONE, tasktype=TaskType.NORMAL):
        """
        Create a symbolic link from the target entry to the source entry 
        (this entry) so that any reference to the target refers to the source 
        entry
        @summary: create a symbolic link
        @param target: name to link to
        @param flags: flags defining the operation modus
        @type target: L{URL}
        @type flags: int
        @type tasktype: int
        @param tasktype: return the normal return value or a Task object in a 
            final, RUNNING or NEW state. By default, tasktype is L{TaskType.NORMAL}
        @postcondition: - a symbolic link to the entry exists at target.
        @postcondition: 'Owner' of target is the id of the context use to 
            perform the opereration if target gets created.
        @permission: Query
        @permission: Exec for parent directory.
        @permission: Query for target.
        @permission: Query for target's parent directory.
        @permission: Exec for target's parent directory.
        @permission: Write for target if target does exist.
        @permission: Write for target's parent directory if target does not 
            exist.
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
        @note: if the target is a directory, the source entry is linked into 
            that directory
        @note: if the source is a directory, and the 'RECURSIVE' flag is set, 
            the source directory is recursively linked to the target (which must 
            be a directory as well - otherwise a 'BadParameter' exception is 
            raised). The method then behaves similar to lndir. If the 
            'RECURSIVE' flag is not set, the source entry itself is linked.
        @note: a 'BadParameter' exception is raised if the source is not a 
            directory and the 'RECURSIVE' flag is set.
        @note: if the target lies in a non-existing part of the name space, a 
            'DoesNotExist' exception is raised, unless the 'CREATEPARENTS' flag 
            is given - then that part of the name space must be created.
        @note: if the target already exists, it will be overwritten if the 
            'OVERWRITE' flag is set, otherwise it is an 'AlreadyExists' exception.
        @note: if a directory is to be moved, but the target exists and is not a 
            directory, and not a link to a directory, an 'AlreadyExists' 
            exception is raised even if the 'OVERWRITE' flag is set.
        @note: if the instance points at an symbolic link, the source is not 
            dereferenced before linking, unless the 'DEREFERENCE' flag is given. 
            If derefencing is impossible (e.g. on a broken link), an 
            'IncorrectState' exception is raised.
        @note: other flags are not allowed, and cause a 'BadParameter' exception.
        @note: the default flags are 'NONE' (0).
        @note: similar to 'ln' as defined by POSIX.
     
        """        

    def move_self(self, target, flags = Flags.NONE, tasktype=TaskType.NORMAL):
        """
        Rename source to target, or move source to target if target is a 
        directory.
        @summary: rename or move target
        @param target: name to move to
        @param flags: flags defining the operation modus
        @type target: L{URL}
        @type flags: int
        @type tasktype: int
        @param tasktype: return the normal return value or a Task object in a 
            final, RUNNING or NEW state. By default, tasktype is L{TaskType.NORMAL}
        @postcondition: an identical copy exists at target.
        @postcondition: the original entry is removed.
        @postcondition: 'Owner' of target is the id of the context use to 
            perform the opereration if target gets created.
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
        @note: if the target is a directory, the source entry is moved into that 
            directory
        @note: a 'BadParameter' exception is raised if the source is a directory 
            and the 'RECURSIVE' flag is not set.
        @note: a 'BadParameter' exception is raised if the source is not a 
            directory and the 'RECURSIVE' flag is set.
        @note: if the target lies in a non-existing part of the name space, a 
            'DoesNotExist' exception is raised, unless the 'CREATEPARENTS' flag 
            is given. Then that part of the name space must be created.
        @note: if the target already exists, it will be overwritten if the 
            'OVERWRITE' flag is set, otherwise it is an 'AlreadyExists' exception.
        @note: if the instance points at an symbolic link, the source is not 
            dereferenced before moving, unless the 'DEREFERENCE' flag is given. 
            If derefencing is impossible (e.g. on a broken link), an 
            'IncorrectState' exception is raised.
        @note: other flags are not allowed, and cause a 'BadParameter' exception.
        @note: the default flags are 'NONE' (0).
        @note: similar to 'mv' as defined by POSIX.
        
        """

    def remove_self(self, flags = Flags.NONE, tasktype=TaskType.NORMAL):
        """
        Removes this entry, and closes it
        @summary: removes this entry, and closes it
        @param flags: the operation modus
        @type flags: int
        @type tasktype: int
        @param tasktype: return the normal return value or a Task object in a 
            final, RUNNING or NEW state. By default, tasktype is L{TaskType.NORMAL}
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
        @note: a 'BadParameter' exception is raised if the source is a directory 
            and the 'RECURSIVE' flag is not set.
        @note: a 'BadParameter' exception is raised if the source is not a 
            directory and the 'RECURSIVE' flag is set.
        @note: the source will not be dereferenced unless the 'DEREFERENCE' flag 
            is given. If derefencing is impossible (e.g. on a broken link), an
            'IncorrectState' exception is raised.
        @note:  other flags are not allowed, and cause a 'BadParameter' exception.
        @note:  the default flags are 'NONE' (0).
        @note:  if the instance was not closed before, this call performs a 
            close() on the instance, and all notes to close() apply.
        @note:  similar to 'rm' as defined by POSIX.
        
        """   

    def close(self, timeout = 0.0, tasktype=TaskType.NORMAL):
        """
        Closes the NSEntry
        @summary: Closes the NSEntry
        @param timeout: seconds to wait
        @type timeout: float 
        @type tasktype: int
        @param tasktype: return the normal return value or a Task object in a 
            final, RUNNING or NEW state. By default, tasktype is L{TaskType.NORMAL}        
        @postcondition: the entry instance is closed.
        @raise NotImplemented:
        @raise IncorrectState:
        @raise NoSuccess:
        @note: any subsequent method call on the object raises an 
            'IncorrectState' exception (apart from __del__() and close()).
        @note:  close() can be called multiple times, with no side effects.
        @note:  if close() is implicitely called in __del__, it will never raise 
            an exception.
        @see: Section 2 of the GFD-R-P.90 document for resource deallocation 
            semantics and timeout semantics.
        
        """        

    def permissions_allow_self(self, id, perm, flags=Flags.NONE, tasktype=TaskType.NORMAL):
        """
        Enable a permission
        @summary: enable a permission
        @param id: id to set permission for
        @param perm: permission to enable
        @param flags: mode of operation
        @type id : string
        @type perm: value from L{Permission}
        @type flags: int
        @type tasktype: int
        @param tasktype: return the normal return value or a Task object in a 
            final, RUNNING or NEW state. By default, tasktype is L{TaskType.NORMAL}
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
        @note: all notes to permissions_allow from the 
            saga.permissions.Permission class apply.
        @note:  allowed flags are: 'RECURSIVE', 'DEREFERENCE'. All other flags 
            cause a 'BadParameter' exception.
        @note:  specifying 'RECURSIVE' for a non-directory causes a 
            'BadParameter' exception.

        """        

    def permissions_deny_self(self, id, perm, flags=Flags.NONE, tasktype=TaskType.NORMAL):
        """
        Disable a permission flag
        @summary: disable a permission flag
        @param id: id to set permission for
        @param perm: permission to disable
        @param flags: mode of operation
        @type id : string
        @type perm: value from L{Permission}
        @type flags: int
        @type tasktype: int
        @param tasktype: return the normal return value or a Task object in a 
            final, RUNNING or NEW state. By default, tasktype is L{TaskType.NORMAL}
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
        @note: all notes to permissions_deny from the 
            saga.permissions.Permission class apply.
        @note: allowed flags are: 'RECURSIVE', 'DEREFERENCE'. All other flags 
            cause a 'BadParameter' exception.
        @note: specifying 'RECURSIVE' for a non-directory causes a 
            'BadParameter' exception.

        """        
    
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
    
    def __init__(self, name, session=Session(), flags=Flags.NONE, tasktype=TaskType.NORMAL):
        """
        Initialize the object
        @summary: initialize the object
        @param name: initial working dir
        @param flags: open mode
        @param session: session handle for object creation
        @type name : L{URL}
        @type session: L{Session}
        @type flags: int
        @param tasktype: return a normal NSDirectory or a Task object that 
            creates a NSDirectory in a final, RUNNING or NEW state. By default, 
            type is L{TaskType.NORMAL}
        @type tasktype: int
        @postcondition: the directory is opened.
        @postcondition: 'Owner' of target is the id of the context use to 
            perform the opereration, if the directory gets created.
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
        @note:  the constructor performs an open of the entry - all notes to the 
            respective open call apply.
        @note:  the default flags are 'NONE' (0).

        """
   
    def change_dir(self, url, tasktype=TaskType.NORMAL):
        """
        Change the working directory
        @summary: change the working directory
        @param url : directory to change to
        @type url: L{URL}
        @param tasktype: return the normal return value or a Task object in a 
            final, RUNNING or NEW state. By default, tasktype is L{TaskType.NORMAL}
        @type tasktype: int
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
        @note:  if 'dir' can be parsed as URL, but contains an invalid directory 
            name, a 'BadParameter' exception is raised.
        @note:  if 'dir' does not exist, a 'DoesNotExist' exception is raised.
        @note:  similar to the 'cd' command in the POSIX shell.
       
        """
 
    def list(self, name_pattern = ".", flags = Flags.NONE, tasktype=TaskType.NORMAL):
        """
        List entries in this directory
        @summary: list entries in this directory
        @param flags: flags defining the operation modus
        @param name_pattern: name or pattern to list
        @type flags: int
        @type name_pattern: string
        @type tasktype: int
        @param tasktype: return the normal return value or a Task object in a 
            final, RUNNING or NEW state. By default, tasktype is L{TaskType.NORMAL}
        @rtype: list
        @return: list of names matching the name_pattern
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
        @note: if name_pattern is not given (i.e. is an empty string), all 
            entries in the current working directory are listed.
        @note: if name_pattern is given and points to a directory, the contents 
            of that directory are listed.
        @note: the name_pattern follows the standard POSIX  shell wildcard 
            specification, as described above.
        @note:  list does not follow symbolically linked directories, unless the 
            'DEREFERENCE' flag is specified - otherwise list lists symbolic link 
            entries with a matching name.
        @note:  if the 'DEREFERENCE' flag is set, list returns the name of link 
            targets, not of the link entry itself.
        @note:  the default flags are 'NONE' (0).
        @note:  other flags are not allowed, and cause a 'BadParameter' exception.
        @note:  if the name_pattern cannot be parsed, a 'BadParameter' exception 
            with a descriptive error message is raised.
        @note:  if the name_pattern does not match any entry, an empty list is 
            returned, but no exception is raised.
        @note:  similar to 'ls' as defined by POSIX.
        """        
 
    def find(self, name_pattern, flags = Flags.RECURSIVE, tasktype=TaskType.NORMAL):
        """
        Find entries in the current directory and below
        @summary: find entries in the current directory and below
        @param name_pattern: pattern for names of entries to be found
        @param flags: flags defining the operation modus
        @type name_pattern: string
        @type flags: int
        @type tasktype: int
        @param tasktype: return the normal return value or a Task object in a 
            final, RUNNING or NEW state. By default, tasktype is L{TaskType.NORMAL}
        @rtype: list 
        @return: list of names matching the name_pattern
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
        @note:  find operates recursively below the current working directory if 
            the 'RECURSIVE' flag is specified (default)
        @note:  find does not follow symbolically linked directories, unless the 
            'DEREFERENCE' flag is specified - otherwise find lists symbolic link 
            entries with a matching name.
        @note:  the default flags are 'RECURSIVE' (1).
        @note:  other flags are not allowed, and cause a 'BadParameter' exception.
        @note:  the name_pattern follows the standard POSIX shell wildcard 
            specification, as described above.
        @note:  the matching entries returned are path names relative to cwd.
        @note:  similar to 'find' as defined by POSIX, but limited to the -name 
            option.

        """
    
    def exists (self, name, tasktype=TaskType.NORMAL):
        """
        Checks if entry exists
        @summary: checks if entry exists
        @param name: name to be tested for existence
        @type name : L{URL}
        @type tasktype: int
        @param tasktype: return the normal return value or a Task object in a 
            final, RUNNING or NEW state. By default, tasktype is L{TaskType.NORMAL}
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
        @note:  if 'name' can be parsed as URL, but contains an invalid entry 
            name, an 'BadParameter' exception is raised.
        @note:  note that no exception is raised if the entry does not exist. 
            The method just returns 'False' in this case.
        @note:  similar to 'test -e' as defined by POSIX.

        """

    def is_dir(self, name, tasktype=TaskType.NORMAL):
        """
        Tests url for being a directory
        @summary: tests name for being a directory
        @param name: URL to be tested
        @type name: L{URL}
        @type tasktype: int
        @param tasktype: return the normal return value or a Task object in a 
            final, RUNNING or NEW state. By default, tasktype is L{TaskType.NORMAL}
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
        @note:  returns True if the instance represents a directory entry, False 
            otherwise
        @note:  all notes to the NSEntry.is_dir() method apply.
        @note:  if 'name' can be parsed as URL, but contains an invalid entry 
            name, an 'BadParameter' exception is raised.
        @note:  if 'name' is a valid entry name but the entry does not exist, a 
            'DoesNotExist' exception is raised.
        @note:  similar to 'test -d' as defined by POSIX.

        """

    def is_entry (self, name, tasktype=TaskType.NORMAL):
        """
        Tests name for being an NSEntry
        @summary: tests name for being an NSEntry
        @param name: name to be tested
        @type name: L{URL}
        @type tasktype: int
        @param tasktype: return the normal return value or a Task object in a 
            final, RUNNING or NEW state. By default, tasktype is L{TaskType.NORMAL}
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
        @note: if 'name' can be parsed as URL, but contains an invalid entry 
            name, a 'BadParameter' exception is raised.
        @note: if 'name' is a valid entry name but the entry does not exist, a 
            'DoesNotExist' exception is raised.
        @note:  similar to 'test -f' as defined by POSIX.

        """

    def is_link(self, name, tasktype=TaskType.NORMAL):
        """
        Tests name for being a symbolic link
        @summary: tests name for being a symbolic link
        @param name: name to be tested
        @type name: L{URL}
        @type tasktype: int
        @param tasktype: return the normal return value or a Task object in a 
            final, RUNNING or NEW state. By default, tasktype is L{TaskType.NORMAL}
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
        @note: if 'name' can be parsed as URL, but contains an invalid entry 
            name, a 'BadParameter' exception is raised.
        @note: if 'name' is a valid entry name but the entry does not exist, a 
            'DoesNotExist' exception is raised.
        @note: similar to 'test -L' as defined by POSIX.

        """
 
    def read_link(self, name, tasktype=TaskType.NORMAL):
        """
        Returns the name of the link target
        @summary: returns the name of the link target
        @param name: name to be resolved
        @type name: L{URL}
        @type tasktype: int
        @param tasktype: return the normal return value or a Task object in a 
            final, RUNNING or NEW state. By default, tasktype is L{TaskType.NORMAL}
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
        @note:  if 'name' can be parsed as URL, but contains an invalid entry 
            name, a 'BadParameter' exception is raised.
        @note:  if 'name' does not exist, a 'DoesNotExist' exception is raised.

        """

    def get_num_entries (self, tasktype=TaskType.NORMAL):
        """
        Gives the number of entries in the directory
        @summary:gives the number of entries in the directory
        @type tasktype: int
        @param tasktype: return the normal return value or a Task object in a 
            final, RUNNING or NEW state. By default, tasktype is L{TaskType.NORMAL}
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
        @note: at the time of using the result of this call, the actual number 
            of entries may already have changed (no locking is implied)
        @note: vaguely similar to 'opendir'/'readdir' (2) as defined by POSIX.

        """

    def get_entry(self, entry, tasktype=TaskType.NORMAL):
        """
        Gives the name of an entry in the directory based upon the enumeration 
        defined by get_num_entries
        @summary:gives the name of an entry in the directory
        @param entry: index of entry to get
        @type entry: int
        @type tasktype: int
        @param tasktype: return the normal return value or a Task object in a 
            final, RUNNING or NEW state. By default, tasktype is L{TaskType.NORMAL}
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
        @note: there is no sort order implied by the enumeration, however an 
            underlying implementation MAY choose to sort the entries
        @note:  subsequent calls to get_entry and/or get_num_entries may return 
            inconsistent data, i.e. no locking or state tracking is implied. In 
            particular, an index may be invalid - a 'DoesNotExist' exception is 
            then raised (not a 'BadParameter' exception).
        @note:  vaguely similar to 'opendir'/'readdir' (2) as defined by POSIX.

        """       

    def copy(self, source, target, flags=Flags.NONE, tasktype=TaskType.NORMAL):
        """
        Copy the entry to another part of the name space
        @summary: copy the entry to another part of the name space
        @param source: name to copy
        @param target: name to copy to
        @param flags: flags defining the operation modus
        @type source: L{URL} or string
        @type target: L{URL}
        @type flags: int
        @type tasktype: int
        @param tasktype: return the normal return value or a Task object in a 
            final, RUNNING or NEW state. By default, tasktype is L{TaskType.NORMAL}
        @postcondition: an identical copy of source exists at target.
        @postcondition:  'Owner' of target is the id of the context used to 
            perform the opereration if target gets created.
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
        @note: if 'name' can be parsed as URL, but contains an invalid entry 
            name, a 'BadParameter' exception is raised.
        @note: if 'name' is a valid entry name but the entry does not exist, a 
            'DoesNotExist' exception is raised.
        @note: when 'source' as a string is given as parameter, it can contain 
            wildcards.
        @note:  on error conditions on any of the expanded
           list of source entries, the respective error is raised. The state of 
           the operations on the other elements of the expanded entry list is 
           undefined.
        @note:  if source expands to multiple entries, then the target URL 
            specifies a directory, otherwise a 'BadParameter' exception is 
            raised.

        """

    def link(self, source, target, flags = Flags.NONE, tasktype=TaskType.NORMAL):
        """
        Create a symbolic link from the target entry to the source entry so that 
        any reference to the target refers to the source entry
        @summary: create a symbolic link from the target entry.
        @param source: name of link
        @param target: name to link to
        @param flags: flags defining the operation modus
        @type source:  L{URL} or string
        @type target: L{URL}
        @type flags: int
        @type tasktype: int
        @param tasktype: return the normal return value or a Task object in a 
            final, RUNNING or NEW state. By default, tasktype is L{TaskType.NORMAL}
        @postcondition: - a symbolic link to source exists at target.
        @postcondition:  'Owner' of target is the id of the context used to 
            perform the opereration if target gets created.
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
        @note:  if the 'RECURSIVE' flag is defined, the source is recursively 
            linked if it is a directory; otherwise this flag is ignored.
        @note:  if the 'DEREFERENCE' flag is specified, the method applies to 
            the link target of source. The flag causes a 'BadParameter' 
            exception if source is not a link.
        @note:  if the the target already exists, the 'OVERWRITE' flag must be 
            specified, otherwise an 'AlreadyExists' exception is raised.
        @note:  the default flags are 'NONE' (0).
        @note:  other flags are not allowed on this method, and cause a 
            'BadParameter' exception.
        @note:  if 'source' can be parsed as URL, but contains an invalid entry 
            name, a 'BadParameter' exception is raised.
        @note: if 'source' is a valid entry name but the entry does not exist, a 
            'DoesNotExist' exception is raised.
        @note: the 'source' parameter as string can contain wildcards.
        @note: on error conditions on any of the expanded list of source entries, 
            the respective error is raised - the state of the operations on the 
            other elements of the expanded entry list is undefined.
        @note: if source expands to multiple entries, then the
             target URL specifies a directory. Otherwise a 'BadParameter' 
             exception is raised.

        """

    def move (self, source, target, flags = Flags.NONE, tasktype=TaskType.NORMAL):
        """
        Rename source to target, or move source to target if target is a 
        directory.
        @summary: rename or move target
        @param source: name to move
        @param target: name to move to
        @param flags: flags defining the operation modus
        @type source: L{URL} or string
        @type target: L{URL}
        @type flags: int
        @type tasktype: int
        @param tasktype: return the normal return value or a Task object in a 
            final, RUNNING or NEW state. By default, tasktype is L{TaskType.NORMAL}
        @postcondition: an identical copy of source exists at target.
        @postcondition: source is removed.
        @postcondition: 'Owner' of target is the id of the context used to 
            perform the opereration if target gets created.
        @permission: Query for source.
        @permission: Write for source.
        @permission: Exec for source's parent directory.
        @permission: Write for source's parent directory.
        @permission: Query for target.
        @permission: Exec for target's parent directory.
        @permission: Write for target if target does exist.
        @permission: Write for target's parent directory if target does not 
            exist.
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
        @note:  if the 'RECURSIVE' flag is defined, the source is recursively 
            copied if it is a directory; otherwise this flag is ignored.
        @note:  if the 'DEREFERENCE' flag is specified, the method applies to 
            the link target of source. The flag causes a 'BadParameter' 
            exception if source is not a link.
        @note:  if the the target already exists, the 'OVERWRITE' flag must be 
            specified, otherwise an 'AlreadyExists' exception is raised.
        @note:  the default flags are 'NONE' (0).
        @note:  other flags are not allowed on this method, and cause a 
            'BadParameter' exception.
        @note:  if 'source' can be parsed as URL, but contains an invalid entry 
            name, a 'BadParameter' exception is raised.
        @note:  if 'source' is a valid entry name but the entry does not exist, 
            a 'DoesNotExist' exception is raised.
        @note:  moving any parent or the current directoy (e.g. '.', '..' etc.) 
            is not allowed, and raises a 'BadParameter' exception
        @note:  the 'source' string as parameter can contain wildcards.
        @note:  on error conditions on any of the expanded list of source 
            entries, the respective error is raised - the state of the 
            operations on the other elements of the expanded entry list is 
            undefined.
        @note:  if source expands to multiple entries, then the target URL 
            specifies a directory - otherwise a 'BadParameter' exception is 
            raised.
        """

    def remove(self, target, flags =  Flags.NONE, tasktype=TaskType.NORMAL):
        """
        Removes the entry
        @summary: removes the entry
        @param target: entry to be removed
        @param flags: defining the operation modus
        @type target: L{URL} or string
        @type flags: int
        @type tasktype: int
        @param tasktype: return the normal return value or a Task object in a 
            final, RUNNING or NEW state. By default, tasktype is L{TaskType.NORMAL}
        @postcondition: target is removed.
        @postcondition: target is closed if it refers to the cwd.
        @permission: Query for target.
        @permission: Write for target.
        @permission: Exec for target's parent directory.
        @permission: Write for target's parent directory.
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
        @note: if the 'RECURSIVE' flag is defined, the target is recursively 
            removed if it is a directory; otherwise this flag is ignored.
        @note:  if the 'DEREFERENCE' flag is specified, the method applies to 
            the link target of target. The flag causes a 'BadParameter' 
            exception if target is not a link.
        @note:  the default flags are 'NONE' (0).
        @note:  other flags are not allowed on this method, and cause a 
            'BadParameter' exception.
        @note:  if 'target' can be parsed as URL, but contains an invalid entry 
            name, a 'BadParameter' exception is raised.
        @note:  if 'target' is a valid entry name but the entry does not exist, 
            a 'DoesNotExist' exception is raised.
        @note:  removing any parent or the current directoy (e.g. '.', '..' etc.) 
            is not allowed, and raises a 'BadParameter' exception
        @note:  the 'target' string as parameter can contain wildcards
        @note:  on error conditions on any of the expanded list of target 
            entries, the respective error is raised - the state of the 
            operations on the other elements of the expanded entry list is 
            undefined.

        """

    def make_dir(self, target, flags = Flags.NONE, tasktype=TaskType.NORMAL):
        """
        Creates a new directory
        @summary: creates a new directory
        @param target: directory to create
        @param flags: flags defining the operation modus
        @type target: L{URL}
        @type flags: int
        @type tasktype: int
        @param tasktype: return the normal return value or a Task object in a 
            final, RUNNING or NEW state. By default, tasktype is L{TaskType.NORMAL}
        @postcondition: 'Owner' of target is the id of the context used to 
            perform the opereration if target gets created.
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
        @note:  if the parent directory or directories do not exist, the 
            'CREATEPARENTS' flag must be set or a 'DoesNotExist' exception is 
            raised. If set, the parent directories are created as well.
        @note:  an 'AlreadyExists' exception is raised if the directory already 
            exists and the 'EXCLUSIVE' flag is given.
        @note:  the default flags are 'NONE' (0).
        @note:  other flags are not allowed on this method, and cause a 
            'BadParameter' exception.
        @note:  if 'target' can be parsed as URL, but contains an invalid entry 
            name, a 'BadParameter' exception is raised.
        @note:  similar to 'mkdir' (2) as defined by POSIX.
        
        """

    def open(self, name, flags = Flags.NONE, tasktype=TaskType.NORMAL):
        """
        Creates a new NSEntry instance
        @summary: creates a new NSEntry instance
        @param name:  entry to open
        @param flags: flags defining the operation modus
        @type name: L{URL}
        @type flags: int
        @type tasktype: int
        @param tasktype: return the normal return value or a Task object in a 
            final, RUNNING or NEW state. By default, tasktype is L{TaskType.NORMAL}
        @rtype: L{NSEntry}
        @return: opened entry instance
        @postcondition: the session of the returned instance is that of the 
            calling instance.
        @postcondition: 'Owner' of name is the id of the context used to perform 
            the opereration if name gets created.
        @postcondition: the namespace entry is created if it does not yet exist, 
            and the CREATE flag is specified.
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
        @note: a 'BadParameter' exception is raised if 'name' points to a 
            directory, or is an invalid entry name.
        @note: a 'DoesNotExist' exception is raised if 'name' does not exist, 
            and the 'CREATE' flag is not given.
        @note: a 'AlreadyExists' exception is raised if 'name' does exist, and 
            the 'CREATE' and 'EXCLUSIVE' flags are given.
        @note: 'name' is always deeply dereferenced, the cwd, however, is not 
            changed to the link targets cwd.
        @note: parent directories are created on the fly if the 'CREATEPARENTS' 
            and 'CREATE' flag are both given, if they don't exist.
        @note: the entry is locked on open if the 'LOCK' flag is given. If the 
            entry is already in a locked state, the open will fail and a 
            descriptive error will be issued. If a entry is opened in locked 
            mode, any other open on that entry fails with a 'NoSuccess' 
            exception if the 'LOCK' flag is given. Note that a entry can be 
            opened in unlocked mode, and then in locked mode, without an error 
            getting raised. The application programmer must take precautions
            to avoid such situations. The lock will get removed on destruction 
            of the entry object, and also on close. If an implementation does 
            not support locking, a descriptive 'BadParameter' exception is 
            raised if the 'LOCK' flag is given. Read-locks and Write-locks are 
            not distinguished.
        @note: the default flags are 'NONE' (0).
        @note: other flags are not allowed on this method, and cause a 
            'BadParameter' exception.
        @note: similar to 'open' (2) as defined by POSIX.

        """        

    def open_dir(self, name, flags = Flags.NONE, tasktype=TaskType.NORMAL):
        """
        Creates a new NSDirectory instance
        @summary: creates a new NSDirectory instance
        @param name: directory to open
        @param flags: flags defining the operation modus
        @type name: L{URL}
        @type flags: int 
        @type tasktype: int 
        @param tasktype: return the normal return value or a Task object in a 
            final, RUNNING or NEW state. By default, tasktype is L{TaskType.NORMAL}
        @rtype: L{NSDirectory}
        @return: opened directory instance
        @postcondition: - the session of the returned instance is that of the 
            calling instance.
        @postcondition: 'Owner' of name is the id of the context used to perform 
            the opereration if name gets created.
        @postcondition: the namespace directory is created if it does not yet 
            exist, and the Create is set.
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
        @note: a 'DoesNotExist' exception is raised if 'name' does not exist and 
            the 'CREATE' flag is not given.
        @note: a 'AlreadyExist' exception is raised if 'name' does exist and the 
            'CREATE' flag and the 'EXCLUSIVE' flag are given.
        @note: no exception is raised if 'name' does exist and the 'CREATE' 
            flag is given, and the 'EXCLUSIVE' flag is not given.
        @note: if the 'CREATE' flag is given, all notes to the 
            NSDirectory.make_dir() method apply.
        @note: the default flags are 'NONE' (0).
        @note: other flags are not allowed on this method, and cause a 
            'BadParameter' exception.
        @note: 'name' is always deeply dereferenced, however, the cwd is still 
            set to 'name', and not to the value of the link target.
        @note: parent directories are created on the fly if the 'CREATEPARENTS' 
            and 'CREATE' flag are both given, if they don't exist.
        @note: if 'name' can be parsed as URL, but contains an invalid directory 
            name, a 'BadParameter' exception is raised

        """

    def permissions_allow(self, target, id, perm, flags = Flags.NONE, tasktype=TaskType.NORMAL):
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
        @type tasktype: int
        @param tasktype: return the normal return value or a Task object in a 
            final, RUNNING or NEW state. By default, tasktype is L{TaskType.NORMAL}
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
        @note:  all notes to permissions_allow from the 
            saga.permissions.Permission class apply.
        @note:  allowed flags are: 'RECURSIVE', 'DEREFERENCE'. All other flags 
            cause a 'BadParameter' exception.
        @note:  specifying 'RECURSIVE' for a non-directory causes a 
            'BadParameter' exception.
        @note: the 'target' string can contain wildcards.
        @note: on error conditions on any of the expanded list of target 
            entries, the respective error is raised - the state of the 
            operations on the other elements of the expanded entry list is
            undefined. 
        """ 

    def permissions_deny(self, target, id, perm, flags = Flags.NONE, tasktype=TaskType.NORMAL):
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
        @type tasktype: int
        @param tasktype: return the normal return value or a Task object in a 
            final, RUNNING or NEW state. By default, tasktype is L{TaskType.NORMAL}
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
        @note: all notes to permissions_deny from the 
            saga.permissions.Permissions class apply.
        @note: allowed flags are: 'RECURSIVE', 'DEREFERENCE'. All other flags 
            cause a 'BadParameter' exception.
        @note: specifying 'RECURSIVE' for a non-directory causes a 
            'BadParameter' exception.
        @note: the 'target' string can contain wildcards.
        @note: on error conditions on any of the expanded list of source 
            entries, the respective error is raised - the state of the 
            operations on the other elements of the expanded entry list is
            undefined. 
        """       
