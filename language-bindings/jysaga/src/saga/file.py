#For File
#package saga.file

from saga.namespace import  NSEntry, NSDirectory
from saga.permissions import  Permissions
from saga.buffer import Buffer
from saga.object import Object, ObjectType
from saga.attributes import Attributes
from saga.error import NotImplemented, BadParameter
from saga.task import TaskType, Task
from saga.session import Session
from saga.url import URL

import array.array
import jarray.array

#import org.ogf.saga.url.URLFactory;
#import org.ogf.saga.url.URL;
#import org.ogf.saga.namespace.Flags;
#import org.ogf.saga.file.File;
from org.ogf.saga.file import FileFactory
from org.ogf.saga.task import TaskMode
from org.ogf.saga.buffer import BufferFactory
import org.ogf.saga.file.SeekMode

#import java.lang.Exception;
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
    START   = 1        
    CURRENT = 2
    END     = 3
#DOCUMENT: Python uses start=0,current=1, 2=end, SAGA 1,2,3

class Iovec(Buffer, Object):
    """
    The iovec class inherits the Buffer class, and three additional state
    attributes: offset, len in and len out (with the latter one being read-only).
    With that addition, the new class can be used very much the same way as the
    iovec structure defined by POSIX for readv/writev, with the buffer len in
    beeing interpreted as the POSIX iov len, i.e. the number of bytes to read/write.

    """
    delegateObject = None
    managedByImp = True
    array = None
    applicationBuf = None
    closed = False

    def __init__(self, size = -1, data = None, len_in = -1, offset = 0, **impl):
        #in array<byte> data = "", in int size = 0, in int offset = 0, in int len_in = size, out buffer obj
        """
        Initialize an iovec instance
        @summary: initialize an iovec instance
        @param size: size of data to be used
        @param data: data to be used
        @param offset: offset for I/O operation
        @param len_in: number of units to read or write on read_v/write_v. default -1 means that len_in is size
        @type size: int
        @type offset: int
        @type len_in: int
        @type data: list or char array
        @raise BadParameter:
        @raise NoSuccess:
        @Note: all notes from the buffer __init__() apply.
        @Note: if len_in is larger than size, and size is not given as -1, a BadParameter exception is raised.
        """

        if "delegateObject" in impl:
            if not isinstance(impl["delegateObject"], org.ogf.saga.file.IOVec):
                raise BadParameter, "Parameter impl[\"delegateObject\"] is not a org.ogf.saga.file.IOVec. Type: " + str(impl["delegateObject"].__class__)
            self.delegateObject = impl["delegateObject"]
            return
        if type(size) is not int: 
            raise BadParameter, "Parameter size is not an int. Type: " + str(type(size))
        if type(data) is not array.array and type(data) is not list and data is not None:
            raise BadParameter, "Parameter data is not an list or a char array. Type: " + str(type(size)) 
        if type(data) is array and data.typecode != 'c':
            raise BadParameter, "Parameter data is an array of the wrongtype. Typecode:" + data.typecode           
        if type(len_in) is not int:
            raise BadParameter, "Parameter len_in is not an int. Type: " + str(type(len_in))
        if type(offset) is not int:
            raise BadParameter, "Parameter offset is not an int. Type: " + str(type(offset))        
        if offset < 0:
            raise BadParameter, "Parameter offset < 0. offset:" + str(offset)
# 0-0        
        if size == -1 and data is None:
            raise BadParameter, "Parameters size and data are not specified." 
# 1-0
        elif size != -1 and data is None:    
            if size is 0 or size < -1:
                raise BadParameter, "Parameter size is <= 0"
            try:
                if len_in == -1:
                    self.delegateObject = FileFactory.createIOVec(size)
                    self.managedByImp = True
                else:
                    self.delegateObject = FileFactory.createIOVec(size, len_in)
                    self.managedByImp = True
            except org.ogf.saga.error.SagaException, e:
                raise self.convertException(e)
# 0-1 
        elif size == -1 and data is not None: 
            try:
                self.array = jarray.zeros( len(data), 'b')
                if len_in == -1:
                    self.delegateObject =  FileFactory.createIOVec(self.array)
                else:
                    self.delegateObject =  FileFactory.createIOVec(self.array, len_in)
                self.managedByImp = False
                self.applicationBuf = data
            except org.ogf.saga.error.SagaException, e:
                raise self.convertException(e)
#1-1 
        elif size != -1 and data is not None:
            try:
                self.array = jarray.zeros( size, 'b')
                if len_in == -1:
                    self.delegateObject =  FileFactory.createIOVec(self.array)
                else:
                    self.delegateObject =  FileFactory.createIOVec(self.array, len_in)
                self.managedByImp = False
                self.applicationBuf = data
            except org.ogf.saga.error.SagaException, e:
                raise self.convertException(e)
        else:
            raise BadParameter, "Parameters can not be processed. size:" + size + " " + str(type(size)) + ". data: " + data + " " + str(type(data))          
        if offset > 0:
            self.set_offset(offset)
        
    def set_offset(self, offset):
        #in int offset
        """
        Set offset
        @summary: set offset
        @param offset: value for offset
        @type offset: int
        @raise BadParameter: if offset is smaller that zero, a BadParameter exception is raised.
        """
        if type(offset) is not int:
            raise BadParameter, "Parameter offset is not an int. Type: " + str(type(offset)) 
        try:
            self.delegateObject.setOffset(offset)
        except org.ogf.saga.error.SagaException, e:
            raise self.convertException(e)


    def get_offset (self):
        #out int offset);
        """
        Retrieve the current value for offset
        @summary: retrieve the current value for offset
        @return: value of offset
        @rtype: int
        """
        try:
            return self.delegateObject.getOffset()
        except org.ogf.saga.error.SagaException, e:
            raise self.convertException(e)
        
    def set_len_in(self, len_in):
        #set_len_in (in int len_in);
        """
        Set len_in
        @param len_in: value for len_in (see __init__)
        @type len_in: int
        @raise BadParameter: if len_in is larger than size, and size is not set to -1, a BadParameter exception is raised.
        """
        try:
            return self.delegateObject.setLenIn(len_in)
        except org.ogf.saga.error.SagaException, e:
            raise self.convertException(e)

    def get_len_in(self):
        #get_len_in (out int len_in);
        """
        Retrieve the current value for len_in
        @summary: retrieve the current value for len_in
        @return: value of len_in (see __init__)
        @rtype: int
        """
        try:
            return self.delegateObject.getLenIn()
        except org.ogf.saga.error.SagaException, e:
            raise self.convertException(e)    

    def get_len_out(self):
        #get_len_out (out int len_out);
        """
        Retrieve the value for len_out
        @return: value of len_out
        @rtype: int
        @Note: len_out reports the number of units read or written in a completed read_w or write_w operation.
        @Note: before completion of the operation, the returned value is -1.
        @Note: for implementation managed memory, the value of len_out is always the same as for size.
        """
        try:
            return self.delegateObject.getLenOut()
        except org.ogf.saga.error.SagaException, e:
            raise self.convertException(e)    
        
    def get_type(self):
        """
        Query the object type.
        @summary: Query the object type.
        @return: type of the object as an int from ObjectType
        @rtype: int
        """
        return ObjectType.IOVEC
    
    def clone(self):
        """
        @summary: Deep copy the object
        @return: the deep copied object
        @rtype: L{Object}
        @PostCondition: apart from session and callbacks, no other state is shared
            between the original object and it's copy.
        @raise NoSuccess:
        @Note: that method is overloaded by all classes which implement saga.object.Object, and returns
                 a deep copy of the respective class type.
        @see: section 2 of the GFD-R-P.90 document for deep copy semantics.

        """
        try:
            javaClone = self.delegateObject.clone()
            temp = Iovec(delegateObject = javaClone)
            temp.managedByImp = self.managedByImp
            if self.array is None:
                temp.array = None
            else:
                temp.array = jarray.array(self.array, 'b')
            #TODO: check clone and buffer behaviour -> Java Data copying? Set data?
            temp.applicationBuf = self.applicationBuf
            temp.closed = self.closed
            return temp
        except org.ogf.saga.error.SagaException, e:
            raise self.convertException(e)          


