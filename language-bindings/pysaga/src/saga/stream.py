# Package: pysaga
# Module: stream 
# Description: The module which specifies classes which interact with streams
# Specification and documentation can be found in section 4.5, page 281-301 of 
#    the GFD-R-P.90 document
# Author: P.F.A. van Zoolingen, Computer Systems Section, Faculty of 
#    Exact Science (FEW), Vrije Universiteit, Amsterdam, The Netherlands.

from object import Object, ObjectType
from task import Async, TaskType, Task
from monitoring import Monitorable
from permissions import Permissions
from url import URL
from context import Context
from attributes import Attributes
from error import NotImplemented
from session import Session


class State(object):
    """
    State holds the possible states for a Stream.

    @summary: State holds the possible states for a Stream.
    @version: 1.0, designed for Python 2.x
        
    """
    
    NEW =  1
    """
    @summary: A newly constructed stream enters the initial NEW state. It is not 
        connected yet, and no I/O operations can be performed on it. connect()
        must be called to advance the state to OPEN (on success) or ERROR (on
        failure).
    """

    OPEN = 2
    """
    @summary: The stream is connected to the remote endpoint, and I/O operations 
        can be called. If any error eccurs on the stream, it will move into the 
        ERROR state. If the remote party closes the connection, the stream will 
        move into the DROPPED state. If close() is called on the stream, the 
        stream will enter the CLOSED state.
    """
    
    CLOSED = 3
    """
    @summary: The close() method was called on the stream - I/O is no longer
        possible. This is a final state.
    """
    
    DROPPED = 4
    """
    @summary: The remote party closed the connection - I/O is no longer 
        possible. This is a final state.
    """
    
    ERROR = 5
    """
    @summary: An error occured on the stream - I/O is no longer possible. This 
        is a final state. 
    """
 
class Activity(object):
    """
    The SAGA stream API allows for event driven communication. A stream can
    flag activities, i.e. Read, Write and Exception, and the application can 
    react on these activities. It is possible to poll for these events 
    (using wait() with a potential timeout), or to get asynchronous notification 
    of these events, by using the respective metrics.

    @summary: Activity holds the possible events for which the application can
        get an asynchronous notification.
    @version: 1.0, designed for Python 2.x
           
    """
    
    READ = 1
    """
    @summary: Data are available on the stream, and a subsequent read() will  
        succeed.
    """
    
    WRITE = 2
    """
    @summary: The stream is accepting data, and a subsequent write() will 
        succeed.
    """
    
    EXCEPTION = 4
    """
    @summary: An error occured on the stream, and a following I/O operation may 
        fail.
    """
    
