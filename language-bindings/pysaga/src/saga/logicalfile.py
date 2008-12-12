# Package: saga
# Module: logicalfile 
# Description: The module which specifies classes which interact with replica
#    systems
# Specification and documentation can be found in section 4.4, page 266-280 of 
#    the GFD-R-P.90 document
# Author: P.F.A. van Zoolingen, Computer Systems Section, Faculty of 
#    Exact Science (FEW), Vrije Universiteit, Amsterdam, The Netherlands.

from object import Object, ObjectType
from attributes import Attributes
from namespace import NSEntry, NSDirectory
from url import URL
from session import Session
from task import Async, TaskType, Task

class Flags(object):
    """
    The Flags class is inherited from the namespace package. A number of replica 
    specific flags are added to it. All added flags are used for the opening of 
    logical file and logical directory instances, and are not applicable to the 
    operations inherited from the namespace package.

    """
    NONE = 0
    """ 
    @summary: indicates the absence of flags, and thus also implies that the 
        default flags for an operation do not apply, either
    """

    OVERWRITE = 1
    """
    @summary: enforces an operation which creates a new namespace entry to 
        continue even if the target entry does already exist. If that flag is 
        not given, an 'AlreadyExists' exception would result from such an 
        operation
    """

    RECURSIVE = 2
    """
    @summary: enforces an operation to apply recursively on a directory tree. 
        If that flag is not given, the same operation would only apply to the 
        given directory, and not to its children
    """
        
    DEREFERENCE = 4
    """
    @summary: enforces an operation to apply not to the entry pointed to by the 
        target name, but to the link target of that entry. If that flag is not 
        given, the same operation would apply to the entry directly, and its 
        link target stays unaffected
    """

    CREATE = 8
    """
    @summary: allows a namespace entry to be created while opening it, if it 
        does not already exist. If that flag is not given, the same open 
        operation would cause a 'DoesNotExist' exception. If the entry exists, 
        the flag is ignored.
    """
        
    EXCLUSIVE = 16
    """
    @summary: implies a modification to the meaning of the 'CREATE' flag: if 
        the entry already exists, the 'CREATE' flag is is no longer silently 
        ignored, but causes an 'AlreadyExists' exception.
    """
        
    LOCK = 32
    """
    @summary: enforces a lock on the name space entry when it is opened. Locks 
        are advisory in SAGA, semantic details for locking are defined in the 
        description of the open() call
    """
    
    CREATEPARENTS = 64
    """
    @summary: An operation which would create a name space entry would normally
        fail if any path element in the targets name does not yet exist. If this
        flag is given, such an operation would not fail, but would imply that 
        the missing path elements are created on the fly.
    """
    
    # 128    #reserved for Truncate
    # 256    #reserved for Append  
    READ = 512
    """
    @summary: The logical file or directory is opened for reading - that does 
        not imply the ability to change the logical file or directory.
    """
    
    WRITE = 1024
    """
    @summary: The logical file or directory is opened for writing - that does 
        not imply the ability to read from the logical file or directory.

    """
    READWRITE = 1536 
    """
    @summary: The logical file or directory is opened for reading and writing.
    """
    # 2048  #   reserved for Binary

