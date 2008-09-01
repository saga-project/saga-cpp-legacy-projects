#For File
#package saga.file

from namespace import  NSEntry, NSDirectory
from task import Async
from permissions import  Permissions
from buffer import Buffer
from object import Object, ObjectType
from attributes import Attributes
from error import NotImplemented

#import org.ogf.saga.url.URLFactory;
#import org.ogf.saga.url.URL;
#import org.ogf.saga.namespace.Flags;
#import org.ogf.saga.file.File;
#import org.ogf.saga.file.FileFactory;
#import java.lang.Exception;

class Flags(object):
    """
    
    The flags describe the properties of several operations on file and directory instances.

    """
    NONE          = 0 
    OVERWRITE     = 1
    RECURSIVE     = 2
    DEREFERENCE   = 4
    CREATE        = 8
    EXCLUSIVE     = 16
    LOCK          = 32
    CREATEPARENTS = 64
    TRUNCATE      = 128
    APPEND        = 256
    READ          = 512
    WRITE         = 1024
    READWRITE     = 1536 
    BINARY        = 2048

class SeekMode(object):
    START   = 1        # Python uses start=0,current=1, 2=end
    CURRENT = 2
    END     = 3

class Iovec(Buffer, Object):
    """
    The iovec class inherits the Buffer class, and three additional state
    attributes: offset, len in and len out (with the latter one being read-only).
    With that addition, the new class can be used very much the same way as the
    iovec structure defined by POSIX for readv/writev, with the buffer len in
    beeing interpreted as the POSIX iov len, i.e. the number of bytes to read/write.

    """

    def __init__(self, size = -1, data = "", offset = 0, len_in = -1):
        #in array<byte> data = "", in int size = 0, in int offset = 0, in int len_in = size, out buffer obj
        """
        Create an iovec instance
        size: size of data to be used
        data: data to be used
        offset: offset for I/O operation
        len_in: number of units to read or write on read_v/write_v. default -1 means that len_in is size
        Raises:   BadParameter
                  NoSuccess
        Notes:    - all notes from the buffer CONSTRUCTOR apply.
                  - if len_in is larger than size, and size is not given as -1, a BadParameter exception is thrown.
        """
        pass

    def set_offset(self, offset):
        #in int offset
        """
        Set offset
        offset: value for offset
        Raises: BadParameter
        Notes: if offset is smaller that zero, a BadParameter exception is thrown.

        """

    def get_offset (self):
        #out int offset);
        """
        Retrieve the current value for offset
        offset: value of offset
        """
        offset = 0
        return offset
        
    def set_len_in(self, len_in):
        #set_len_in (in int len_in);
        """
        Set len_in
        len_in:               value for len_in
        Raises: BadParameter
        Note: if len_in is larger than size, and size is not set to -1, a BadParameter exception is thrown.
        """

    def get_len_in(self):
        #get_len_in (out int len_in);
        """
        Retrieve the current value for len_in
        len_in: value of len_in
        """
        len_in = 0
        return len_in

    def get_len_out(self):
        #get_len_out (out int len_out);
        """
        Retrieve the value for len_out
        len_out: value of len_out
        Notes: - len_out reports the number of bytes read or written in a completed read_w or write_w operation.
          - before completion of the operation, the returned value is -1.
          - for implementation managed memory, the value of len_out is always the same as for size.
        """
        len_out = 0
        return len_out

class File(NSEntry):
    fileObject = None
    
    def __init__(self, session, url_name, flags=Flags.READ):
        """
        Create the File object
        @summary: Create the File object
        @param session:  session to associate the object with
        @paramname: location of file
        @param flags: mode for opening
        @postcondition: the file is opened.
        @postcondition: Owner of target is the id of the context use to perform the opereration, if the file gets created.
        @permission: Exec for parent directory.
                    Write for parent directory if Create is set.
                    Write for name if Write is set.
                    Read for name if Read is set.
        @raise NotImplemented:
        @raise IncorrectURL
                BadParameter
                AlreadyExists
                DoesNotExist
                PermissionDenied
                AuthorizationFailed
                AuthenticationFailed
                Timeout
                NoSuccess
        @notes:   - all notes from the Directory.open() method apply.
                  - the default flags are READ (512).

        """
        super(File,self).__init__()
        # todo: check types
#        self.fileObject = org.ogf.saga.file.FileFactory.createFile( url_name.urlObject, org.ogf.saga.namespace.Flags.READ.getValue() );
        #print "sagaFile.File object created"
 
