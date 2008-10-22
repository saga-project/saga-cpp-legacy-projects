# Page 281
#  package saga.stream

from object import Object, ObjectType
from task import Async
from monitoring import Monitorable
from permissions import Permissions
from url import URL
from context import Context
from attributes import Attributes
from error import NotImplemented



class State(object):
    """
    status: This object is not completly specified yet.
    ===================================================
    """
    NEW =  1
    OPEN = 2
    CLOSED = 3
    DROPPED = 4
    ERROR = 5
 
class Activity(object):
    """
    status: This object is not completly specified yet.
    ===================================================
    """
    READ = 1
    WRITE = 2
    EXCEPTION = 4
    
    
class StreamService(Object, Monitorable, Permissions, Async): # Async in Permissions
    """
    The StreamService object establishes a listening/server object that waits for
    client connections. It can only be used as a factory for client sockets. It does not
    do any read/write I/O.

    status: This object is not completly specified yet.
    ===================================================
    
        - B{Metrics:}
            - name: stream_server.client_connect
            - desc: fires if a client connects
            - mode: ReadOnly
            - unit: 1
            - type: Trigger
            - value: 1
            
    """
      
    def __init__(self, session, url):
        #in session s, in URL url, out StreamService obj
        """
        Create a new StreamService object
        
        B{TODO ADD!} 
        """

    # Destructor -> close()

    def get_url(self):
        #out URL url
        """
        Get URL to be used to connect to this server
        @summary: get URL to be used to connect to this server
        @return: the URL of the connection.
        @rtype: L{URL}
        @raise NotImplemented:
        @raise IncorrectState:
        @raise PermissionDenied: 
        @raise AuthorizationFailed: 
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @Note: returns a URL which can be passed to the Stream constructor to create a connection to this StreamService.
        """
        url = URL()
        return url
    
    def serve(self, timeout = -1.0):
        #in float timeout = -1.0, out stream stream
        """
        Wait for incoming client connections
        @summary: wait for incoming client connections
        @param timeout: number of seconds to wait for a client
        @type timeout: float
        @return: new Connected Stream object
        @rtype: L{Stream}
        @PostCondition: the returned client is in "OPEN" state.
        @PostCondition: the session of the returned client is that of the StreamServer.
        @permission:  Exec.
        @permission: Exec for the connecting remote party.
        @raise NotImplemented:
        @raise IncorrectState:
        @raise PermissionDenied: 
        @raise AuthorizationFailed: 
        @raise AuthenticationFailed:
        @raise NoSuccess:
        @raise Timeout: if no client connects within the specified timeout
        """
        stream = Stream()
        return stream

    def close(self, timeout = 0.0):
        #in float timeout = 0.0
        """
        Closes a stream service
        @summary: closes a stream service
        @param timeout: seconds to wait
        @type timeout: float
        @PostCondition: no clients are accepted anymore.
        @PostCondition: no callbacks registered for the "ClientConnect" metric are invoked.
        @raise NotImplemented:
        @raise IncorrectState:
        @raise NoSuccess:
        @Note: any subsequent method call on the object, besides close(), raises an "IncorrectState" exception.
        """

    