class LogicalFile(NSEntry, Attributes, Async):
    """
    This class provides the means to handle the contents of logical files. 
    These contents consists of strings representing locations of physical files 
    (replicas) associated with the logical file.
    @summary: This class provides the means to handle the contents of logical 
        files
    """
      
    def __init__(self, name, session=Session(), 
                 flags=Flags.READ, tasktype=TaskType.NORMAL):
        """
        Initialize the object.
        @summary: Initialize the object.
        @param session: session to associate with the object
        @type session: L{Session}
        @param name: location of file
        @type name: L{URL}
        @param flags: mode for opening
        @type flags: int
        @param tasktype: return a normal LogicalFile object or or return a Task 
            object that creates the LogicalFile object in a final, RUNNING or 
            NEW state. By default, tasktype is L{TaskType.NORMAL}
        @type tasktype: value from L{TaskType}
        @postcondition: the logical file is opened.
        @postcondition: 'Owner' of target is the id of the context use to 
            perform the opereration, if the logical file gets created.
        @Permission: Exec for parent directory.
        @Permission: Write for parent directory if Create is set.
        @Permission: Write for name if Write is set.
        @Permission: Read for name if Read is set.
        @raise NotImplemented:
        @raise IncorrectURL:
        @raise BadParameter:
        @raise AlreadyExists:
        @raise DoesNotExist:
        @raise PermissionDenied:
        @raise AuthorizationFailed:
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @Note: the semantics of the inherited constructors and of the 
            LogicalDirectory.open() method apply.
        @note: the default flags are 'Read' (512)
        """

        
    def add_location(self, name, tasktype=TaskType.NORMAL):
        """
        Add a replica location to the replica set
        @summary: Add a replica location to the replica set
        @param name: location to add to set
        @type name: L{URL}
        @param tasktype: return the normal return values or a Task object in a 
            final, RUNNING or NEW state. By default, tasktype 
            is L{TaskType.NORMAL}
        @type tasktype: value from L{TaskType}
        @PostCondition: name is in the list of replica locations for the 
            LogicalFile.
        @Permission: Write
        @raise NotImplemented:
        @raise IncorrectURL:
        @raise BadParameter:
        @raise IncorrectState:
        @raise PermissionDenied:
        @raise AuthorizationFailed:
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @note: this methods adds a given replica location (name) to the set of 
            locations associated with the LogicalFile.
        @note: the implementation MAY choose to interpret the replica locations 
            associated with the LogicalFile. It MAY return an 'IncorrectURL' 
            error indicating an invalid location if it is unable or unwilling 
            to handle that specific locations scheme. The implementation 
            documentation specifies how valid replica locations are formed.
        @note: if 'name' can be parsed as URL, but contains an invalid entry 
            name, a 'BadParameter' is raised
        @note: if the replica is already in the set, this method does nothing, 
            and in particular does not raise an 'AlreadyExists' exception
        @note: if the LogicalFile was opened ReadOnly, a 'PermissionDenied' 
            exception is raised.
        """
        
    def remove_location(self, name, tasktype=TaskType.NORMAL):
        """
        Remove a replica location from the replica set
        @summary: Remove a replica location from the replica set
        @param name: replica to remove from set
        @type name: L{URL}
        @param tasktype: return the normal return values or a Task object in a 
            final, RUNNING or NEW state. By default, tasktype 
            is L{TaskType.NORMAL}
        @type tasktype: value from L{TaskType}
        @postCondition: name is not anymore in list of replica locations for the 
            LogicalFile.
        @Permission: Write
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
        @note: this method removes a given replica location from the set of 
            replicas associated with the LogicalFile.
        @note: the implementation MAY choose to interpret the replica locations 
            associated with the LogicalFile. It MAY return an 'IncorrectURL' 
            error indicating an invalid location if it is unable or unwilling 
            to handle that specific locations scheme. The implementation 
            documentation specifies how valid replica locations are formed.
        @note: if 'name' can be parsed as URL, but contains an invalid entry 
            name, a 'BadParameter' exception is raised.
        @note: if the location is not in the set of replicas, a 'DoesNotExist' 
            exception is raised.
        @note: if the set of locations is empty after this operation, the 
            LogicalFile object is still a valid object (see replicate() method
            description).
        @note: if the LogicalFile was opened ReadOnly, a 'PermissionDenied' 
            exception is raised.
        """

    def update_location(self, old, new, tasktype=TaskType.NORMAL):
        """
        Change a replica location in replica set.
        @summary: Change a replica location in replica set.
        @param old: replica to be updated
        @type old: L{URL}
        @param new: update of replica
        @type new: L{URL}
        @param tasktype: return the normal return values or a Task object in a 
            final, RUNNING or NEW state. By default, tasktype 
            is L{TaskType.NORMAL}
        @type tasktype: value from L{TaskType}
        @PostCondition: old is not anymore in list of replica locations for the 
            LogicalFile.
        @PostCondition: new is in the list of replica locations for the logical 
            file.
        @Permission: Read
        @Permission: Write
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
        @note: this method removes a given replica location from the set of 
            locations associated with the LogicalFile, and adds a new location.
        @note: the implementation MAY choose to interpret the replica locations 
            associated with the LogicalFile. It MAY raise an 'IncorrectURL' 
            error indicating an invalid location if it is unable or unwilling 
            to handle that specific locations scheme. The implementation 
            documentation specifies how valid replica locations are formed.
        @note: if old or new can be parsed as URL, but contains an invalid entry 
            name, a 'BadParameter' exception is thrown.
        @note: if the old replica location is not in the set of locations, a 
            'DoesNotExist' exception is raised.
        @note: if the new replica location is already in the set of locations, 
            an 'AlreadyExists' exception is raised.
        @note: if the LogicalFile was opened ReadOnly, an 'PermissionDenied' 
            exception is raised.
        @note: if the LogicalFile was opened WriteOnly, an 'PermissionDenied' 
            exception is thrown.
        """
                  
    def list_locations(self, tasktype=TaskType.NORMAL):
        """
        List the locations in the location set
        @summary: List the locations in the location set
        @return: List of locations in set
        @rtype: list of L{URL}s
        @param tasktype: return the normal return values or a Task object in a 
            final, RUNNING or NEW state. By default, tasktype 
            is L{TaskType.NORMAL}
        @type tasktype: value from L{TaskType} 
        @Permission: Read
        @raise NotImplemented:
        @raise IncorrectState:
        @raise PermissionDenied:
        @raise AuthorizationFailed:
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @note: this method returns an list of URLs containing the complete set 
            of locations associated with the LogicalFile.
        @note: an empty array returned is not an error. The LogicalFile object 
            is still a valid object (see replicate() method description).
        @note: if the LogicalFile was opened WriteOnly, an 'PermissionDenied' 
            exception is thrown.
        """
    
    locations = property(list_locations, doc="""Locations in the location set 
                                            \n@type: list of URLs""")    
        
    def replicate(self, name, flags = Flags.NONE, tasktype=TaskType.NORMAL):
        """
        Replicate a file from any of the known replica locations to a new 
        location, and, on success, add the new replica location to the
        set of associated replicas
        @summary: Replicate a file from a location to a new location.
        @param name: location to replicate to
        @type name: L{URL}
        @param flags: flags defining the operation modus
        @param tasktype: return the normal return values or a Task object in a 
            final, RUNNING or NEW state. By default, tasktype 
            is L{TaskType.NORMAL}
        @type tasktype: value from L{TaskType}
        @PostCondition: an identical copy of one of the available replicas 
            exists at name.
        @PostCondition: name is in the list of replica locations for the 
            LogicalFile.
        @Permission: Read
        @Permission: Write
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
        @note: the method implies a two step operation: 1) create a new and 
            comlete replica at the given location, which then represents a new 
            replica location. 2) perform an add_location() for the new replica 
            location.
        @note: all notes to the saga.namespace.NSEntry.copy() and 
            saga.logicalfile.logicalFile.add_location() methods apply.
        @note: the method is not required to be atomic, but: the implementation 
            is either successfull in both steps, or throw an exception 
            indicating if both methods failed, or if one of the methods 
            succeeded.
        @note: a replicate call on an instance with empty location set raises 
            an 'IncorrectState' exception, with an descriptive error message.
        @note: the default flags are 'NONE' (0). The interpretation of flags is 
            as described for the NSEntry.copy() method.
        @note: The 'Recursive' flag is not allowed, and causes a 'BadParameter' 
            exception.
        @note: if the LogicalFile was opened ReadOnly, an 'PermissionDenied' 
            exception is thrown.
        @note: if the LogicalFile was opened WriteOnly, an 'PermissionDenied' 
            exception is thrown.
    """
    