class StreamService(Object, Monitorable, Permissions, Async): 
    """
    The StreamService object establishes a listening/server object that waits 
    for client connections. It can only be used as a factory for client sockets. 
    It does not do any read/write I/O.
    
        - B{Metrics:}
            - name: stream_server.client_connect
            - desc: fires if a client connects
            - mode: ReadOnly
            - unit: 1
            - type: Trigger
            - value: 1
            
    @summary: The StreamService object establishes a listening/server object 
        that waits for client connections.
    @version: 1.0, designed for Python 2.x
             
    """
      
    def __init__(self, url="", session=Session(), tasktype=TaskType.NORMAL):
        """Initializes a new StreamService object.
        
        @summary: Initializes a new StreamService object.
        @param session: session to be used for object creation
        @type session: L{Session} 
        @param url: channel name or url, defines the source side binding for 
            the stream
        @type url: L{URL}
        @param tasktype: return a normal StreamService object or or return a 
            Task object that creates the StreamService object in a final, 
            RUNNING or NEW state. By default, tasktype is L{TaskType.NORMAL}
        @type tasktype: value from L{TaskType}
        @postcondition: StreamService can wait for incoming connections.
        @postcondition: 'Owner' of name is the id of the context used to create 
            the StreamService.
        @postcondition: the StreamServer has 'Exec', 'Query', 'Read'
           and 'Write' permissions for '*'.
        @raise NotImplemented:
        @raise IncorrectURL:
        @raise BadParameter:
        @raise PermissionDenied:
        @raise AuthorizationFailed:
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @note: if the given url is an empty string (the default), the 
            implementation will choose an appropriate default value.
        @note: the implementation ensures that the given URL is usable, and a 
            later call to 'serve' will not fail because of the information given
            by the URL - otherwise, a 'BadParameter' exception is thrown.
            
        """

    def get_url(self, tasktype=TaskType.NORMAL):
        """Get the URL to be used to connect to this server.
        
        @summary: Get the URL to be used to connect to this server.
        @param tasktype: return the normal return values or a Task object in a 
            final, RUNNING or NEW state. By default, tasktype 
            is L{TaskType.NORMAL}
        @type tasktype: value from L{TaskType}
        @return: the URL of the connection.
        @rtype: L{URL}
        @raise NotImplemented:
        @raise IncorrectState:
        @raise PermissionDenied: 
        @raise AuthorizationFailed: 
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @Note: returns a URL which can be passed to the Stream constructor to 
            create a connection to this StreamService.
            
        """
        raise NotImplemented("This method is not yet implemented")
    
    def serve(self, timeout=-1.0, tasktype=TaskType.NORMAL):
        """Wait for incoming client connections.
        
        @summary: Wait for incoming client connections.
        @param timeout: number of seconds to wait for a client
        @type timeout: float
        @param tasktype: return the normal return values or a Task object in a 
            final, RUNNING or NEW state. By default, tasktype 
            is L{TaskType.NORMAL}
        @type tasktype: value from L{TaskType}
        @return: new Connected Stream object
        @rtype: L{Stream}
        @PostCondition: the returned client is in "OPEN" state.
        @PostCondition: the session of the returned client is that of the 
            StreamServer.
        @permission: Exec.
        @permission: Exec for the connecting remote party.
        @raise NotImplemented:
        @raise IncorrectState:
        @raise PermissionDenied: 
        @raise AuthorizationFailed: 
        @raise AuthenticationFailed:
        @raise NoSuccess:
        @raise Timeout: if no client connects within the specified timeout
        
        """
        raise NotImplemented("This method is not yet implemented")

    def close(self, timeout=0.0, tasktype=TaskType.NORMAL):
        """Closes a stream service.
        
        @summary: closes a stream service.
        @param timeout: seconds to wait
        @type timeout: float
        @param tasktype: return the normal return values or a Task object in a 
            final, RUNNING or NEW state. By default, tasktype 
            is L{TaskType.NORMAL}
        @type tasktype: value from L{TaskType}
        @PostCondition: no clients are accepted anymore.
        @PostCondition: no callbacks registered for the "ClientConnect" metric 
            are invoked.
        @raise NotImplemented:
        @raise IncorrectState:
        @raise NoSuccess:
        @Note: any subsequent method call on the object, besides close(), 
            raises an "IncorrectState" exception.
            
        """
        raise NotImplemented("This method is not yet implemented")
    
    