#   def __convertException(self, e):
#        if isinstance(e, org.ogf.saga.error.AlreadyExistsException):
#            print "org.ogf.saga.error.AlreadyExistsException: " + e.getMessage()
#        elif isinstance(e, org.ogf.saga.error.AuthenticationFailedException):
#            print "org.ogf.saga.error.AuthenticationFailedException: " + e.getMessage()
#        elif isinstance(e, org.ogf.saga.error.AuthorizationFailedException):
#            print "org.ogf.saga.error.AuthorizationFailedException: " + e.getMessage()
#        elif isinstance(e, org.ogf.saga.error.BadParameterException):
#            print "org.ogf.saga.error.BadParameterException: " + e.getMessage()
#        elif isinstance(e, org.ogf.saga.error.DoesNotExistException):
#            print "org.ogf.saga.error.DoesNotExistException: " + e.getMessage()
#        elif isinstance(e, org.ogf.saga.error.IncorrectStateException):
#            print "org.ogf.saga.error.IncorrectStateException: " + e.getMessage()
#        elif isinstance(e, org.ogf.saga.error.IncorrectURLException):
#            print "org.ogf.saga.error.IncorrectURLException: " + e.getMessage()
#        elif isinstance(e, org.ogf.saga.error.NoSuccessException):
#            print "org.ogf.saga.error.NoSuccessException: \n" + e.getMessage()
#        elif isinstance(e, org.ogf.saga.error.NotImplementedException):
#            print "org.ogf.saga.error.NotImplementedException: " + e.getMessage()
#        elif isinstance(e, org.ogf.saga.error.PermissionDeniedException):
#            print "org.ogf.saga.error.PermissionDeniedException: " + e.getMessage()
#        elif isinstance(e, org.ogf.saga.error.SagaIOException):
#            print "org.ogf.saga.error.SagaIOException: " + e.getMessage()
#        elif isinstance(e, org.ogf.saga.error.TimeoutException):
#            print "org.ogf.saga.error.TimeoutException: " + e.getMessage()
#        else:
#            print "Unknown other java.exception " + e.getMessage() 
     