class File(NSEntry):
    """
    This class represents an open file descriptor for read/write operations on a 
    physical file

    """
    delegateObject = None
    
    def __init__(self, name, session = Session(), flags=Flags.READ, **impl):
        """
        Initialize the File object
        @summary: initialize the File object
        @param session:  session to associate the object with
        @param name: location of file
        @param flags: mode for opening
        @type session: L{Session}
        @type name: L{URL}
        @type flags: int
        @postcondition: the file is opened.
        @postcondition: Owner of target is the id of the context use to perform 
            the opereration, if the file gets created.
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
        @Note: all notes from the Directory.open() method apply.
        @Note: the default flags are READ (512).
        """
        if "delegateObject" in impl:
            if not isinstance(impl["delegateObject"], org.ogf.saga.file.File):
                raise BadParameter, "Parameter impl[\"delegateObject\"] is not"\
                " a org.ogf.saga.file.File. Type: " + \
                str(impl["delegateObject"].__class__)
            self.delegateObject = impl["delegateObject"]
        else:
            if type(session) is not Session:
                raise BadParameter,"Parameter session is not a Session. Type: "\
                     + str(type(session))
            if type(name) is not URL:
                raise BadParameter, "Parameter name is not a URL. Type: " \
                + str(type(name))
            if type(flags) is not int:
                raise BadParameter, "Parameter flags is not an int. Type: " \
                + str(type(flags))
            try:
                self.delegateObject = FileFactory.createFile\
                            (session.delegateObject, name.delegateObject, flags)
            except org.ogf.saga.error.SagaException, e:
                raise self.convertException(e)
            
