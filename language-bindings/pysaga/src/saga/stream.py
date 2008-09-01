# Page 281
#  package saga.stream

from object import Object, ObjectType
from task import Async
from monitoring import Monitorable
from permissions import Permissions
from url import URL
from context import Context
from attributes import Attributes

class State(object):
    NEW =  1
    OPEN = 2
    CLOSED = 3
    DROPPED = 4
    ERROR = 5
 
class Activity(object):
    READ = 1
    WRITE = 2
    EXCEPTION = 4
    
    
class StreamService(Object, Monitorable, Permissions): # Async in Permissions
    """
    The StreamService object establishes a listening/server object that waits for
    client connections. It can only be used as a factory for client sockets. It does not
    do any read/write I/O.
    
    Metrics: 
    name: stream_server.client_connect
    desc: fires if a client connects
    mode: ReadOnly
    unit: 1
    type: Trigger
    value: 1
    """
      
    def __init__(self, session, url):
        #in session s, in URL url, out StreamService obj
        """
        Create a new StreamService object
        """

    # Destructor -> close()

    def get_url(self):
        #out URL url
        """
        Get URL to be used to connect to this server
        url: the URL of the connection.
        Raises: NotImplemented
                IncorrectState
                PermissionDenied
                AuthorizationFailed
                AuthenticationFailed
                Timeout
                NoSuccess
        Notes: returns a URL which can be passed to the Stream constructor to create a connection to this StreamService.
        """
        url = URL()
        return url
    
    def serve(self, timeout = -1.0):
        #in float timeout = -1.0, out stream stream
        """
        Wait for incoming client connections
        timeout: number of seconds to wait for a client
        client: new Connected Stream object
        Post Condition: 
               -the returned client is in "Open" state.
              - the session of the returned client is that of the StreamServer.
        Perms:    - Exec.
                  - Exec for the connecting remote party.
        Raises:   NotImplemented
                  IncorrectState
                  PermissionDenied
                  AuthorizationFailed
                  AuthenticationFailed
                  NoSuccess
                  Timeout, if no client connects within the specified timeout
        """
        stream = Stream()
        return stream

    def close(self, timeout = 0.0):
        #in float timeout = 0.0
        """
        Closes a stream service
        timeout: seconds to wait
        Post Condition: 
            - no clients are accepted anymore.
            - no callbacks registered for the "ClientConnect" metric are invoked.
        Raises: NotImplemented
                IncorrectState
                NoSuccess
        Notes:    
           - any subsequent method call on the object, besides close(), raises an "IncorrectState" exception.
        """

    