#    def copy(self, url_target, flags=0):
#        """override from NSEntry"""
#        print type(url_target.urlObject) 
#        try:
#            self.fileObject.copy(url_target.urlObject, flags);
#        except java.lang.Exception, exception:
#            self.__convertException(e=exception)
#        print "file.copy finished"
#        # org.ogf.saga.namespace.Flags.OVERWRITE.getValue());
# 


    def get_size(self):
        """
        Returns the number of bytes in the file
        size: number of bytes in the file
        Permission:    Query
        Raises: NotImplemented
                IncorrectState
                PermissionDenied
                AuthorizationFailed
                AuthenticationFailed
                Timeout
                NoSuccess
        Note: similar to the st_size field from stat(2)as defined by POSIX

        """
        size = 0;
        return size
    
    def read(self, len = -1, buf=None):
        #inout buffer buf, in int len_in = -1, out int len_out ):
        """
        Reads up to len bytes from the file into a buffer.
        len: number of bytes to be read
        buf: buffer to read data into
        len_out: number of bytes successfully read
        buf_out: buffer containing the data just read
        Post Condition: the data from the file are available in the buffer.
        Permission: Read
        Raises: NotImplemented
                BadParameter
                IncorrectState
                PermissionDenied
                AuthorizationFailed
                AuthenticationFailed
                Timeout
                NoSuccess
        Notes:    - the actual number of bytes read into buffer is returned in len_out. It is not an error
                 to read less bytes than requested, or in fact zero bytes, e.g. at the end of the file.
               - errors are indicated by returning negative values for len_out, which correspond to
                 negatives of the respective POSIX ERRNO error code.
               - the file pointer is positioned at the end of the byte area successfully read during this call.
               - the given buffer must be large enough to store up to len_in bytes, or managed by the
                 implementation - otherwise a BadParameter exception is thrown.
               - the notes about memory management from the buffer class apply.
               - if the file was opened in write-only mode (i.e. no READ or READWRITE flag was given), this
                 method throws an PermissionDenied exception.
               - if len_in is smaller than 0, or not given, the buffer size is used for len_in. If that is also not available, a BadParameter exception is thrown.
               - similar to read (2) as specified by POSIX

        """
        len_out = 0
        return len_out
        
    def write(self, buf, len = -1):
        # (in buffer buf, in int len_in = -1, out int len_out ):
        """
        Writes up to len from buffer into the file at the current file position.
        buf:  buffer to write data from       
        len: number of bytes to write
        len_out: number of bytes successfully written
        Post Condition: the buffer data are written to the file.
        Permission: Write
        Raises:  NotImplemented
                 BadParameter
                 IncorrectState
                 PermissionDenied
                 AuthorizationFailed
                 AuthenticationFailed
                 Timeout
                 NoSuccess
        Notes: - errors are indicated by returning negative values for len_out, which correspond to
                 negatives of the respective POSIX ERRNO error code.
               - the file pointer is positioned at the end of the byte area written during this call.
               - if the file was opened in read-only mode (i.e. no WRITE or READWRITE flag was given), this
                 method throws an PermissionDenied exception.
               - the given buffer must hold enough data to write - otherwise, only the available data
                 will be written, and and len_out will be set to the number of bytes written.
               - the notes about memory management from the buffer class apply.
               - if len_in is smaller than 0, or not given, the buffer size is used for len_in.
                 If that is also not available, a BadParameter exception is thrown.
               - if data are written beyond the current end of file, the intermediate gap is filled with null bytes.
               - similar to write (2) as specified by POSIX

        """

    def seek (self, offset, seek_mode = 0):
        #return out int position
        """
        Reposition the file pointer
        offset: offset to move pointer
        whence: offset is relative to whence
        position: position of pointer after seek
        Post Condition: - the file pointer is moved to the new position.
                        - following read() or write() operations use that position.
        Permission: Read or Write.
        Raises: NotImplemented
                IncorrectState
                PermissionDenied
                AuthorizationFailed
                AuthenticationFailed
                Timeout
                NoSuccess
        Notes: - seek repositions the file pointer for subsequent read, write and seek calls.
               - initially (after open), the file pointer is positioned at the beginning of the file,
                 unless the Append flag was given - then the initial position is the end of the file.
               - the repositioning is done relative to the position given in Whence, so relative to
                 the BEGIN or END of the file, or to the CURRENT position.
               - errors are indicated by returning negative values for len_out, which correspond to
                 negatives of the respective POSIX ERRNO error code.
               - the file pointer can be positioned after the end of the file without extending it.
               - the given offset can be positive, negative, or zero.
               - note that a subsequent read at or behind the end of file returns no data.
               - similar to lseek (2) as specified by POSIX.
        """
        position = 0
        return position
            
    def read_v(self, iovecs):
        #inout array<iovec> iovecs
        """
        Gather/scatter read
        iovecs: list of iovec structs defining start (offset) and length (len_in) of each individual read, 
                the buffer to read into, and integer to store result into (len_out).
        Post Condition: data from the file are available in the iovec buffers.
        Permission: Read
        Raises: NotImplemented
                BadParameter
                IncorrectState
                PermissionDenied
                AuthorizationFailed
                AuthenticationFailed
                Timeout
                NoSuccess
        Notes: - the behaviour of each individual read is as in the normal read method, and all notes from the read() method apply.
               - an exception MUST be thrown if any of the individual reads detects a condition which
                 would raise an exception for the normal read() method.
               - the notes about memory management from the buffer class apply.
               - if for any of the given iovecs no len_in is given, then the buffer size is used as len_in.
                 If that is also not available, a BadParameter exception is thrown.
               - if the file was opened WriteOnly, a PermissionDenied exception is thrown.
               - similar to readv (2) as specified by POSIX
        
        """

    def write_v(self, iovecs): 
        #inout array<iovec> iovecs ):
        """
        Gather/scatter write
        iovecs: list of iovec structs defining start (offset) and length (len_in) of each
                individual write, and buffers containing the data to write (len_out)
        Post Condition: - the iovec buffer data are written to the file.
        Permission: Write
        Raises: NotImplemented
                IncorrectState
                BadParameter
                PermissionDenied
                AuthorizationFailed
                AuthenticationFailed
                Timeout
                NoSuccess
        Notes: - the behaviour of each individual write is as in the normal write method.
               - an exception MUST be thrown if any of the individual writes detects a condition which
                 would raise an exception for the normal write method.
               - the notes about memory management from the buffer class apply.
               - if for any of the given iovecs no len_in is given, then the buffer size is used as len_in.
                 If that is also not available, a BadParameter exception is thrown.
               - if the file was opened ReadOnly, a PermissionDenied exception is thrown.
               - similar to writev (2) as specified by POSIX

        """
    
    
    def size_p (self, pattern):
        #in string pattern, out int size
        """
        Determine the storage size required for a pattern I/O operation
        pattern: pattern to determine size for
        size: size required for I/O operation with that pattern
        Raises: NotImplemented
                BadParameter
                IncorrectState
                PermissionDenied
                AuthorizationFailed
                AuthenticationFailed
                Timeout
                NoSuccess
        Notes:  - the method does, in general, not perform a remote operation, but is intended to help
                  the application programmer to correctly handle pattern-based I/O and associated buffer sizes.
                - if the pattern cannot be parsed or interpreted, a BadParameter exception is thrown.

        """
        size = 0
        return size
    
    
    def read_p(self, pattern, buf):
        # in string pattern, inout buffer buf, out int len_out
        """
        Pattern-based read
        pattern: pattern specification for read operation
        buf: buffer to store read data into
        len_out:  number of successfully read
        Post Condition: - data from the file are available in the buffers.
        Permission: Read
        Raises: NotImplemented
                BadParameter
                IncorrectState
                PermissionDenied
                AuthorizationFailed
                AuthenticationFailed
                Timeout
                NoSuccess
        Notes: - if the pattern cannot be parsed or interpreted, a BadParameter exception is thrown.
                - all notes for the read() method apply for the individual reads resulting from the interpretation of the pattern.
                - an exception MUST be thrown if any of the individual writes detects a condition which would raise an exception for the normal write method.

        """
        len_out = 0
        return len_out


    def write_p(self, pattern, buf): 
        #in string pattern, in buffer buf, out int len_out
        """
        Pattern-based write
        pattern: pattern specification for write operation
        buf: buffer to be written
        len_out: number of bytes successfully written
        Post Condition: the buffer data are written to the file.
        Permission: Write
        Raises: NotImplemented
                BadParameter
                IncorrectState
                PermissionDenied
                AuthorizationFailed
                AuthenticationFailed
                Timeout
                NoSuccess
        Notes: - if the pattern cannot be parsed or interpreted, a BadParameter exception is thrown.
          - all notes for the write() method apply for the individual writes resulting from the interpretation of the pattern.
          - an exception MUST be thrown if any of the individual writes detects a condition which would raise an exception for the normal write method.

        """
        len_out = 0
        return len_out

    def modes_e(self):
        #out array<string> emodes
        """
        List the extended modes available in this implementation, and/or on server side
        emodes: list of modes available for extended I/O
        Raises: NotImplemented
                IncorrectState
                PermissionDenied
                AuthorizationFailed
                AuthenticationFailed
                Timeout
                NoSuccess
        Notes: - the method does, in general, not perform a remote operation, but is intended to help
                 the application programmer to determine what extended I/O methods are supported by the implementation.

        """
        emodes = (None, ) #tuple
        return emodes

    def size_e (self, emode, spec):
        #in string emode, in string spec, out int size
        """
        Determine the storage size required for an extended I/O operation
        emode: extended mode to use
        spec: specification to determine size for
        size: size required for I/O operation with that emode/spec
        Raises: NotImplemented
                BadParameter
                IncorrectState
                PermissionDenied
                AuthorizationFailed
                AuthenticationFailed
                Timeout
                NoSuccess
        Note: - the method does, in general, not perform a remote operation, but is intended to help
                the application programmer to correctly handle extended I/O and associated buffer sizes.
              - if the specification cannot be parsed or interpreted, a BadParameter exception is thrown.

        """
        size = 0
        return size

    def read_e(self, emode, spec, buf):
        # (in string emode, in string spec, inout buffer buf, out int len_out );
        """
        Extended read
        emode: extended mode to use
        spec: specification of read operation
        buf: buffer to store read data into
        len_out: number of successfully read
        Post Condition: data from the file are available in the buffers.
        Permission: Read
        Raises: NotImplemented
                BadParameter
                IncorrectState
                PermissionDenied
                AuthorizationFailed
                AuthenticationFailed
                Timeout
                NoSuccess
        Notes: - if the emode is not supported, a BadParameter exception is thrown.
          - if the spec cannot be parsed or interpreted, a BadParameter exception is thrown.
          - all notes from the read() method apply to the individual reads resulting from the interpretation of the emode and spec.
          - an exception MUST be thrown if any of the individual writes detects a condition which would raise an exception for the normal write method.

        """
        len_out = 0
        return len_out

    def write_e (self, emode, spec, buf):
        #in string emode, in string spec, in buffer buf, out int len_out
        """
        Extended write
        emode: extended mode to use
        spec: specification of write operation
        buf: buffer to store read data into
        len_out: number of bytes successfully written
        Post Condition: the buffer data are written to the file.
        Permission: Write
        Raises: NotImplemented
                BadParameter
                IncorrectState
                PermissionDenied
                AuthorizationFailed
                AuthenticationFailed
                Timeout
                NoSuccess
        Notes: - if the emode is not supported, a BadParameter exception is thrown.
               - if the spec cannot be parsed or interpreted, a BadParameter exception is thrown.
               - all notes from the write() method apply to the individual writes resulting from the interpretation of the emode and spec.
               - an exception MUST be thrown if any of the individual writes detects a condition which would raise an exception for the normal write method.

        """   
        len_out = 0
        return len_out