class Stream(Object, Async, Attributes, Monitorable):
    """
    This is the object that encapsulates all client Stream objects.

    B{Attributes supported:}
    
        - B{Bufsize}
            - name: Bufsize
            - desc: determines the size of the send buffer
            - mode: ReadWrite, optional
            - type: Int
            - value: system dependend
            - notes:
                - the implementation documents the default value, 
                  and its meaning (e.g. on what layer that buffer
                  is maintained, or if it disables zero copy).
        - B{Timeout}
            - name: Timeout
            - desc: determines the amount of idle time before dropping the line, 
                in seconds
            - mode: ReadWrite, optional
            - type: Int
            - value: system dependend
            - notes: 
                - the implementation documents the default value
                - if this attribute is supported, the connection is closed by 
                    the implementation if for that many seconds nothing has 
                    been read from or written to the stream.
        - B{Blocking}
            - name: Blocking
            - desc: determines if read/writes are blocking or not
            - mode: ReadWrite, optional
            - type: Bool
            - value: True
            - notes: 
                - if the attribute is not supported, the implementation is 
                    blocking
                - if the attribute is set to "True", a read or write operation 
                    may return immediately if no data can be read or written - 
                    that does not constitute an error (see EAGAIN in POSIX).
        - B{Compression}
            - name:  Compression
            - desc:  determines if data are compressed before/after transfer
            - mode: ReadWrite, optional
            - type: Bool
            - value: schema dependent
            - note: the implementation documents the default values for 
                the available schemas
        - B{Nodelay}
            - name:  Nodelay
            - desc:  determines if packets are sent immediately, i.e. without 
                delay
            - mode: ReadWrite, optional
            - type: Bool
            - value: True
            - note: similar to the TCP_NODELAY option
        - B{Reliable}
            - name:  Reliable
            - desc:  determines if all sent data MUST arrive
            - mode:  ReadWrite, optional
            - type:  Bool
            - value: True
            - note: if the attribute is not supported, the implementation 
                is reliable
        
    B{Metrics supported:}
    
        - B{stream.state}
            - name: stream.state
            - desc: fires if the state of the stream changes, and has the 
                value of the new state
            - mode: ReadOnly
            - unit: 1
            - type: Enum
            -value: New
        - B{stream.read}
            - name: stream.read
            - desc: fires if a stream gets readable
            - mode: ReadOnly
            - unit: 1
            - type: Trigger
            - value: 1
            - note: a stream is considered readable if subsequent read()
                can sucessfully read 1 or more bytes of data.
        - B{stream.write}
            - name:  stream.write
            - desc:  fires if a stream gets writable
            - mode:  ReadOnly
            - unit:  1
            - type:  Trigger
            - value: 1
            - note: a stream is considered writable if a subsequent write() 
                can sucessfully write 1 or more units of data.
        - B{stream.exception}
            - name:  stream.exception
            - desc:  fires if a stream has an error condition
            - mode:  ReadOnly
            - unit:  1
            - type:  Trigger
            - value: 1
        - B{stream.dropped}
            - name:  stream.dropped
            - desc:  fires if the stream gets dropped by the remote party
            - mode: ReadOnly
            - unit: 1
            - type: Trigger
            - value: 1
            
    @version: 1.0, designed for Python 2.x    
    @undocumented: __set_Bufsize
    @undocumented: __get_Bufsize
    @undocumented: __set_Timeout
    @undocumented: __get_Timeout
    @undocumented: __set_Blocking
    @undocumented: __get_Blocking
    @undocumented: __set_Compression
    @undocumented: __get_Compression
    @undocumented: __set_Nodelay
    @undocumented: __get_Nodelay
    @undocumented: __set_Reliable     
    @undocumented: __get_Reliable
    
    """
    
    def __init__(self, url="", session=Session(), tasktype=TaskType.NORMAL):
        """Initializes a client stream for later connection to a server.
        
        @summary: initializes a client stream for later connection to a server.
        @param session: Session handle
        @param url: server location as URL
        @type session: L{Session}
        @type url: L{URL}
        @param tasktype: return a normal Stream object or or return a 
            Task object that creates the Stream object in a final, 
            RUNNING or NEW state. By default, tasktype is L{TaskType.NORMAL}
        @type tasktype: value from L{TaskType}
        @PostCondition:   the state of the socket is "New".
        @permission: Query for the StreamService represented by url.
        @raise NotImplemented:
        @raise IncorrectURL:
        @raise BadParameter:
        @raise PermissionDenied: 
        @raise AuthorizationFailed: 
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @Note: server location and possibly protocol are described by the input 
            URL.
        @Note: the url parameter can be None (which is the default). A stream 
            so constructed is only to be used as parameter to an asynchronous 
            StreamServer.serve() call. For such a stream, a later call to 
            connect() will fail.
        @Note: the implementation ensures that the information given in the URL 
            are usable - otherwise a BadParameter exception is raised.
        @Note: the socket is only connected after the connect() method is 
            called.
            
        """

    def __def__(self):
        """Destroy a Stream object.
        
        @summary: Destroy a Stream object.
        @note: If during the destruction of the object, the object was not 
            closed before, the destructor performs a close() on the instance, 
            and all notes to close() apply.
            
        """

    def get_url(self, tasktype=TaskType.NORMAL): 
        """Get URL used for creating the stream.
        
        @summary: get URL used for creating the stream.
        @param tasktype: return the normal return values or a Task object in a 
            final, RUNNING or NEW state. By default, tasktype 
            is L{TaskType.NORMAL}
        @type tasktype: value from L{TaskType}
        @return: the URL of the connection.
        @rtype: L{URL}
        @Raise NotImplemented:
        @raise IncorrectState:
        @raise PermissionDenied: 
        @raise AuthorizationFailed: 
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @Note: returns a URL which can be passed to a stream constructor to 
            create another connection to the same StreamService.
        @Note: the returned url may be empty, indicating that this instance has 
            been created with an empty url as parameter to the Stream 
            constructor.
            
        """
        raise NotImplemented("This method is not yet implemented") 
       
    def get_context(self, tasktype=TaskType.NORMAL):
        """Return remote authorization info
        
        @summary: return remote authorization info
        @param tasktype: return the normal return values or a Task object in a 
            final, RUNNING or NEW state. By default, tasktype 
            is L{TaskType.NORMAL}
        @type tasktype: value from L{TaskType}
        @return: remote context
        @rtype: L{Context}
        @PreCondition: the stream is, or has been, in the "OPEN" state.
        @PostCondition: the returned context is deep copied, and does not share 
            state with any other object.
        @Raise NotImplemented:
        @raise IncorrectState:
        @raise PermissionDenied: 
        @raise AuthorizationFailed: 
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @Note: the context returned contains the security information from the 
            REMOTE party, and can be used for authorization.
        @Note: if the stream is in a final state, but has been in "OPEN" state 
            before, the returned context represents the remote party the stream 
            has been connected to while it was in "OPEN" state.
        @Note: if the stream is not in "OPEN" state, and is not in a final state 
            after having been in "OPEN" state, an "IncorrectState" exception is 
            raised.
        @Note: if no security information are available, the returned context 
            has the type "Unknown" and no attributes are attached.
        @Note: the returned context is authenticated, or is of type 
            "Unknown" as described above.
            
        """
        raise NotImplemented("This method is not yet implemented")
    
    def connect(self, tasktype=TaskType.NORMAL):
        """Establishes a connection to the target defined during the 
        construction of the stream.
        
        @summary: Establishes a connection to the target defined during the 
            construction of the stream.
        @param tasktype: return the normal return values or a Task object in a 
            final, RUNNING or NEW state. By default, tasktype 
            is L{TaskType.NORMAL}
        @type tasktype: value from L{TaskType}
        @PreCondition: the stream is in "NEW" state.
        @PostCondition: the stream is in "OPEN" state.
        @permission: Exec for the StreamService represented by the url used for 
            creating this stream instance.
        @raise NotImplemented:
        @raise IncorrectState:
        @raise PermissionDenied: 
        @raise AuthorizationFailed: 
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @Note: on failure, the stream state is changed to "ERROR"
        @Note: if the stream instance is not in "NEW" state, an "IncorrectState" 
            exception is raised.
            
        """
        raise NotImplemented("This method is not yet implemented")
    
    def wait(self, what, timeout=-1.0, tasktype=TaskType.NORMAL):
        """Check if stream is ready for reading/writing, or if it has 
        entered an error state.
        
        @summary: Check if stream is ready for reading/writing, or if it has 
            entered an error state
        @param what: activity type to wait for from L{Activity}
        @type what: int
        @param timeout: number of seconds to wait
        @type timeout: float
        @param tasktype: return the normal return values or a Task object in a 
            final, RUNNING or NEW state. By default, tasktype 
            is L{TaskType.NORMAL}
        @type tasktype: value from L{TaskType}
        @return: activity type causing the call to return
        @rtype: int
        @PreCondition: the stream is in "OPEN" state.
        @PostCondition: the stream can be read from, or written to, or it is in 
            "ERROR" state.
        @raise NotImplemented:
        @raise IncorrectState:
        @raise PermissionDenied: 
        @raise AuthorizationFailed: 
        @raise AuthenticationFailed:
        @raise NoSuccess:
        @Note: wait will only check on the conditions specified by "what"
        @Note: "what" is an integer representing OR"ed "READ", "WRITE", or 
            "EXCEPTION" flags from L{Activity}
        @Note: "cause" describes the availability of the socket (eg. OR"ed 
            "READ", "WRITE", or "EXCEPTION")
        @Note: if the stream is not in "OPEN" state, an "IncorrectState" 
            exception is raised.
            
        """
        raise NotImplemented("This method is not yet implemented")
          
    def close(self, timeout=0.0, tasktype=TaskType.NORMAL):
        """Closes an active connection.
        
        @summary: Closes an active connection.
        @param timeout: seconds to wait
        @type timeout: float
        @param tasktype: return the normal return values or a Task object in a 
            final, RUNNING or NEW state. By default, tasktype 
            is L{TaskType.NORMAL}
        @type tasktype: value from L{TaskType}
        @PostCondition: stream is in "Closed" state
        @raise NotImplemented:
        @raise IncorrectState:
        @raise NoSuccess:
        @Note: any subsequent method call on the object raises an 
            "IncorrectState" exception (apart from __del__() and close()).
        @Note: if close() is implicitely called in the __del__(), it will never 
            raise an exception.
        @Note: close() can be called multiple times, with no side effects.
        
        """
        raise NotImplemented("This method is not yet implemented")
    
    def read (self, size=-1, buf=None, tasktype=TaskType.NORMAL):
        """Read up to size bytes of data from stream.

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
        
        @summary: Read a data from a stream
        @param size: Maximum number of units that can be copied into the buffer.
        @type size: int
        @param buf: buffer to store read data into
        @type buf: L{Buffer}
        @param tasktype: return the normal return values or a Task object in a 
            final, RUNNING or NEW state. By default, tasktype 
            is L{TaskType.NORMAL}
        @type tasktype: value from L{TaskType}
        @return: string containing the data or the number of bytes read, if 
            successful. 
        @rtype: int or string
        @PreCondition: the stream is in "OPEN" state.
        @PostCondition: data from the stream are available in the buffer or in
            the returned string.
        @permission: Read for the StreamService represented by the url used for 
            creating this stream instance.
        @raise NotImplemented:
        @raise BadParameter:
        @raise IncorrectState:
        @raise PermissionDenied: 
        @raise AuthorizationFailed: 
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @Note: if the stream is blocking, the call waits until data become 
            available.
        @Note: if the stream is non-blocking, the call returns immediately, even 
            if no data is available. That is not an error condition.
        @Note: the actually number read into buffer is returned in the return 
            value or can be found by checking the length of the returned string. 
            It is not an error to read less bytes than requested, or in fact 
            zero bytes.
        @Note: errors are indicated by returning negative values, which 
            correspond to negatives of the respective ERRNO error code
        @Note: the given buffer must be large enough to store up to len, or 
            managed by the implementation, otherwise a BadParameter exception is 
            raised.
        @Note: the notes about memory management from the Buffer class apply.
        @Note: if len is smaller than 0, or not given, the buffer size is used 
            for len. If that is also not available, a BadParameter exception is 
            raised.
        @Note: if the stream is not in "OPEN" state, an "IncorrectState" 
            exception is raised.
        @Note: similar to read (2) as specified by POSIX
        
        """
        raise NotImplemented("This method is not yet implemented")
        
    def write(self, buf, size=-1, tasktype=TaskType.NORMAL):
        """Write a data buffer to stream
        
        @summary: write a data buffer to stream
        @param size: number of units of data in the buffer
        @type size: int
        @param buf: buffer containing data that will be sent out via socket
        @type buf: L{Buffer} or string
        @param tasktype: return the normal return values or a Task object in a 
            final, RUNNING or NEW state. By default, tasktype 
            is L{TaskType.NORMAL}
        @type tasktype: value from L{TaskType}
        @return: nr of bytes written if successful
        @PreCondition: the stream is in "OPEN" state.
        @PostCondition: the buffer data are written to the stream.
        @permission: Write for the StreamService represented by the url used for 
            creating this stream instance.
        @raise NotImplemented:
        @raise BadParameter:
        @raise IncorrectState:
        @raise PermissionDenied:
        @raise AuthorizationFailed:
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @Note: if the stream is blocking, the call waits until the data can be 
            written.
        @Note: if the stream is non-blocking, the call returns immediately, 
            even if no data are written. That is not an error condition.
        @Note: it is not an error if write has written less than size to the 
            stream.
        @Note: errors are indicated by returning negative values for the return 
            value, which correspond to negatives of the respective ERRNO error 
            code
        @Note: the notes about memory management from the buffer class apply.
        @Note: if size is smaller than 0, or not given, the buffer size is used 
            for size. If that is also not available, a BadParameter exception is 
            raised.
        @Note: if the stream is not in "OPEN" state, an "IncorrectState" 
            exception is raised.
        @Note: similar to write (2) as specified by POSIX

        """
        raise NotImplemented("This method is not yet implemented")
    
    def __set_Bufsize(self, value):
        set_attribute("Bufsize", value)
        
    def __get_Bufsize(self):
        return get_attribute("Bufsize")  

    Bufsize = property(__get_Bufsize, __set_Bufsize,
            doc="""The Bufsize attribute. 
                @type: int""")

    def __set_Timeout(self, value):
        set_attribute("Timeout", value)
        
    def __get_Timeout(self):
        return get_attribute("Timeout")  

    Timeout = property(__get_Timeout, __set_Timeout,
            doc="""The Timeout attribute. 
                @type: int""")


    def __set_Blocking(self, value):
        set_attribute("Blocking", value)
        
    def __get_Blocking(self):
        return get_attribute("Blocking")  

    Blocking = property(__get_Blocking, __set_Blocking,
            doc="""The Blocking attribute. 
                @type: bool""")

    def __set_Compression(self, value):
        set_attribute("Compression", value)
        
    def __get_Compression(self):
        return get_attribute("Compression")  

    Compression = property(__get_Compression, __set_Compression, 
                           doc="""The Compression attribute. 
                               @type: bool""")

    def __set_Nodelay(self, value):
        set_attribute("Nodelay", value)
        
    def __get_Nodelay(self):
        return get_attribute("Nodelay")  
    
    Nodelay = property(__get_Nodelay, __set_Nodelay,
            doc="""The Nodelay attribute. 
                @type: bool""")


    def __set_Reliable(self, value):
        set_attribute("Reliable", value)
        
    def __get_Reliable(self):
        return get_attribute("Reliable")  

    Reliable = property(__get_Reliable, __set_Reliable, 
            doc="""The Reliable attribute. 
                @type: bool""")
    
     
  