#TODO: add async object creation to all task implementing methods.
#TODO: async task creation -> errors -> create dummy task. file and namespace modules

    def get_size(self, tasktype=TaskType.NORMAL):
        """
        Returns the number of bytes in the file
        @summary: returns the number of bytes in the file
        @return: number of bytes in the file
        @rtype: int
        @permission: Query
        @raise NotImplemented:
        @raise IncorrectState:
        @raise PermissionDenied:
        @raise AuthorizationFailed:
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @Note: similar to the st_size field from stat(2)as defined by POSIX

        """
        if tasktype is not TaskType.NORMAL and tasktype is not TaskType.SYNC \
        and tasktype is not TaskType.ASYNC  and tasktype is not TaskType.TASK:
            raise BadParameter, "Parameter tasktype is not one of the TaskType values, but " + str(tasktype)
        try:
            if tasktype is TaskType.ASYNC:
                javaObject = self.delegateObject.getSize(TaskMode.ASYNC)
                return Task(delegateObject=javaObject)
            if tasktype is TaskType.SYNC:
                javaObject = self.delegateObject.getSize(TaskMode.SYNC)
                return Task(delegateObject=javaObject)
            if tasktype is TaskType.TASK:
                javaObject = self.delegateObject.getSize(TaskMode.TASK)
                return Task(delegateObject=javaObject)        
            else:
                import java.lang.Long
                temp = java.lang.Long( self.delegateObject.getSize() )
                return temp.intValue()
                #DOCUMENT: cast the long to int
        except org.ogf.saga.error.SagaException, e:
                raise self.convertException(e)

    
    def read(self, size = -1, buf=None, tasktype=TaskType.NORMAL):
        #inout buffer buf, in int size_in = -1, out int size_out ):
        """
        Reads up to size bytes from the file into a buffer.
               
            - B{Call format: read( size, data )}
                - B{Returns: int}
                    - number of bytes successfully read
                - B{Postcondition:}
                    - the data from the file are available in the buffer.
            
            - B{Call format: read( size )}
                - B{Returns: string}
                    - the read data. 'size' determines the maximum length to be 
                        read 
                - B{Postcondition:}
                    - the data is available in the returned string
                    
            - B{Call format: read()}
                - B{Returns: string}
                    - the read data. Size of the string could be between 0 and 
                        the file length
        
        @summary: reads up to size bytes from the file into a buffer
        @param size: number of bytes to be read
        @param buf: buffer to read data into
        @type size: int
        @type buf: L{Buffer}
        @return: number of bytes successfully read or string containing the 
            read data 
        @rtype: int or string
        @permission: Read
        @raise NotImplemented:
        @raise BadParameter:
        @raise IncorrectState:
        @raise PermissionDenied:
        @raise AuthorizationFailed:
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @Note: the actual number of bytes read into buffer is returned in the 
            int. It is not an error to read less bytes than requested, or in 
            fact zero bytes, e.g. at the end of the file.
        @Note: errors are indicated by returning negative values, which 
            correspond to negatives of the respective POSIX ERRNO error code.
        @Note: the file pointer is positioned at the end of the byte area
            successfully read during this call.
        @Note: the given buffer must be able to grow large enough to store up to
            size bytes, or managed by the implementation - otherwise a 
            BadParameter exception is raised.
        @Note: the notes about memory management from the buffer class apply.
        @Note: if the file was opened in write-only mode (i.e. no READ or 
            READWRITE flag was given), this method raises an PermissionDenied 
            exception.
        @Note: if size is smaller than 0, or not given, the buffer size is used 
            for size. If that is also not available, a BadParameter exception is 
            raised.
        @Note: similar to read (2) as specified by POSIX
        """
        
        if tasktype is not TaskType.NORMAL and tasktype is not TaskType.SYNC \
        and tasktype is not TaskType.ASYNC  and tasktype is not TaskType.TASK:
            raise BadParameter( "Parameter tasktype is not one of the TaskType values, but " + str(tasktype))
        if type (size)is not int:
            raise BadParameter, "Parameter size is not an int. Type: " + str(type(size))
        if not issubclass(buf.__class__, Buffer) and buf is not None:
            raise BadParameter, "Parameter buf is not a Buffer or subclass. Class: " + str(buf.__class__)
        try:
            if size != -1 and buf is not None:
                if tasktype is TaskType.ASYNC:
                    javaObject = self.delegateObject.read(TaskMode.ASYNC, buf.delegateObject, size)
                    return Task(delegateObject=javaObject, fileReadBuffer = buf)
                if tasktype is TaskType.SYNC:
                    javaObject = self.delegateObject.read(TaskMode.SYNC, buf.delegateObject, size)
                    return Task(delegateObject=javaObject, fileReadBuffer = buf)
                if tasktype is TaskType.TASK:
                    javaObject = self.delegateObject.read(TaskMode.TASK, buf.delegateObject, size)
                    return Task(delegateObject=javaObject, fileReadBuffer = buf)        
                else:
                    retval = self.delegateObject.read(buf.delegateObject, size)
                    if buf.managedByImp is False:
                        buf.update_data()
                    return retval
            elif size != -1 and buf is None:
                javaBuffer =  BufferFactory.createBuffer(size)
                if tasktype is TaskType.ASYNC:
                    javaObject = self.delegateObject.read(TaskMode.ASYNC, javaBuffer, size)
                    return Task(delegateObject=javaObject, fileReadBuffer=javaBuffer)
                if tasktype is TaskType.SYNC:
                    javaObject = self.delegateObject.read(TaskMode.SYNC, javaBuffer, size)
                    return Task(delegateObject=javaObject, fileReadBuffer=javaBuffer)
                if tasktype is TaskType.TASK:
                    javaObject = self.delegateObject.read(TaskMode.TASK, javaBuffer, size)
                    return Task(delegateObject=javaObject, fileReadBuffer=javaBuffer)        
                else:
                    retval = self.delegateObject.read(javaBuffer, size)
                    return javaBuffer.getData().tostring()
            elif size == -1 and buf is None:
                #- B{Call format: read()} 
                
                javaBuffer =  BufferFactory.createBuffer(self.delegateObject.getSize())
                if tasktype is TaskType.ASYNC:
                    javaObject = self.delegateObject.read(TaskMode.ASYNC, javaBuffer)
                    return Task(delegateObject=javaObject, fileReadBuffer=javaBuffer)
                if tasktype is TaskType.SYNC:
                    javaObject = self.delegateObject.read(TaskMode.SYNC, javaBuffer)
                    return Task(delegateObject=javaObject, fileReadBuffer=javaBuffer)
                if tasktype is TaskType.TASK:
                    javaObject = self.delegateObject.read(TaskMode.TASK, javaBuffer)
                    return Task(delegateObject=javaObject, fileReadBuffer=javaBuffer)        
                else:
                    retval = self.delegateObject.read(javaBuffer)
                    return javaBuffer.getData().tostring()    
            else:
                raise BadParameter("Correct call is read(size,data), read(size) or read() ")  
        except org.ogf.saga.error.SagaException, e:
            raise self.convertException(e)
                      