class Stream(Object, Async, Attributes, Monitorable):
    """
    This is the object that encapsulates all client Stream objects.

    Attributes supported:
    name: Bufsize
    desc: determines the size of the send buffer
    mode: ReadWrite, optional
    type: Int
    value: system dependend
    notes: - the implementation MUST document the default value, and its meaning (e.g. on what layer that buffer is maintained, or if it disables zero copy).
 
    name: Timeout
    desc: determines the amount of idle time before dropping the line, in seconds
    mode: ReadWrite, optional
    type: Int
    value: system dependend
    notes: - the implementation MUST document the default value
           - if this attribute is supported, the connection MUST be closed by the
             implementation if for that many seconds nothing has been read from or written to the stream.

    name: Blocking
    desc: determines if read/writes are blocking or not
    mode: ReadWrite, optional
    type: Bool
    value: True
    notes: - if the attribute is not supported, the implementation MUST be blocking
           - if the attribute is set to "True", a read or write operation MAY return immediately if
             no data can be read or written - that does not constitute an error (see EAGAIN in POSIX).

    name:  Compression
    desc:  determines if data are compressed before/after transfer
    mode: ReadWrite, optional
    type: Bool
    value: schema dependent
    notes: - the implementation MUST document the default values for the available schemas
    
    name:  Nodelay
    desc:  determines if packets are sent immediately, i.e. without delay
    mode: ReadWrite, optional
    type: Bool
    value: True
    notes: - similar to the TCP_NODELAY option
    
    name:  Reliable
    desc:  determines if all sent data MUST arrive
    mode:  ReadWrite, optional
    type:  Bool
    value: True
    notes: - if the attribute is not supported, the implementation MUST be reliable
  
    Metrics supported:
    name: stream.state
    desc: fires if the state of the stream changes, and has the value of the new state (ENUM)
    mode: ReadOnly
    unit: 1
    type: Enum
    value: New
    
    name: stream.read
    desc: fires if a stream gets readable
    mode: ReadOnly
    unit: 1
    type: Trigger
    value: 1
    notes: - a stream is considered readable if subsequent read() can sucessfully read 1 or more bytes of data.

    name:  stream.write
    desc:  fires if a stream gets writable
    mode:  ReadOnly
    unit:  1
    type:  Trigger
    value: 1
    notes: - a stream is considered writable if a subsequent write() can sucessfully write 1 or more units of data.

    name:  stream.exception
    desc:  fires if a stream has an error condition
    mode:  ReadOnly
    unit:  1
    type:  Trigger
    value: 1
    
    name:  stream.dropped
    desc:  fires if the stream gets dropped by the remote party
    mode: ReadOnly
    unit: 1
    type: Trigger
    value: 1
    
    """
    
    def __init__(self, session, url = ""):
        """
        Initializes a client stream for later connection to a server.
        
        session: Session handle
        url: server location as URL
        Post Condition: the state of the socket is "New".
        Perms: Query for the StreamService represented by url.
        Raises: NotImplemented
                IncorrectURL
                BadParameter
                PermissionDenied
                AuthorizationFailed
                AuthenticationFailed
                Timeout
                NoSuccess
        Notes: - server location and possibly protocol are described by the input URL.
               - the url parameter can be None (which is the default). A stream so constructed is only to be used
                 as parameter to an asynchronous StreamServer.serve() call. For such a stream, a later call to connect() will fail.
               - the implementation MUST ensure that the information given in the URL are usable - otherwise a BadParameter exception MUST be thrown.
               - the socket is only connected after the connect() method is called.

        """