class LogicalDirectory(NSDirectory, Attributes, Async):
    """
    This class represents a container for logical files in a logical file 
    namespace. It allows traversal of the catalog's name space, and the 
    manipulation and creation (open) of logical files in that name space.
    @summary:  This class represents a container for logical files in a logical 
        file namespace.
    """
   
    def __init__(self, name, session=Session(), 
                 flags=Flags.READ, tasktype=TaskType.NORMAL):
        """
        Initialize the object.
        @summary: Initialize the object.
        @param session: session to associate with the object
        @type session: L{Session}
        @param name: location of directory
        @type name: L{URL}
        @param flags: mode for opening
        @type flags: int
        @param tasktype: return a normal LogicalDirectory object or or return a 
            Task object that creates the LogicalDirectory object in a final, 
            RUNNING or NEW state. By default, tasktype is L{TaskType.NORMAL}
        @type tasktype: value from L{TaskType}
        @postcondition: the logical_directory is opened.
        @postcondition: 'Owner' of target is the id of the context use to 
            perform the opereration, if the LogicalDirectory gets created.
        @permission: Exec for parent directory.
        @permission: Write for parent directory if Create is set.
        @permission: Write for name if Write is set.
        @permission: Read for name if Read is set.
        @raise NotImplemented:
        @raise IncorrectURL:
        @raise BadParameter:
        @raise AlreadyExists:
        @raise DoesNotExist:
        @raise PermissionDenied:
        @raise AuthorizationFailed:
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @Note: the semantics of the inherited constructors and of the 
            LogicalDirectory.open_dir() method apply.
        @note: the default flags are 'READ' (512).
        """

    def is_file(self, name, tasktype=TaskType.NORMAL):
        """
        Alias for L{NSDirectory.is_entry()}
        """
        
        
    def open_dir(self, name, flags = Flags.READ, tasktype=TaskType.NORMAL):
        """
        Creates a new LogicalDirectory instance
        @summary: Creates a new LogicalDirectory instance
        @param name: name of directory to open
        @param flags: flags defining operation modus
        @type name: L{URL}
        @type flags: int  
        @param tasktype: return the normal return values or a Task object in a 
            final, RUNNING or NEW state. By default, tasktype 
            is L{TaskType.NORMAL}
        @type tasktype: value from L{TaskType}
        @return: opened directory instance
        @rtype: L{LogicalDirectory}
        @postcondition: the session of the returned instance is that of
            the calling instance.
        @postcondition: 'Owner' of name is the id of the context used to perform 
            the opereration if name gets created.
        @permission: Exec for name's parent directory.
        @permission: Write for name's parent directory if Create is set.
        @permission: Write for name if Write is set.
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
        @Note: all notes from the NSDirectory.open_dir() method apply.
        @note: default flags are 'READ' (512).
        """
    
    def open(self, name, flags = Flags.READ, tasktype=TaskType.NORMAL):
        """
        Create a new LogicalFile instance.
        @summary: Create a new LogicalFile instance.
        @param name: file to be opened
        @param flags: flags defining operation modus
        @type name: L{URL}
        @type flags: int
        @param tasktype: return the normal return values or a Task object in a 
            final, RUNNING or NEW state. By default, tasktype 
            is L{TaskType.NORMAL}
        @type tasktype: value from L{TaskType}
        @return: opened file instance
        @rtype: L{LogicalFile}
        @postcondition: the session of the returned instance is that of the 
            calling instance.
        @postcondition: 'Owner' of name is the id of the context used to perform 
            the opereration if name gets created.
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
        @Note: all notes from the NSDirectory.open() method apply.
        @note: the flag set 'Read | Write' is equivalent to the flag 
            'ReadWrite'.
        @note: default flags are 'READ' (512).
        """
    
    def find(self, name_pattern, attr_pattern, 
             flags=Flags.RECURSIVE, tasktype=TaskType.NORMAL):
        """
        Find entries in the current directory and below, with matching names 
        and matching meta data
        @summary: Find entries in directory and below.
        @type name_pattern: string
        @type attr_pattern: list of strings 
        @type flags: int 
        @param name_pattern: pattern for names of entries to be found
        @param attr_pattern: pattern for meta data key/values of entries to be
            found
        @param flags: flags defining the operation modus
        @param tasktype: return the normal return values or a Task object in a 
            final, RUNNING or NEW state. By default, tasktype 
            is L{TaskType.NORMAL}
        @type tasktype: value from L{TaskType}
        @return: list of names matching both pattern
        @rtype: list of L{URL}s
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
        @Note: the description of find() in the Introduction of 
            section 4.4 of GFD-R-P.90, applies.
        @note: the semantics for both the find_attributes() method in the 
            saga.attributes.Attributes class and for the find() method in the
            saga.namespace.NSDirectory class apply. On conflicts, the find() 
            semantic supersedes the find_attributes() semantic. Only entries
            matching all attribute patterns and the name space pattern are 
            returned.
        @note: the default flags are 'RECURSIVE' (2).
        """  
   