class Directory(NSDirectory):
    """
    This class represents an open file descriptor for read/write operations on a physical directory. 
    """
    
    def __init__(self, name, session, flags=Flags.READ):
        #in session s, in URL name, in int flags = Read, out directory obj the newly created object
        """
        Open the directory
        @summary: Open the directory
        @param session: session to associate the object with
        @param name: location of directory 
        @keyword flags: mode for opening
        @postcondition: the directory is opened.
        @postcondition: 'Owner' of target is the id of the context use to perform the opereration, if the directory gets created.
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
        @note: the default flags are "READ" (512).
        @note: the semantics of the inherited constructors apply

        """
        super(Directory, self).__init__()
        
        
    def get_size(self, name, flags = None):
        #in URL name, in int flags = None, out int size
        """
        Returns the size of the file
        @summary: Returns the size of the file
        @param name: name of file to inspect
        @type name: sagaURL.URL()
        @param flags: mode for operation
        @type flags: int
        @return: size of the file
        @rtype: int
        @permission: Query
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
        @Note: if 'name' can be parsed as URL, but contains an invalid entry name, a BadParameter exception is thrown.
        @note: if the entry 'name' points to does not exist, a 'DoesNotExist' exception is thrown.
        @note: if the 'name' points to a link and the 'Dereference' flag is set, the size is
            returned for the link target. If that target does not exist, a 'DoesNotExist' exception is thrown.
        @note: the default flags are 'None' (0).
        @note: other flags are not allowed on this method, and cause a BadParameter exception.
        @note: similar to the 'st_size' field from 'stat' (2) as defined by POSIX

        """
        size = 0
        return size
    
    def is_file (self, name, flags = None):
        #return boolean test
        """
        Alias:    for is_entry in saga.namespace.NSDirectory
        @see: L{saga.namespace.NSDirectory.is_entry()}
        """
        
        pass

    def open_dir (self, name, flags = Flags.READ):
        #in URL name, in int flags = READ, out directory dir)
        """
        Creates a directory object
        @summary: Creates a directory object
        @param name: name of directory to open
        @type name: saga.url.URL
        @param flags: flags defining operation modus
        @type flags: value from saga.file.Flags
        @return: opened directory instance
        @rtype: saga.file.Directory
        @PostCondition: the session of the returned instance is that of the calling instance.
        @PostCondition: 'Owner' of name is the id of the context used to perform the opereration if name gets created.
        @Permission: Exec for name's parent directory.
        @Permission: Write for name's parent directory if Create is set.
        @Permission: Write for name if Write is set.
        @Permission: Read for name if Read is set.
        @Raise NotImplemented:
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
        @Note: all notes from the NSDirectory.open_dir()  method apply.
        @Note: default flags are 'READ' (512).

        """
        dir = Directory(name, self.session, flags)
        return dir
        pass

    def open (self, name, flags = Flags.READ):
        #in URL name, in int flags = Read, out file file
        """
        Creates a new file instance
        @summary: Creates a new file instance
        @param name: URl of the file to be opened
        @param flags: flags defining operation modus
        @return: opened file instance
        @PostCondition: the session of the returned instance is that of the calling instance.
        @PostCondition: 'Owner' of name is the id of the context used to perform the opereration if name gets created.
        @Permission: Exec for name's parent directory.
        @permission: Write for name's parent directory if Create is set.
        @permission: Write for name if Write is set.
        @permission: Read for name if Read is set.
        @Raise NotImplemented:
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
        @Note: all notes from the ns_directory::open() method apply.
        @note: the file is truncated to length 0 on the open operation if the 'Trunc' flag is given.
        @note: the file is in opened in append mode if the 'Append' flag is given (a seek(0, End) is
                 performed after the open). If the 'Append' flag is not given, the file pointer is
                 initially placed at the beginning of the file (a seek(0,START) is performed after the open).
        @note: the 'Binary' flag is to be silently ignored on systems which do not support it.
        @note: at least one of the flags 'Read', 'Write' or 'ReadWrite' must be given, otherwise a
                 BadParameter exception is thrown.
        @note: the flag set 'Read | Write' is equivalent to the flag 'ReadWrite'.
        @note: default flags are 'Read' (512).

        """
        pass