#DOCUMENT change len to size        
#TODO: Check after read, update application managed buffer
        
    def write(self, buf, size = -1, tasktype=TaskType.NORMAL):
        # (in buffer buf, in int size_in = -1, out int size_out ):
        """
        Writes up to size from buffer into the file at the current file position.
        @summary: writes up to size from buffer into the file at the current file position.
        @param buf:  buffer to write data from       
        @param size: number of bytes to write
        @type buf: L{Buffer} or string
        @type size: int
        @return: number of bytes successfully written
        @rtype: int
        @postcondition: the buffer data are written to the file.
        @permission: Write
        @raise NotImplemented:
        @raise BadParameter:
        @raise IncorrectState:
        @raise PermissionDenied:
        @raise AuthorizationFailed:
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @Note: errors are indicated by returning negative values, which correspond to
                 negatives of the respective POSIX ERRNO error code.
        @Note: the file pointer is positioned at the end of the byte area written during this call.
        @Note: if the file was opened in read-only mode (i.e. no WRITE or READWRITE flag was given), this
                 method raises an PermissionDenied exception.
        @Note: the given buffer must hold enough data to write - otherwise, only the available data
                 will be written, and the returned value will be set to the number of bytes written.
        @Note: the notes about memory management from the buffer class apply.
        @Note: if size is smaller than 0, or not given, the buffer size is used for size.
                 If that is also not available, a BadParameter exception is raised.
        @Note: if data are written beyond the current end of file, the intermediate gap is filled with null bytes.
        @Note: similar to write (2) as specified by POSIX

        """
        if tasktype is not TaskType.NORMAL and tasktype is not TaskType.SYNC \
        and tasktype is not TaskType.ASYNC  and tasktype is not TaskType.TASK:
            raise BadParameter, "Parameter tasktype is not one of the TaskType values, but " + str(tasktype)
        if type (size)is not int:
            raise BadParameter, "Parameter size is not an int. Type: " + str(type(size))
        if not issubclass(buf.__class__, Buffer) and buf is not None:
            raise BadParameter, "Parameter buf is not a Buffer or subclass. Class: " + str(buf.__class__)
        if size < -1:
            raise BadParameter, "Parameter size < 0"
        #TODO: update internal buffer with data from application
        try:
            if tasktype is TaskType.ASYNC:
                if size is -1:
                    javaObject = self.delegateObject.write(TaskMode.ASYNC, buf.delegateObject)
                else:
                    javaObject = self.delegateObject.write(TaskMode.ASYNC, buf.delegateObject, size)
                return Task(delegateObject=javaObject)
            if tasktype is TaskType.SYNC:
                if size is -1:
                    javaObject = self.delegateObject.write(TaskMode.SYNC, buf.delegateObject)
                else:
                    javaObject = self.delegateObject.write(TaskMode.SYNC, buf.delegateObject, size)
                return Task(delegateObject=javaObject)
            if tasktype is TaskType.TASK:
                if size is -1:
                    javaObject = self.delegateObject.write(TaskMode.Task, buf.delegateObject)
                else:
                    javaObject = self.delegateObject.write(TaskMode.Task, buf.delegateObject, size)
                return Task(delegateObject=javaObject) 
            else:
                if size is -1:
                    return self.delegateObject.write(buf.delegateObject)
                else:
                    return self.delegateObject.write(buf.delegateObject, size)
        except org.ogf.saga.error.SagaException, e:
                raise self.convertException(e)



    def seek (self, offset, whence = SeekMode.START, tasktype=TaskType.NORMAL ):
        #return out int position
        """
        Reposition the file pointer
        @summary: reposition the file pointer
        @param offset: offset to move pointer
        @param whence: offset is relative to whence
        @type offset: int
        @type whence: int
        @return: position of pointer after seek
        @rtype: int
        @postcondition: the file pointer is moved to the new position. Following read() or write() operations use that position.
        @permission: Read or Write.
        @raise NotImplemented:
        @raise IncorrectState:
        @raise PermissionDenied:
        @raise AuthorizationFailed:
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @Note: seek repositions the file pointer for subsequent read, write and seek calls.
        @Note: initially (after open), the file pointer is positioned at the beginning of the file,
                 unless the Append flag was given - then the initial position is the end of the file.
        @Note: the repositioning is done relative to the position given in Whence, so relative to
                 the BEGIN or END of the file, or to the CURRENT position.
        @Note: errors are indicated by returning negative values for len_out, which correspond to
                 negatives of the respective POSIX ERRNO error code.
        @Note: the file pointer can be positioned after the end of the file without extending it.
        @Note: the given offset can be positive, negative, or zero.
        @Note: note that a subsequent read at or behind the end of file returns no data.
        @Note: similar to lseek (2) as specified by POSIX.
        """
        if type(whence) is not int:
            raise BadParameter, "Parameter whence is not an int. Type " + str(type(tasktype))
        if type(offset) is not int:
            raise BadParameter, "Parameter offset is not an int. Type " + str(type(offset))
        if tasktype is not TaskType.NORMAL and tasktype is not TaskType.SYNC \
        and tasktype is not TaskType.ASYNC  and tasktype is not TaskType.TASK:
            raise BadParameter, "Parameter tasktype is not one of the TaskType values, but " + str(tasktype)
        if whence == SeekMode.START:
            whence_parameter = org.ogf.saga.file.SeekMode.START 
        elif whence == SeekMode.CURRENT:
            whence_parameter = org.ogf.saga.file.SeekMode.CURRENT
        else: #whence == SeekMode.END
            whence_parameter = org.ogf.saga.file.SeekMode.END
        try:
            if tasktype is TaskType.ASYNC:
                javaObject = self.delegateObject.seek(TaskMode.ASYNC, offset, whence_parameter)
                return Task(delegateObject=javaObject)
            if tasktype is TaskType.SYNC:
                javaObject = self.delegateObject.seek(TaskMode.SYNC, offset, whence_parameter)
                return Task(delegateObject=javaObject)
            if tasktype is TaskType.TASK:
                javaObject = self.delegateObject.seek(TaskMode.TASK, offset, whence_parameter)
                return Task(delegateObject=javaObject)        
            else:
                return self.delegateObject.seek(offset,whence_parameter)
        except org.ogf.saga.error.SagaException, e:
                raise self.convertException(e)
            
    def read_v(self, iovecs, tasktype=TaskType.NORMAL):
        #inout array<iovec> iovecs
        """
        Gather/scatter read
        @summary: gather/scatter read
        @param iovecs: list of iovec structs defining start (offset) and length (len_in) of each individual read, 
                the buffer to read into, and integer to store result into (len_out).
        @type iovecs: list of iovecs
        @postcondition: data from the file are available in the iovec buffers.
        @permission: Read
        @raise NotImplemented:
        @raise BadParameter:
        @raise IncorrectState:
        @raise PermissionDenied:
        @raise AuthorizationFailed:
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @Note: the behaviour of each individual read is as in the normal read method, and all notes from the read() method apply.
        @Note: an exception MUST be raised if any of the individual reads detects a condition which
                 would raise an exception for the normal read() method.
        @Note: the notes about memory management from the buffer class apply.
        @Note: if for any of the given iovecs no len_in is given, then the buffer size is used as len_in.
                 If that is also not available, a BadParameter exception is raised.
        @Note: if the file was opened WriteOnly, a PermissionDenied exception is raised.
        @Note: similar to readv (2) as specified by POSIX
        
        """
        if tasktype is not TaskType.NORMAL and tasktype is not TaskType.SYNC \
        and tasktype is not TaskType.ASYNC  and tasktype is not TaskType.TASK:
            raise BadParameter, "Parameter tasktype is not one of the TaskType values, but " + str(tasktype)
        if type(iovecs) is not Iovec and type(iovecs) is not list:
            raise BadParameter, "Parameter iovecs is not a list of Iovecs , but " + str(type(tasktype))
        if type(iovecs) is list:
            for i in range(len(iovecs)):
                if type(iovecs[i]) is not Iovec:
                    raise BadParameter, "Parameter iovecs is not a list of IOVecs, Contains a" + str(type(tasktype)) + "at position " + str(i)
        javaArray = jarray.array([], org.ogf.saga.file.IOVec)
        for j in range(len(iovecs)):
            javaArray.append(iovecs[i].delegateObject)
        try:
            if tasktype is TaskType.ASYNC:
                javaObject = self.delegateObject.readV(TaskMode.ASYNC, javaArray)
                return Task(delegateObject=javaObject, fileReadBuffer = iovecs)
            if tasktype is TaskType.SYNC:
                javaObject = self.delegateObject.readV(TaskMode.SYNC, javaArray)
                return Task(delegateObject=javaObject, fileReadBuffer = iovecs)
            if tasktype is TaskType.TASK:
                javaObject = self.delegateObject.readV(TaskMode.TASK, javaArray)
                return Task(delegateObject=javaObject, fileReadBuffer = iovecs)        
            else:
                self.delegateObject.readV(javaArray)
                for item in iovecs:
                    if item.managedByImp is False:
                        item.update_data()               
        except org.ogf.saga.error.SagaException, e:
                raise self.convertException(e)        
            
        
    def write_v(self, iovecs, tasktype=TaskType.NORMAL): 
        #inout array<iovec> iovecs ):
        """
        Gather/scatter write
        @summary: gather/scatter write
        @param iovecs: list of iovecs defining start (offset) and length (len_in) of each
                individual write, and buffers containing the data to write (len_out)
        @type iovecs: list of iovecs
        @postcondition: the iovec buffer data are written to the file.
        @permission: Write
        @raise NotImplemented:
        @raise IncorrectState:
        @raise BadParameter:
        @raise PermissionDenied:
        @raise AuthorizationFailed:
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @Note: the behaviour of each individual write is as in the normal write method.
        @Note: an exception is raised if any of the individual writes detects a condition which
                 would raise an exception for the normal write method.
        @Note: the notes about memory management from the buffer class apply.
        @Note: if for any of the given iovecs no len is given, then the buffer size is used as len.
                 If that is also not available, a BadParameter exception is raised.
        @Note: if the file was opened READONLY, a PermissionDenied exception is raised.
        @Note: similar to writev (2) as specified by POSIX
        """
        if tasktype is not TaskType.NORMAL and tasktype is not TaskType.SYNC \
        and tasktype is not TaskType.ASYNC  and tasktype is not TaskType.TASK:
            raise BadParameter, "Parameter tasktype is not one of the TaskType values, but " + str(tasktype)
        if type(iovecs) is not Iovec and type(iovecs) is not list:
            raise BadParameter, "Parameter iovecs is not a list of Iovecs , but " + str(type(tasktype))
        if type(iovecs) is list:
            for i in range(len(iovecs)):
                if type(iovecs[i]) is not Iovec:
                    raise BadParameter, "Parameter iovecs is not a list of IOVecs, Contains a" + str(type(tasktype)) + "at position " + str(i)
        javaArray = jarray.array([], org.ogf.saga.file.IOVec)
        for j in range(len(iovecs)):
            javaArray.append(iovecs[i].delegateObject)
        try:
            if tasktype is TaskType.ASYNC:
                javaObject = self.delegateObject.writeV(TaskMode.ASYNC, javaArray)
                return Task(delegateObject=javaObject)
            if tasktype is TaskType.SYNC:
                javaObject = self.delegateObject.writeV(TaskMode.SYNC, javaArray)
                return Task(delegateObject=javaObject)
            if tasktype is TaskType.TASK:
                javaObject = self.delegateObject.writeV(TaskMode.TASK, javaArray)
                return Task(delegateObject=javaObject)        
            else:
                self.delegateObject.writeV(javaArray)
        except org.ogf.saga.error.SagaException, e:
                raise self.convertException(e)        



    def size_p (self, pattern, tasktype=TaskType.NORMAL):
        #in string pattern, out int size
        """
        Determine the storage size required for a pattern I/O operation
        @summary: determine the storage size required for a pattern I/O operation
        @param pattern: pattern to determine size for
        @type pattern: string
        @return: size required for I/O operation with that pattern
        @rtype: int
        @raise NotImplemented:
        @raise BadParameter:
        @raise IncorrectState:
        @raise PermissionDenied:
        @raise AuthorizationFailed:
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @Note: the method does, in general, not perform a remote operation, but is intended to help
                  the application programmer to correctly handle pattern-based I/O and associated buffer sizes.
        @Note: if the pattern cannot be parsed or interpreted, a BadParameter exception is raised.

        """
        if type(pattern) is not str:
            raise BadParameter, "Parameter pattern is not a string. Type: " + str(type(pattern))
        if tasktype is not TaskType.NORMAL and tasktype is not TaskType.SYNC \
        and tasktype is not TaskType.ASYNC  and tasktype is not TaskType.TASK:
            raise BadParameter, "Parameter tasktype is not one of the TaskType values, but " + str(tasktype)
        try:
            if tasktype is TaskType.ASYNC:
                javaObject = self.delegateObject.sizeP(TaskMode.ASYNC,pattern)
                return Task(delegateObject=javaObject)
            if tasktype is TaskType.SYNC:
                javaObject = self.delegateObject.sizeP(TaskMode.SYNC, pattern)
                return Task(delegateObject=javaObject)
            if tasktype is TaskType.TASK:
                javaObject = self.delegateObject.sizeP(TaskMode.TASK, pattern)
                return Task(delegateObject=javaObject)        
            else:
                return self.delegateObject.sizeP(pattern)
        except org.ogf.saga.error.SagaException, e:
                raise self.convertException(e)        
 
    
    def read_p(self, pattern, buf, tasktype=TaskType.NORMAL):
        # in string pattern, inout buffer buf, out int len_out
        """
        Pattern-based read
        @summary: pattern-based read
        @param pattern: pattern specification for read operation
        @param buf: buffer to store read data into
        @type pattern: string
        @type buf: L{Buffer}
        @return: number of bytes successfully read
        @rtype: int
        @postcondition: data from the file are available in the buffers.
        @permission: Read
        @raise NotImplemented:
        @raise BadParameter:
        @raise IncorrectState:
        @raise PermissionDenied:
        @raise AuthorizationFailed:
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @Note: if the pattern cannot be parsed or interpreted, a BadParameter exception is raised.
        @Note: all notes for the read() method apply for the individual reads resulting from the interpretation of the pattern.
        @Note: an exception MUST be raised if any of the individual writes detects a condition which would raise an exception for the normal write method.

        """
        if type(pattern) is not str:
            raise BadParameter, "Parameter pattern is not a string. Type: " + str(type(pattern))
        if buf.__class__ is not Buffer:
            raise BadParameter, "Parameter buf is not a Buffer. Class: " + str(buf.__class__)
        if tasktype is not TaskType.NORMAL and tasktype is not TaskType.SYNC \
        and tasktype is not TaskType.ASYNC  and tasktype is not TaskType.TASK:
            raise BadParameter, "Parameter tasktype is not one of the TaskType values, but " + str(tasktype)
        try:
            if tasktype is TaskType.ASYNC:
                javaObject = self.delegateObject.readP(TaskMode.ASYNC, pattern, buf.delegateObject)
                return Task(delegateObject=javaObject, fileReadBuffer = buf)
            if tasktype is TaskType.SYNC:
                javaObject = self.delegateObject.readP(TaskMode.SYNC, pattern,buf.delegateObject)
                return Task(delegateObject=javaObject, fileReadBuffer = buf)
            if tasktype is TaskType.TASK:
                javaObject = self.delegateObject.readP(TaskMode.TASK, pattern, buf.delegateObject)
                return Task(delegateObject=javaObject, fileReadBuffer = buf)        
            else:
                retval = self.delegateObject.readP(pattern, buf.delegateObject)
                if buf.managedByImp is False:
                    buf.update_data()
                return retval 
        except org.ogf.saga.error.SagaException, e:
                raise self.convertException(e)


    def write_p(self, pattern, buf, tasktype=TaskType.NORMAL): 
        #in string pattern, in buffer buf, out int len_out
        """
        Pattern-based write
        @summary: pattern-based write
        @param pattern: pattern specification for write operation
        @param buf: buffer to be written
        @type pattern: string
        @type buf: L{Buffer}
        @return: number of units successfully written
        @rtype: int
        @postcondition: the buffer data are written to the file.
        @permission: Write
        @raise NotImplemented:
        @raise BadParameter:
        @raise IncorrectState:
        @raise PermissionDenied:
        @raise AuthorizationFailed:
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @Note: if the pattern cannot be parsed or interpreted, a BadParameter exception is raised.
        @Note: all notes for the write() method apply for the individual writes resulting from the interpretation of the pattern.
        @Note: an exception MUST be raised if any of the individual writes detects a condition which would raise an exception for the normal write method.

        """
        if type(pattern) is not str:
            raise BadParameter, "Parameter pattern is not a string. Type: " + str(type(pattern))
        if buf.__class__ is not Buffer:
            raise BadParameter, "Parameter buf is not a Buffer. Class: " + str(buf.__class__)
        if tasktype is not TaskType.NORMAL and tasktype is not TaskType.SYNC \
        and tasktype is not TaskType.ASYNC  and tasktype is not TaskType.TASK:
            raise BadParameter, "Parameter tasktype is not one of the TaskType values, but " + str(tasktype)
        try:
            if tasktype is TaskType.ASYNC:
                javaObject = self.delegateObject.writeP(TaskMode.ASYNC, pattern, buf.delegateObject)
                return Task(delegateObject=javaObject)
            if tasktype is TaskType.SYNC:
                javaObject = self.delegateObject.writeP(TaskMode.SYNC, pattern,buf.delegateObject)
                return Task(delegateObject=javaObject)
            if tasktype is TaskType.TASK:
                javaObject = self.delegateObject.writeP(TaskMode.TASK, pattern, buf.delegateObject)
                return Task(delegateObject=javaObject)        
            else:
                retval = self.delegateObject.writeP(pattern, buf.delegateObject)
                return retval 
        except org.ogf.saga.error.SagaException, e:
                raise self.convertException(e)


    def modes_e(self, tasktype=TaskType.NORMAL):
        #out array<string> emodes
        """
        List the extended modes available in this implementation, and/or on server side
        @summary: list the extended modes available in this implementation, and/or on server side
        @return: tuple of modes available for extended I/O
        @rtype: tuple
        @raise NotImplemented:
        @raise IncorrectState:
        @raise PermissionDenied:
        @raise AuthorizationFailed:
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @Note: the method does, in general, not perform a remote operation, but is intended to help
            the application programmer to determine what extended I/O methods are supported by the implementation.

        """
        if tasktype is not TaskType.NORMAL and tasktype is not TaskType.SYNC \
        and tasktype is not TaskType.ASYNC  and tasktype is not TaskType.TASK:
            raise BadParameter, "Parameter tasktype is not one of the TaskType values, but " + str(tasktype)
        try:
            if tasktype is TaskType.ASYNC:
                javaObject = self.delegateObject.modesE(TaskMode.ASYNC)
                return Task(delegateObject=javaObject)
            if tasktype is TaskType.SYNC:
                javaObject = self.delegateObject.modesE(TaskMode.SYNC)
                return Task(delegateObject=javaObject)
            if tasktype is TaskType.TASK:
                javaObject = self.delegateObject.modesE(TaskMode.TASK)
                return Task(delegateObject=javaObject)        
            else:
                retval = self.delegateObject.modesE()
                list = []
                for i in range(retval.size()):
                    list.append( retval.get(i).toString() )
                return tuple(list) 
        except org.ogf.saga.error.SagaException, e:
                raise self.convertException(e)

    def size_e (self, emode, spec, tasktype=TaskType.NORMAL):
        #in string emode, in string spec, out int size
        """
        Determine the storage size required for an extended I/O operation
        @summary: determine the storage size required for an extended I/O operation
        @param emode: extended mode to use
        @param spec: specification to determine size for
        @type emode: string
        @type spec: string
        @return: size required for I/O operation with that emode/spec
        @rtype: int
        @raise NotImplemented:
        @raise BadParameter:
        @raise IncorrectState:
        @raise PermissionDenied:
        @raise AuthorizationFailed:
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @Note: the method does, in general, not perform a remote operation, but is intended to help
                the application programmer to correctly handle extended I/O and associated buffer sizes.
        @Note: if the specification cannot be parsed or interpreted, a BadParameter exception is raised.

        """
        if tasktype is not TaskType.NORMAL and tasktype is not TaskType.SYNC \
        and tasktype is not TaskType.ASYNC  and tasktype is not TaskType.TASK:
            raise BadParameter, "Parameter tasktype is not one of the TaskType values, but " + str(tasktype)
        if type(emode) is not str:
            raise BadParameter, "Parameter emode is not a string. Type: " + str(type(emode))
        if type(spec) is not str:
            raise BadParameter, "Parameter spec is not a string. Type: " + str(type(spec))
        try:
            if tasktype is TaskType.ASYNC:
                javaObject = self.delegateObject.sizeE(TaskMode.ASYNC, emode, spec)
                return Task(delegateObject=javaObject)
            if tasktype is TaskType.SYNC:
                javaObject = self.delegateObject.sizeE(TaskMode.SYNC, emode, spec)
                return Task(delegateObject=javaObject)
            if tasktype is TaskType.TASK:
                javaObject = self.delegateObject.sizeE(TaskMode.TASK, emode, spec)
                return Task(delegateObject=javaObject)        
            else:
                return self.delegateObject.sizeE(emode, spec)       
        except org.ogf.saga.error.SagaException, e:
                raise self.convertException(e) 
            

    def read_e(self, emode, spec, buf, tasktype=TaskType.NORMAL):
        # (in string emode, in string spec, inout buffer buf, out int len_out );
        """
        Extended read
        @summary: extended read
        @param emode: extended mode to use
        @param spec: specification of read operation
        @param buf: buffer to store read data into
        @type emode: string
        @type spec: string
        @type buf: L{Buffer}
        @return: number of bytes successfully read
        @rtype: int
        @postcondition: data from the file are available in the buffers.
        @permission: Read
        @raise NotImplemented:
        @raise BadParameter:
        @raise IncorrectState:
        @raise PermissionDenied:
        @raise AuthorizationFailed:
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @Note: if the emode is not supported, a 'BadParameter' exception is raised.
        @Note: if the spec cannot be parsed or interpreted, a 'BadParameter' exception is raised.
        @Note: all notes from the read() method apply to the individual reads resulting from the interpretation of the emode and spec.
        @Note: an exception is raised if any of the individual reads detects a condition which would raise an exception for the normal read method.

        """
        if tasktype is not TaskType.NORMAL and tasktype is not TaskType.SYNC \
        and tasktype is not TaskType.ASYNC  and tasktype is not TaskType.TASK:
            raise BadParameter, "Parameter tasktype is not one of the TaskType values, but " + str(tasktype)
        if type(emode) is not str:
            raise BadParameter, "Parameter emode is not a string. Type: " + str(type(emode))
        if type(spec) is not str:
            raise BadParameter, "Parameter spec is not a string. Type: " + str(type(spec))
        if buf.__class__ is not Buffer:
            raise BadParameter, "Parameter buf is not a Buffer. Class: " + str(buf.__class__)
        try:
            if tasktype is TaskType.ASYNC:
                javaObject = self.delegateObject.readE(TaskMode.ASYNC, emode, spec, buf.delegateObject)
                return Task(delegateObject=javaObject, fileReadBuffer = buf)
            if tasktype is TaskType.SYNC:
                javaObject = self.delegateObject.readE(TaskMode.SYNC,  emode, spec, buf.delegateObject)
                return Task(delegateObject=javaObject, fileReadBuffer = buf)
            if tasktype is TaskType.TASK:
                javaObject = self.delegateObject.readE(TaskMode.TASK,  emode, spec, buf.delegateObject)
                return Task(delegateObject=javaObject, fileReadBuffer = buf)        
            else:
                retval = self.delegateObject.readE(emode, spec, buf.delegateObject)
                if buf.managedByImp is False:
                    buf.update_data()
                return retval 
        except org.ogf.saga.error.SagaException, e:
                raise self.convertException(e)
                
    
        
    def write_e (self, emode, spec, buf, tasktype=TaskType.NORMAL):
        #in string emode, in string spec, in buffer buf, out int len_out
        """
        Extended write
        @summary: extended write
        @param emode: extended mode to use
        @param spec: specification of write operation
        @param buf: buffer to store read data into
        @type emode: string
        @type spec: string
        @type buf: L{Buffer}
        @return: number of units successfully written
        @rtype: int
        @postcondition: the buffer data are written to the file.
        @permission: Write
        @raise NotImplemented:
        @raise BadParameter:
        @raise IncorrectState:
        @raise PermissionDenied:
        @raise AuthorizationFailed:
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @Note: if the emode is not supported, a BadParameter exception is raised.
        @Note: if the spec cannot be parsed or interpreted, a BadParameter exception is raised.
        @Note: all notes from the write() method apply to the individual writes resulting from the interpretation of the emode and spec.
        @Note: an exception MUST be raised if any of the individual writes detects a condition which would raise an exception for the normal write method.

        """   
        if tasktype is not TaskType.NORMAL and tasktype is not TaskType.SYNC \
        and tasktype is not TaskType.ASYNC  and tasktype is not TaskType.TASK:
            raise BadParameter, "Parameter tasktype is not one of the TaskType values, but " + str(tasktype)
        if type(emode) is not str:
            raise BadParameter, "Parameter emode is not a string. Type: " + str(type(emode))
        if type(spec) is not str:
            raise BadParameter, "Parameter spec is not a string. Type: " + str(type(spec))
        if buf.__class__ is not Buffer:
            raise BadParameter, "Parameter buf is not a Buffer. Class: " + str(buf.__class__)
        try:
            if tasktype is TaskType.ASYNC:
                javaObject = self.delegateObject.writeE(TaskMode.ASYNC, emode, spec, buf.delegateObject)
                return Task(delegateObject=javaObject)
            if tasktype is TaskType.SYNC:
                javaObject = self.delegateObject.writeE(TaskMode.SYNC,  emode, spec, buf.delegateObject)
                return Task(delegateObject=javaObject)
            if tasktype is TaskType.TASK:
                javaObject = self.delegateObject.writeE(TaskMode.TASK,  emode, spec, buf.delegateObject)
                return Task(delegateObject=javaObject)        
            else:
                retval = self.delegateObject.writeE(emode, spec, buf.delegateObject)
                return retval 
        except org.ogf.saga.error.SagaException, e:
                raise self.convertException(e)
  
    def get_type(self):
        """
        Query the object type.
        @summary: Query the object type.
        @return: type of the object as an int from ObjectType
        @rtype: int
        """
        return ObjectType.FILE

    def clone(self):
        """
        @summary: Deep copy the object
        @return: the deep copied object
        @rtype: L{Object}
        @PostCondition: apart from session and callbacks, no other state is shared
            between the original object and it's copy.
        @raise NoSuccess:
        @Note: that method is overloaded by all classes which implement saga.object.Object, and returns
                 a deep copy of the respective class type.
        @see: section 2 of the GFD-R-P.90 document for deep copy semantics.

        """
        try:
            javaClone = self.delegateObject.clone()
            clone = File(name="", delegateObject=javaClone)
            return clone
        except org.ogf.saga.error.SagaException, e:
            raise self.convertException(e)