class Stream(Object, Async, Attributes, Monitorable):
    """
    This is the object that encapsulates all client Stream objects.

    status: This object is not completly specified yet.
    ===================================================
        


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
            - desc: determines the amount of idle time before dropping the line, in seconds
            - mode: ReadWrite, optional
            - type: Int
            - value: system dependend
            - notes: 
                - the implementation documents the default value
                - if this attribute is supported, the connection is closed by the
                  implementation if for that many seconds nothing has been read from or written to the stream.
        - B{Blocking}
            - name: Blocking
            - desc: determines if read/writes are blocking or not
            - mode: ReadWrite, optional
            - type: Bool
            - value: True
            - notes: 
                - if the attribute is not supported, the implementation is blocking
                - if the attribute is set to "True", a read or write operation may return immediately if
                  no data can be read or written - that does not constitute an error (see EAGAIN in POSIX).
        - B{Compression}
            - name:  Compression
            - desc:  determines if data are compressed before/after transfer
            - mode: ReadWrite, optional
            - type: Bool
            - value: schema dependent
            - note: the implementation documents the default values for the available schemas
        - B{Nodelay}
            - name:  Nodelay
            - desc:  determines if packets are sent immediately, i.e. without delay
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
            - note: if the attribute is not supported, the implementation MUST be reliable
        
    B{Metrics supported:}
    
        - B{stream.state}
            - name: stream.state
            - desc: fires if the state of the stream changes, and has the value of the new state
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
            

    """
    
    def __init__(self, session, url = ""):
        """
        Initializes a client stream for later connection to a server.
        @summary: initializes a client stream for later connection to a server.
        @param session: Session handle
        @param url: server location as URL
        @type session: L{Session}
        @type url: L{URL}
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
        @Note: server location and possibly protocol are described by the input URL.
        @Note: the url parameter can be None (which is the default). A stream so constructed is only to be used
                 as parameter to an asynchronous StreamServer.serve() call. For such a stream, a later call to connect() will fail.
        @Note: the implementation MUST ensure that the information given in the URL are usable - otherwise a BadParameter exception MUST be raised.
        @Note: the socket is only connected after the connect() method is called.

        """