# destructor the socket is closed  f the instance was not closed before, the destructor performs a close() on the instance, and all notes to close() apply.

    def get_url(self): 
        #out URL url
        """
        Get URL used for creating the stream
        url: the URL of the connection.
        Raises:   NotImplemented
                  IncorrectState
                  PermissionDenied
                  AuthorizationFailed
                  AuthenticationFailed
                  Timeout
                  NoSuccess
        Notes:    - returns a URL which can be passed to a stream constructor to create another
                    connection to the same stream_service.
                  - the returned url may be empty, indicating that this instance has been created with an empty
                    url as parameter to the stream CONSTRUCTOR().
        """
        
        return URL()
        
    def get_context(self):
        #out context ctx
        """
        Return remote authorization info
        context: remote context
        Pre Condition: the stream is, or has been, in the "Open" state.
        Post Condition: the returned context is deep copied, and does not share state with any other object.
        Raises:   NotImplemented
                  IncorrectState
                  PermissionDenied
                  AuthorizationFailed
                  AuthenticationFailed
                  Timeout
                  NoSuccess
        Notes: - the context returned contains the security information from the REMOTE party, and can be used for authorization.
               - if the stream is in a final state, but has been in "Open" state before, the returned context represents the remote party the stream has been connected to while it was in "Open" state.
               - if the stream is not in "Open" state, and is not in a final state after having been in "Open" state, an "IncorrectState" exception is thrown.
               - if no security information are available, the returned context has the type "Unknown" and no attributes are attached.
               - the returned context MUST be authenticated, or must be of type "Unknown" as described above.

        """
        return Context()
    
    def connect(self):
        """
        Establishes a connection to the target defined during the construction of the stream.
        Pre Condition: the stream is in "New" state.
        Post Condition: the stream is in "Open" state.
        Perms: Exec for the stream_service represented by the url used for creating this stream instance.
        Raises: NotImplemented
                IncorrectState
                PermissionDenied
                AuthorizationFailed
                AuthenticationFailed
                Timeout
                NoSuccess
        Notes: - on failure, the stream state is changed to "Error"
               - if the stream instance is not in "New" state, an "IncorrectState" exception is thrown.

        """

    def wait(self, what, timeout = -1.0):
        #in int what, in float timeout = -1.0, out int cause
        """
        Check if stream is ready for reading/writing, or if it has entered an error state.
        what: activity types to wait for
        timeout: number of seconds to wait
        cause: activity type causing the call to return
        Pre Condition: the stream is in "Open" state.
        Post Condition: the stream can be read from, or written to, or it is in "Error" state.
        Raises: NotImplemented
                IncorrectState
                PermissionDenied
                AuthorizationFailed
                AuthenticationFailed
                NoSuccess
         Notes: - wait will only check on the conditions specified by "what"
               - "what" is an integer representing OR"ed "Read", "Write", or "Exception" flags.
               - "cause" describes the availability of the socket (eg. OR"ed "Read", "Write", or "Exception")
               - if the stream is not in "Open" state, an "IncorrectState" exception is thrown.
        
        """
        cause = 0
        return cause
      
    def close(self, timeout = 0.0):
        """
        Closes an active connection
        timeout: seconds to wait
        Post Condition: stream is in "Closed" state
        Raises: NotImplemented
                IncorrectState
                NoSuccess
        Notes: - any subsequent method call on the object MUST raise an "IncorrectState" exception (apart from DESTRUCTOR and close()).
               - if close() is implicitely called in the DESTRUCTOR, it will never throw an exception.
               - close() can be called multiple times, with no side effects.
        """
        
        
    def read (self, buf, len = -1):
        #inout buffer buf, in int len_in = -1, out int len_out
        """
        Read a data buffer from stream.
        len: Maximum number of units that can be copied into the buffer.
        buf: buffer to store read data into
        len_out: number of bytes read, if successful.
        Pre Condition: the stream is in "Open" state.
        Post Condition: data from the stream are available in the buffer.
        Perms: Read for the stream_service represented by the url used for creating this stream instance.
        Raises: NotImplemented
                BadParameter
                IncorrectState
                PermissionDenied
                AuthorizationFailed
                AuthenticationFailed
                Timeout
                NoSuccess
        Notes: - if the stream is blocking, the call waits until data become available.
            - if the stream is non-blocking, the call returns immediately, even if no data are available -- that is not an error condition.
            - the actually number read into buffer is returned in len_out. It is not an error to read less bytes than requested, or in fact zero bytes.
            - errors are indicated by returning negative values for len_out, which correspond to negatives of the respective ERRNO error code
            - the given buffer must be large enough to store up to len_in, or managed by the implementation - otherwise a BadParameter exception is thrown.
            - the notes about memory management from the buffer class apply.
            - if len_in is smaller than 0, or not given, the buffer size is used for len_in. If that is also not available, a BadParameter exception is thrown.
            - if the stream is not in "Open" state, an "IncorrectState" exception is thrown.
            - similar to read (2) as specified by POSIX
        """
        len_out = 0
        return len_out
        
    def write(self, buf, len):
        #in buffer buf, in int len_in = -1, out int len_out
        """
        Write a data buffer to stream.
        len: number of units of data in the buffer
        buffer: buffer containing data that will be sent out via socket
        len_out: bytes written if successful
        Pre Condition: the stream is in "Open" state.
        Post Condition: the buffer data are written to the stream.
        Perms: Write for the stream_service represented by the url used for creating this stream instance.
        Raises: NotImplemented
                BadParameter
                IncorrectState
                PermissionDenied
                AuthorizationFailed
                AuthenticationFailed
                Timeout
                NoSuccess
        Notes:  - if the stream is blocking, the call waits until the data can be written.
                - if the stream is non-blocking, the call returns immediately, even if no data are written -- that is not an error condition.
                - it is not an error to write less than len bytes.
                - errors are indicated by returning negative values for len_out, which correspond to negatives of the respective ERRNO error code
                - the given buffer must be large enough to store up to len_in bytes, or managed by the implementation - otherwise a BadParameter exception is thrown.
                - the notes about memory management from the buffer class apply.
                - if len_in is smaller than 0, or not given, the buffer size is used for len_in. If that is also not available, a BadParameter exception is thrown.
                - if the stream is not in "Open" state, an "IncorrectState" exception is thrown.
                - similar to write (2) as specified by POSIX

        """
        
        len_out = 0
        return len_out