class Directory(NSDirectory):
    """
    This class represents an open file descriptor for read/write operations on a physical directory. 
    """
    
    def __init__(self, name, session = Session(), flags=Flags.READ, **impl):
        #in session s, in URL name, in int flags = Read, out directory obj the newly created object
        """
        Initialize the Directory object
        @summary: initialize the Directory object
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
        if "delegateObject" in impl:
            if not isinstance(impl["delegateObject"], org.ogf.saga.file.Directory):
                raise BadParameter, "Parameter impl[\"delegateObject\"] is not a org.ogf.saga.file.Directory. Type: " + str(impl["delegateObject"].__class__)
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
                    self.delegateObject = FileFactory.createDirectory(session.delegateObject, name.delegateObject)
                elif flags is not Flags.NONE and session is not "default":
                    self.delegateObject = FileFactory.createDirectory(session.delegateObject, name.delegateObject, flags)
                elif flags is Flags.NONE and session is "default":
                    self.delegateObject = FileFactory.createDirectory(name.delegateObject)
                else:
                    self.delegateObject = FileFactory.createDirectory(name.delegateObject, flags)
            except org.ogf.saga.error.SagaException, e:
                raise self.convertException(e)       
        
    def get_size(self, name, flags = None, tasktype=TaskType.NORMAL):
        #in URL name, in int flags = None, out int size
        """
        Returns the size of the file
        @summary: Returns the size of the file
        @param name: name of file to inspect
        @type name: L{URL}
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
        @Note: if 'name' can be parsed as URL, but contains an invalid entry name, a BadParameter exception is raised.
        @note: if the entry 'name' points to does not exist, a 'DoesNotExist' exception is raised.
        @note: if the 'name' points to a link and the 'Dereference' flag is set, the size is
            returned for the link target. If that target does not exist, a 'DoesNotExist' exception is raised.
        @note: the default flags are 'None' (0).
        @note: other flags are not allowed on this method, and cause a BadParameter exception.
        @note: similar to the 'st_size' field from 'stat' (2) as defined by POSIX

        """
        if tasktype is not TaskType.NORMAL and tasktype is not TaskType.SYNC \
        and tasktype is not TaskType.ASYNC  and tasktype is not TaskType.TASK:
            raise BadParameter, "Parameter tasktype is not one of the TaskType values, but " + str(tasktype)
        if type(name) is not URL:
                raise BadParameter, "Parameter name is not a URL. Type: " + str(type(name))
        if type(flags) is not int:
                raise BadParameter, "Parameter flags is not an int. Type: " + str(type(flags))
        try:
            if tasktype is TaskType.ASYNC:
                javaObject = self.delegateObject.getSize(TaskMode.ASYNC, name.delegateObject,flags)
                return Task(delegateObject=javaObject)
            if tasktype is TaskType.SYNC:
                javaObject = self.delegateObject.getSize(TaskMode.SYNC, name.delegateObject, flags)
                return Task(delegateObject=javaObject)
            if tasktype is TaskType.TASK:
                javaObject = self.delegateObject.getSize(TaskMode.TASK, name.delegateObject, flags)
                return Task(delegateObject=javaObject)        
            else:
                import java.lang.Long
                temp = java.lang.Long(self.delegateObject.getSize(name.delegateObject, flags))
                return temp.intValue()
                #DOCUMENT: cast the long to int
        except org.ogf.saga.error.SagaException, e:
                raise self.convertException(e)

            
    def is_file (self, name, tasktype=TaskType.NORMAL):
        #return boolean test
        """
        Alias:    for is_entry in saga.namespace.NSDirectory
        @see: L{saga.namespace.NSDirectory.is_entry()}
        """
        if tasktype is not TaskType.NORMAL and tasktype is not TaskType.SYNC \
        and tasktype is not TaskType.ASYNC  and tasktype is not TaskType.TASK:
            raise BadParameter, "Parameter tasktype is not one of the TaskType values, but " + str(tasktype)
        if type(name) is not URL:
                raise BadParameter, "Parameter name is not a URL. Type: " + str(type(name))
        try:
            if tasktype is TaskType.ASYNC:
                javaObject = self.delegateObject.isFile(TaskMode.ASYNC, name.delegateObject)
                return Task(delegateObject=javaObject)
            if tasktype is TaskType.SYNC:
                javaObject = self.delegateObject.isFile(TaskMode.SYNC, name.delegateObject)
                return Task(delegateObject=javaObject)
            if tasktype is TaskType.TASK:
                javaObject = self.delegateObject.isFile(TaskMode.TASK, name.delegateObject)
                return Task(delegateObject=javaObject)        
            else:
                return self.delegateObject.isFile(name.delegateObject)
        except org.ogf.saga.error.SagaException, e:
                raise self.convertException(e)                

#DOCUMENT: Flags parameter not needed!


    def open_dir (self, name, flags = Flags.READ, tasktype=TaskType.NORMAL):
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
        @permission: Exec for name's parent directory.
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
        @Note: all notes from the NSDirectory.open_dir()  method apply.
        @Note: default flags are 'READ' (512).

        """
        if type(name) is not URL:
            raise BadParameter, "Parameter name is not a URL. Type: " + str(type(name))
        if type(flags) is not int:
            raise BadParameter, "Parameter flags is not an int. Type: " + str(type(flags)) 
        if tasktype is not TaskType.NORMAL and tasktype is not TaskType.SYNC \
        and tasktype is not TaskType.ASYNC  and tasktype is not TaskType.TASK:
            raise BadParameter, "Parameter tasktype is not one of the TaskType values, but " + str(tasktype)
        try:
            if tasktype is TaskType.ASYNC:
                javaObject = self.delegateObject.openDirectory(TaskMode.ASYNC, name.delegateObject, flags)
                return Task(delegateObject=javaObject)
            if tasktype is TaskType.SYNC:
                javaObject = self.delegateObject.openDirectory(TaskMode.SYNC, name.delegateObject, flags)
                return Task(delegateObject=javaObject)
            if tasktype is TaskType.TASK:
                javaObject = self.delegateObject.openDirectory(TaskMode.TASK, name.delegateObject, flags)
                return Task(delegateObject=javaObject)
            else:
                javaObject = self.delegateObject.openDirectory(name.delegateObject, flags)
                return Directory(delegateObject = javaObject)
        except org.ogf.saga.error.SagaException, e:
            raise self.convertException(e)

    def open (self, name, flags = Flags.READ, tasktype=TaskType.NORMAL):
        #in URL name, in int flags = Read, out file file
        """
        Creates a new file instance
        @summary: Creates a new file instance
        @param name: URl of the file to be opened
        @param flags: flags defining operation modus
        @return: opened file instance
        @PostCondition: the session of the returned instance is that of the calling instance.
        @PostCondition: 'Owner' of name is the id of the context used to perform the opereration if name gets created.
        @permission: Exec for name's parent directory.
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
                 BadParameter exception is raised.
        @note: the flag set 'Read | Write' is equivalent to the flag 'ReadWrite'.
        @note: default flags are 'Read' (512).

        """
        if type(name) is not URL:
            raise BadParameter, "Parameter name is not a URL. Type: " + str(type(name))
        if type(flags) is not int:
            raise BadParameter, "Parameter flags is not an int. Type: " + str(type(flags)) 
        if tasktype is not TaskType.NORMAL and tasktype is not TaskType.SYNC \
        and tasktype is not TaskType.ASYNC  and tasktype is not TaskType.TASK:
            raise BadParameter, "Parameter tasktype is not one of the TaskType values, but " + str(tasktype)
        try:
            if tasktype is TaskType.ASYNC:
                javaObject = self.delegateObject.openFile(TaskMode.ASYNC, name.delegateObject, flags)
                return Task(delegateObject=javaObject)
            if tasktype is TaskType.SYNC:
                javaObject = self.delegateObject.openFile(TaskMode.SYNC, name.delegateObject, flags)
                return Task(delegateObject=javaObject)
            if tasktype is TaskType.TASK:
                javaObject = self.delegateObject.openFile(TaskMode.TASK, name.delegateObject, flags)
                return Task(delegateObject=javaObject)
            else:
                javaObject = self.delegateObject.openFile(name.delegateObject, flags)
                return File(delegateObject = javaObject)
        except org.ogf.saga.error.SagaException, e:
            raise self.convertException(e)
        