# destructor the socket is closed  f the instance was not closed before, the destructor performs a close() on the instance, and all notes to close() apply.

    def get_url(self): 
        #out URL url
        """
        Get URL used for creating the stream
        @summary: get URL used for creating the stream
        @return: the URL of the connection.
        @rtype: L{URL}
        @Raise NotImplemented:
        @raise IncorrectState:
        @raise PermissionDenied: 
        @raise AuthorizationFailed: 
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @Note: returns a URL which can be passed to a stream constructor to create another
                    connection to the same StreamService.
        @Note: the returned url may be empty, indicating that this instance has been created with an empty
                    url as parameter to the stream CONSTRUCTOR().
        """
        
        return URL()
        
    def get_context(self):
        #out context ctx
        """
        Return remote authorization info
        @summary: return remote authorization info
        @return: remote context
        @rtype: L{Context}
        @PreCondition: the stream is, or has been, in the "OPEN" state.
        @PostCondition: the returned context is deep copied, and does not share state with any other object.
        @Raise NotImplemented:
        @raise IncorrectState:
        @raise PermissionDenied: 
        @raise AuthorizationFailed: 
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @Note: the context returned contains the security information from the REMOTE party, and can be used for authorization.
        @Note: if the stream is in a final state, but has been in "OPEN" state before, the returned context represents the remote party the stream has been connected to while it was in "OPEN" state.
        @Note: if the stream is not in "OPEN" state, and is not in a final state after having been in "OPEN" state, an "IncorrectState" exception is raised.
        @Note: if no security information are available, the returned context has the type "Unknown" and no attributes are attached.
        @Note: the returned context MUST be authenticated, or must be of type "Unknown" as described above.

        """
        return Context()
    
    def connect(self):
        """
        Establishes a connection to the target defined during the construction of the stream.
        @summary: establishes a connection to the target defined during the construction of the stream.
        @PreCondition:   the stream is in "NEW" state.
        @PostCondition:   the stream is in "OPEN" state.
        @permission: Exec for the StreamService represented by the url used for creating this stream instance.
        @raise NotImplemented:
        @raise IncorrectState:
        @raise PermissionDenied: 
        @raise AuthorizationFailed: 
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @Note: on failure, the stream state is changed to "ERROR"
        @Note: if the stream instance is not in "NEW" state, an "IncorrectState" exception is raised.

        """

    def wait(self, what, timeout = -1.0):
        #in int what, in float timeout = -1.0, out int cause
        """
        Check if stream is ready for reading/writing, or if it has entered an error state.
        @summary: check if stream is ready for reading/writing, or if it has entered an error state
        @param what: activity types to wait for
        @type what: int
        @param timeout: number of seconds to wait
        @type timeout: float
        @return: activity type causing the call to return
        @rtype: int
        @PreCondition: the stream is in "OPEN" state.
        @PostCondition: the stream can be read from, or written to, or it is in "ERROR" state.
        @raise NotImplemented:
        @raise IncorrectState:
        @raise PermissionDenied: 
        @raise AuthorizationFailed: 
        @raise AuthenticationFailed:
        @raise NoSuccess:
        @Note: wait will only check on the conditions specified by "what"
        @Note: "what" is an integer representing OR"ed "READ", "WRITE", or "EXCEPTION" flags from L{Activity}
        @Note: "cause" describes the availability of the socket (eg. OR"ed "READ", "WRITE", or "EXCEPTION")
        @Note: if the stream is not in "OPEN" state, an "IncorrectState" exception is raised.
        
        """
        cause = 0
        return cause
      
    def close(self, timeout = 0.0):
        """
        Closes an active connection
        @summary: closes an active connection
        @param timeout: seconds to wait
        @type timeout: float
        @PostCondition:   stream is in "Closed" state
        @raise NotImplemented:
        @raise IncorrectState:
        @raise NoSuccess:
        @Note: any subsequent method call on the object raises an "IncorrectState" exception (apart from __del__() and close()).
        @Note: if close() is implicitely called in the __del__(), it will never raise an exception.
        @Note: close() can be called multiple times, with no side effects.
        """
        
        
    def read (self, len = -1, buf = ""):
        #inout buffer buf, in int len_in = -1, out int len_out
        """
        Read a data buffer from stream
        @summary: read a data buffer from stream
        @param len: Maximum number of units that can be copied into the buffer.
        @type len: int
        @param buf: buffer to store read data into
        @type buf: L{Buffer} or string
        @return: number of bytes read, if successful.
        @rtype: int
        @PreCondition: the stream is in "OPEN" state.
        @PostCondition: data from the stream are available in the buffer.
        @permission: Read for the StreamService represented by the url used for creating this stream instance.
        @raise NotImplemented:
        @raise BadParameter:
        @raise IncorrectState:
        @raise PermissionDenied: 
        @raise AuthorizationFailed: 
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @Note: if the stream is blocking, the call waits until data become available.
        @Note: if the stream is non-blocking, the call returns immediately, even if no data 
            are available -- that is not an error condition.
        @Note: the actually number read into buffer is returned in the return value or can be found by 
            checking the length of the returned string. It is not an error to read less bytes than requested, or in fact zero bytes.
        @Note: errors are indicated by returning negative values, which correspond
            to negatives of the respective ERRNO error code
        @Note: the given buffer must be large enough to store up to len, or managed by the
            implementation - otherwise a BadParameter exception is raised.
        @Note: the notes about memory management from the Buffer class apply.
        @Note: if len is smaller than 0, or not given, the buffer size is used for len. If that is also
            not available, a BadParameter exception is raised.
        @Note: if the stream is not in "OPEN" state, an "IncorrectState" exception is raised.
        @Note: similar to read (2) as specified by POSIX
        """
        len_out = 0
        return len_out
        
    def write(self, buf, len):
        #in buffer buf, in int len_in = -1, out int len_out
        """
        Write a data buffer to stream
        @summary: write a data buffer to stream
        @param len: number of units of data in the buffer
        @type len: int
        @param buf: buffer containing data that will be sent out via socket
        @type buf: L{Buffer} or string
        @return: nr of units written if successful
        @PreCondition: the stream is in "OPEN" state.
        @PostCondition: the buffer data are written to the stream.
        @permission: Write for the StreamService represented by the url used for creating this stream instance.
        @raise NotImplemented:
        @raise BadParameter
        @raise IncorrectState:
        @raise PermissionDenied
        @raise AuthorizationFailed
        @raise AuthenticationFailed
        @raise Timeout
        @raise NoSuccess
        @Note: if the stream is blocking, the call waits until the data can be written.
        @Note: if the stream is non-blocking, the call returns immediately, 
            even if no data are written. That is not an error condition.
        @Note: it is not an error to write less than len units.
        @Note: errors are indicated by returning negative values for the return value, 
            which correspond to negatives of the respective ERRNO error code
        @Note: the notes about memory management from the buffer class apply.
        @Note: if len is smaller than 0, or not given, the buffer size is used for len. 
            If that is also not available, a BadParameter exception is raised.
        @Note: if the stream is not in "OPEN" state, an "IncorrectState" exception is raised.
        @Note: similar to write (2) as specified by POSIX

        """
        
        len_out = 0
        return len_out