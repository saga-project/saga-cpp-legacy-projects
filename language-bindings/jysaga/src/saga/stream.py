# Package: saga
# Module: stream 
# Description: The module which specifies classes which interact with streams
# Specification and documentation can be found in section 4.5, page 281-301 of 
#    the GFD-R-P.90 document
# Author: P.F.A. van Zoolingen, Computer Systems Section, Faculty of 
#    Exact Science (FEW), Vrije Universiteit, Amsterdam, The Netherlands.

from saga.object import Object, ObjectType
from saga.task import Async, TaskType, Task
from saga.monitoring import Monitorable
from saga.permissions import Permissions
from saga.url import URL
from saga.context import Context
from saga.attributes import Attributes
from saga.error import NotImplemented
from saga.session import Session
from saga.buffer import Buffer

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
import org.ogf.saga.stream.Stream
import org.ogf.saga.stream.StreamService
from org.ogf.saga.stream import StreamFactory
from org.ogf.saga.task import TaskMode
from org.ogf.saga.buffer import BufferFactory

class State(object):
    """
    State holds the possible states for a Stream.
    @summary: State holds the possible states for a Stream.
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
    """
      
    def __init__(self, url="", session=Session(), tasktype=TaskType.NORMAL, **impl ):
        """
        Initializes a new StreamService object.
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
        self.delegateObject = None
        if "delegateObject" in impl:
            if not isinstance(impl["delegateObject"], org.ogf.saga.stream.StreamService):
                raise BadParameter,"Parameter impl[\"delegateObject\"] is not" \
                    + " a org.ogf.saga.stream.StreamService. Type: " \
                    + str(impl["delegateObject"].__class__)
            self.delegateObject = impl["delegateObject"]
            return

        if (type(url) is not str and url != "") and not isinstance(url, URL):
            raise BadParameter, "Parameter url is not default or an URL, but "\
                + "a " + str(url.__class__)
        if not isinstance(session, Session):
            raise BadParameter, "Parameter session is not a Session, but "\
                + "a " + str(session.__class__)
        try:
            if url == "":
                self.delegateObject = \
                  StreamFactory.createStreamService(session.delegateObject)
            else:
                self.delegateObject = \
                  StreamFactory.createStreamService(session.delegateObject, \
                                                    url.delegateObject)
        except org.ogf.saga.error.SagaException, e:
            raise self.convertException(e)


    def get_url(self, tasktype=TaskType.NORMAL):
        """
        Get the URL to be used to connect to this server.
        @summary: Get the URL to be used to connect to this server.
        @param tasktype: return the normal return values or a Task object in a 
            final, RUNNING or NEW state. By default, tasktype is L{TaskType.NORMAL}
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
        if tasktype is not TaskType.NORMAL and tasktype is not TaskType.SYNC \
        and tasktype is not TaskType.ASYNC  and tasktype is not TaskType.TASK:
            raise BadParameter, "Parameter tasktype is not one of the TaskType"\
                +" values, but "+ str(tasttype)+"("+ str(tasktype.__class__)+")"
                
        #Normal get_url()
        if tasktype == TaskType.NORMAL:
            try:
                url =  self.delegateObject.getUrl()
                return URL(delegateObject = url) 
            except org.ogf.saga.error.SagaException, e:
                raise self.convertException(e)        
        
        #Asynchronous get_url()
        else:
            try:
                javaObject = None
                if tasktype is TaskType.ASYNC:
                    javaObject = self.delegateObject.getUrl(TaskMode.ASYNC)
                if tasktype is TaskType.SYNC:
                    javaObject = self.delegateObject.getUrl(TaskMode.SYNC)
                if tasktype is TaskType.TASK:
                    javaObject = self.delegateObject.getUrl(TaskMode.TASK)
                return Task(delegateObject=javaObject)                
            except org.ogf.saga.error.SagaException, e:
                raise self.convertException(e) 
            
    
    def serve(self, timeout = -1.0, tasktype=TaskType.NORMAL):
        """
        Wait for incoming client connections.
        @summary: Wait for incoming client connections.
        @param timeout: number of seconds to wait for a client
        @type timeout: float
        @param tasktype: return the normal return values or a Task object in a 
            final, RUNNING or NEW state. By default, tasktype is L{TaskType.NORMAL}
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
        if tasktype is not TaskType.NORMAL and tasktype is not TaskType.SYNC \
        and tasktype is not TaskType.ASYNC  and tasktype is not TaskType.TASK:
            raise BadParameter, "Parameter tasktype is not one of the TaskType"\
                +" values, but "+ str(tasttype)+"("+ str(tasktype.__class__)+")"
                
        #Normal serve()
        if tasktype == TaskType.NORMAL:
            if type(timeout) is not int and type(timeout) is not float:
                raise BadParameter, "Parameter timeout is not a float, but a "\
                    + str(timeout.__class__)
            try:
                if timeout == -1.0:
                    javaObject = self.delegateObject.serve()
                else:
                    javaObject = self.delegateObject.serve(timeout)    
                return Stream(delegateObject = javaObject) 
            except org.ogf.saga.error.SagaException, e:
                raise self.convertException(e)        
        
        #Asynchronous serve()
        else:
            if type(timeout) is not int and type(timeout) is not float:
                bp = BadParameter("Parameter timeout is not a float, but a "\
                    + str(timeout.__class__))
                return Task(error = bp)
            
            try:
              javaObject = None
              if timeout == -1.0:
                if tasktype is TaskType.ASYNC:
                  javaObject = self.delegateObject.serve(TaskMode.ASYNC)
                if tasktype is TaskType.SYNC:
                  javaObject = self.delegateObject.serve(TaskMode.SYNC)
                if tasktype is TaskType.TASK:
                  javaObject = self.delegateObject.serve(TaskMode.TASK)
              else:
                if tasktype is TaskType.ASYNC:
                  javaObject = self.delegateObject.serve(TaskMode.ASYNC,timeout)
                if tasktype is TaskType.SYNC:
                  javaObject = self.delegateObject.serve(TaskMode.SYNC,timeout)
                if tasktype is TaskType.TASK:
                  javaObject = self.delegateObject.serve(TaskMode.TASK,timeout)                    
                
              return Task(delegateObject=javaObject)                
            except org.ogf.saga.error.SagaException, e:
                raise self.convertException(e) 
        
    def close(self, timeout = 0.0, tasktype=TaskType.NORMAL):
        #in float timeout = 0.0
        """
        Closes a stream service
        @summary: closes a stream service
        @param timeout: seconds to wait
        @type timeout: float
        @param tasktype: return the normal return values or a Task object in a 
            final, RUNNING or NEW state. By default, tasktype is L{TaskType.NORMAL}
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
        if tasktype is not TaskType.NORMAL and tasktype is not TaskType.SYNC \
        and tasktype is not TaskType.ASYNC  and tasktype is not TaskType.TASK:
            raise BadParameter, "Parameter tasktype is not one of the TaskType"\
                +" values, but "+ str(tasttype)+"("+ str(tasktype.__class__)+")"

        #Normal close()
        if tasktype == TaskType.NORMAL:
            if type(timeout) is not int and type(timeout) is not float:
                raise BadParameter, "Parameter timeout is not a float, but a "\
                    + str(timeout.__class__)
            try:
                if timeout == -1.0:
                    self.delegateObject.close()
                else:
                    self.delegateObject.close(timeout)    
            except org.ogf.saga.error.SagaException, e:
                raise self.convertException(e)        
        
        #Asynchronous close()
        else:
            if type(timeout) is not int and type(timeout) is not float:
                bp = BadParameter("Parameter timeout is not a float, but a "\
                    + str(timeout.__class__))
                return Task(error = bp)
            
            try:
                javaObject = None
                if timeout == -1.0:
                    if tasktype is TaskType.ASYNC:
                        javaObject = self.delegateObject.close(TaskMode.ASYNC)
                    if tasktype is TaskType.SYNC:
                        javaObject = self.delegateObject.closee(TaskMode.SYNC)
                    if tasktype is TaskType.TASK:
                        javaObject = self.delegateObject.closee(TaskMode.TASK)
                else:
                  if tasktype is TaskType.ASYNC:
                    javaObject = self.delegateObject.close(TaskMode.ASYNC,timeout)
                  if tasktype is TaskType.SYNC:
                    javaObject = self.delegateObject.close(TaskMode.SYNC,timeout)
                  if tasktype is TaskType.TASK:
                    javaObject = self.delegateObject.close(TaskMode.TASK,timeout)                    
                return Task(delegateObject=javaObject)
            except org.ogf.saga.error.SagaException, e:
                raise self.convertException(e) 

    
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
            - note: if the attribute is not supported, the implementation is reliable
        
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
    buffer_size = 4096
    
    def __init__(self, url="", session=Session(), tasktype=TaskType.NORMAL, **impl):
        """
        Initializes a client stream for later connection to a server.
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
        if "delegateObject" in impl:
            if not isinstance(impl["delegateObject"], org.ogf.saga.stream.Stream):
                raise BadParameter,"Parameter impl[\"delegateObject\"] is not" \
                    + " a org.ogf.saga.stream.Stream. Type: " \
                    + str(impl["delegateObject"].__class__)
            self.delegateObject = impl["delegateObject"]
            return

        if (type(url) is not str and url != "") and not isinstance(url, URL):
            raise BadParameter, "Parameter url is not default or an URL, but "\
                + "a " + str(url.__class__)
        if not isinstance(session, Session):
            raise BadParameter, "Parameter session is not a Session, but "\
                + "a " + str(session.__class__)
        try:
            if url == "":
                self.delegateObject = \
                  StreamFactory.createStream(session.delegateObject)
            else:
                self.delegateObject = \
                  StreamFactory.createStream(session.delegateObject, \
                                                    url.delegateObject)
        except org.ogf.saga.error.SagaException, e:
            raise self.convertException(e)

    def __def__(self):
        """
        Destroy a Stream object.
        @summary: Destroy a Stream object.
        @note: If during the destruction of the object, the object was not 
            closed before, the destructor performs a close() on the instance, 
            and all notes to close() apply.
        """

    def get_url(self, tasktype=TaskType.NORMAL): 
        """
        Get URL used for creating the stream
        @summary: get URL used for creating the stream
        @param tasktype: return the normal return values or a Task object in a 
            final, RUNNING or NEW state. By default, tasktype is L{TaskType.NORMAL}
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
        if tasktype is not TaskType.NORMAL and tasktype is not TaskType.SYNC \
        and tasktype is not TaskType.ASYNC  and tasktype is not TaskType.TASK:
            raise BadParameter, "Parameter tasktype is not one of the TaskType"\
                +" values, but "+ str(tasttype)+"("+ str(tasktype.__class__)+")"
                
        #Normal get_url()
        if tasktype == TaskType.NORMAL:
            try:
                url = self.delegateObject.getUrl()
                return URL(delegateObject = url) 
            except org.ogf.saga.error.SagaException, e:
                raise self.convertException(e)        
        
        #Asynchronous get_url()
        else:
            try:
                javaObject = None
                if tasktype is TaskType.ASYNC:
                    javaObject = self.delegateObject.getUrl(TaskMode.ASYNC)
                if tasktype is TaskType.SYNC:
                    javaObject = self.delegateObject.getUrl(TaskMode.SYNC)
                if tasktype is TaskType.TASK:
                    javaObject = self.delegateObject.getUrl(TaskMode.TASK)
                return Task(delegateObject=javaObject)                
            except org.ogf.saga.error.SagaException, e:
                raise self.convertException(e) 

        
    def get_context(self, tasktype=TaskType.NORMAL):
        """
        Return remote authorization info
        @summary: return remote authorization info
        @param tasktype: return the normal return values or a Task object in a 
            final, RUNNING or NEW state. By default, tasktype is L{TaskType.NORMAL}
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
        if tasktype is not TaskType.NORMAL and tasktype is not TaskType.SYNC \
        and tasktype is not TaskType.ASYNC  and tasktype is not TaskType.TASK:
            raise BadParameter, "Parameter tasktype is not one of the TaskType"\
                +" values, but "+ str(tasttype)+"("+ str(tasktype.__class__)+")"
                
        #Normal get_context()
        if tasktype == TaskType.NORMAL:
            try:
                context = self.delegateObject.getContext()
                return Context(delegateObject = context) 
            except org.ogf.saga.error.SagaException, e:
                raise self.convertException(e)        
        
        #Asynchronous get_context()
        else:
            try:
                javaObject = None
                if tasktype is TaskType.ASYNC:
                    javaObject = self.delegateObject.getContext(TaskMode.ASYNC)
                if tasktype is TaskType.SYNC:
                    javaObject = self.delegateObject.getContext(TaskMode.SYNC)
                if tasktype is TaskType.TASK:
                    javaObject = self.delegateObject.getContext(TaskMode.TASK)
                return Task(delegateObject=javaObject)                
            except org.ogf.saga.error.SagaException, e:
                raise self.convertException(e) 
    
    def connect(self, tasktype=TaskType.NORMAL):
        """
        Establishes a connection to the target defined during the construction 
        of the stream.
        @summary: Establishes a connection to the target defined during the 
            construction of the stream.
        @param tasktype: return the normal return values or a Task object in a 
            final, RUNNING or NEW state. By default, tasktype is L{TaskType.NORMAL}
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
        if tasktype is not TaskType.NORMAL and tasktype is not TaskType.SYNC \
        and tasktype is not TaskType.ASYNC  and tasktype is not TaskType.TASK:
            raise BadParameter, "Parameter tasktype is not one of the TaskType"\
                +" values, but "+ str(tasttype)+"("+ str(tasktype.__class__)+")"
                
        #Normal connect()
        if tasktype == TaskType.NORMAL:
            try:
                context = self.delegateObject.connect()
            except org.ogf.saga.error.SagaException, e:
                raise self.convertException(e)        
        
        #Asynchronous connect()
        else:
            try:
                javaObject = None
                if tasktype is TaskType.ASYNC:
                    javaObject = self.delegateObject.connect(TaskMode.ASYNC)
                if tasktype is TaskType.SYNC:
                    javaObject = self.delegateObject.connect(TaskMode.SYNC)
                if tasktype is TaskType.TASK:
                    javaObject = self.delegateObject.connect(TaskMode.TASK)
                return Task(delegateObject=javaObject)                
            except org.ogf.saga.error.SagaException, e:
                raise self.convertException(e) 

    def wait(self, what, timeout = -1.0, tasktype=TaskType.NORMAL):
        """
        Check if stream is ready for reading/writing, or if it has entered an 
        error state.
        @summary: Check if stream is ready for reading/writing, or if it has 
            entered an error state
        @param what: activity type to wait for from L{Activity}
        @type what: int
        @param timeout: number of seconds to wait
        @type timeout: float
        @param tasktype: return the normal return values or a Task object in a 
            final, RUNNING or NEW state. By default, tasktype is L{TaskType.NORMAL}
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
        if tasktype is not TaskType.NORMAL and tasktype is not TaskType.SYNC \
        and tasktype is not TaskType.ASYNC  and tasktype is not TaskType.TASK:
            raise BadParameter, "Parameter tasktype is not one of the TaskType"\
                +" values, but "+ str(tasttype)+"("+ str(tasktype.__class__)+")"

        #Normal wait()
        if tasktype == TaskType.NORMAL:
#            if what is not Activity.READ and what is not Activity.WRITE \
#            and what is not Activity.EXCEPTION:
#                raise BadParameter, "Parameter what is not one of the Activity"\
#                +" values, but "+ str(what)+"("+ str(what.__class__)+")"
                
            if type(timeout) is not int and type(timeout) is not float:
                raise BadParameter, "Parameter timeout is not a float, but a "\
                    + str(timeout.__class__)
            try:
                if timeout == -1.0:
                    return self.delegateObject.waitFor(what)
                else:
                    return self.delegateObject.waitFor(what, timeout)    
            except org.ogf.saga.error.SagaException, e:
                raise self.convertException(e)        
        
        #Asynchronous wait()
        else:
            if type(timeout) is not int and type(timeout) is not float:
                bp = BadParameter("Parameter timeout is not a float, but a "\
                    + str(timeout.__class__))
                return Task(error = bp)
 
#            if what is not Activity.READ and what is not Activity.WRITE \
#            and what is not Activity.EXCEPTION:
#                bp = BadParameter( "Parameter what is not one of the Activity"\
#                +" values, but "+ str(what)+"("+ str(what.__class__)+")" )
#                return Task(error = bp)
            
            try:
                javaObject = None
                if timeout == -1.0:
                    if tasktype is TaskType.ASYNC:
                      javaObject = self.delegateObject.waitFor(TaskMode.ASYNC,what)
                    if tasktype is TaskType.SYNC:
                      javaObject = self.delegateObject.waitFor(TaskMode.SYNC,what)
                    if tasktype is TaskType.TASK:
                      javaObject = self.delegateObject.waitFor(TaskMode.TASK,what)
                else:
                  if tasktype is TaskType.ASYNC:
                    javaObject = self.delegateObject.waitFor(TaskMode.ASYNC,what,timeout)
                  if tasktype is TaskType.SYNC:
                    javaObject = self.delegateObject.waitFor(TaskMode.SYNC,what,timeout)
                  if tasktype is TaskType.TASK:
                    javaObject = self.delegateObject.waitFor(TaskMode.TASK,what,timeout)                    
                return Task(delegateObject=javaObject)
            except org.ogf.saga.error.SagaException, e:
                raise self.convertException(e) 

      
    def close(self, timeout = 0.0, tasktype=TaskType.NORMAL):
        """
        Closes an active connection
        @summary: Closes an active connection.
        @param timeout: seconds to wait
        @type timeout: float
        @param tasktype: return the normal return values or a Task object in a 
            final, RUNNING or NEW state. By default, tasktype is L{TaskType.NORMAL}
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
        if tasktype is not TaskType.NORMAL and tasktype is not TaskType.SYNC \
        and tasktype is not TaskType.ASYNC  and tasktype is not TaskType.TASK:
            raise BadParameter, "Parameter tasktype is not one of the TaskType"\
                +" values, but "+ str(tasttype)+"("+ str(tasktype.__class__)+")"

        #Normal close()
        if tasktype == TaskType.NORMAL:
            if type(timeout) is not int and type(timeout) is not float:
                raise BadParameter, "Parameter timeout is not a float, but a "\
                    + str(timeout.__class__)
            try:
                if timeout == -1.0:
                    self.delegateObject.close()
                else:
                    self.delegateObject.close(timeout)    
            except org.ogf.saga.error.SagaException, e:
                raise self.convertException(e)        
        
        #Asynchronous close()
        else:
            if type(timeout) is not int and type(timeout) is not float:
                bp = BadParameter("Parameter timeout is not a float, but a "\
                    + str(timeout.__class__))
                return Task(error = bp)
            
            try:
                javaObject = None
                if timeout == -1.0:
                    if tasktype is TaskType.ASYNC:
                        javaObject = self.delegateObject.close(TaskMode.ASYNC)
                    if tasktype is TaskType.SYNC:
                        javaObject = self.delegateObject.closee(TaskMode.SYNC)
                    if tasktype is TaskType.TASK:
                        javaObject = self.delegateObject.closee(TaskMode.TASK)
                else:
                  if tasktype is TaskType.ASYNC:
                    javaObject = self.delegateObject.close(TaskMode.ASYNC,timeout)
                  if tasktype is TaskType.SYNC:
                    javaObject = self.delegateObject.close(TaskMode.SYNC,timeout)
                  if tasktype is TaskType.TASK:
                    javaObject = self.delegateObject.close(TaskMode.TASK,timeout)                    
                return Task(delegateObject=javaObject)
            except org.ogf.saga.error.SagaException, e:
                raise self.convertException(e) 

               
        
    def read (self, size=-1, buf=None, tasktype=TaskType.NORMAL):
        #inout buffer buf, in int len_in = -1, out int len_out
        """
        Read up to size bytes of data from stream.

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
            final, RUNNING or NEW state. By default, tasktype is L{TaskType.NORMAL}
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
        if tasktype is not TaskType.NORMAL and tasktype is not TaskType.SYNC \
        and tasktype is not TaskType.ASYNC  and tasktype is not TaskType.TASK:
            raise BadParameter( "Parameter tasktype is not one of the TaskType"\
                               +"values, but " + str(tasktype))

        
        if tasktype == TaskType.NORMAL:
            if type (size)is not int:
                raise BadParameter, "Parameter size is not an int. Type: "\
                    + str(type(size))
            if buf.__class__ is not Buffer and buf is not None:
                raise BadParameter, "Parameter buf is not a Buffer. Class: " \
                    + str(buf.__class__)
            
            try:
                if size != -1 and buf is not None:
                    retval = self.delegateObject.read(buf.delegateObject, size)
                    if buf.managedByImp is False:
                        buf.update_data()
                    return retval
                elif size != -1 and buf is None:
                    javaBuffer =  BufferFactory.createBuffer(size)
                    retval = self.delegateObject.read(javaBuffer, size)
                    return retval.getData().toString()   
                elif size == -1 and buf is None:             
                    javaBuffer =  BufferFactory.createBuffer(self.buffer_size)
                    retval = self.delegateObject.read(javaBuffer, self.buffer_size)
                    return javaBuffer.getData().tostring()   
                else:
                    raise BadParameter("Correct call is read(size,data), "\
                                       +"read(size) or read() ")  
            except org.ogf.saga.error.SagaException, e:
                raise self.convertException(e)        
        
        #Asynchronous read()
        else:
            if type (size)is not int:
                bp = BadParameter( "Parameter size is not an int. Type: "\
                    + str(type(size)))
                return Task(error=bp)
            if buf.__class__ is not Buffer and buf is not None:
                bp = BadParameter("Parameter buf is not a Buffer. Class: " \
                    + str(buf.__class__))
                return Task(error=bp)
        
            try:
                javaObject = None
                if size != -1 and buf is not None:
                    if tasktype is TaskType.ASYNC:
                        javaObject = self.delegateObject.read(TaskMode.ASYNC, buf.delegateObject, size)
                    if tasktype is TaskType.SYNC:
                        javaObject = self.delegateObject.read(TaskMode.SYNC, buf.delegateObject, size)
                    if tasktype is TaskType.TASK:
                        javaObject = self.delegateObject.read(TaskMode.TASK, buf.delegateObject, size)
                    return Task(delegateObject=javaObject, fileReadBuffer = buf)        
                elif size != -1 and buf is None:
                    javaBuffer =  BufferFactory.createBuffer(size)
                    if tasktype is TaskType.ASYNC:
                        javaObject = self.delegateObject.read(TaskMode.ASYNC, javaBuffer, size)
                    if tasktype is TaskType.SYNC:
                        javaObject = self.delegateObject.read(TaskMode.SYNC, javaBuffer, size)
                    if tasktype is TaskType.TASK:
                        javaObject = self.delegateObject.read(TaskMode.TASK, javaBuffer, size)
                    return Task(delegateObject=javaObject, fileReadBuffer=javaBuffer)        
                elif size == -1 and buf is None:
                #- B{Call format: read()} 
                    javaBuffer =  BufferFactory.createBuffer(self.buffer_size)
                    if tasktype is TaskType.ASYNC:
                        javaObject = self.delegateObject.read(TaskMode.ASYNC, javaBuffer)
                    if tasktype is TaskType.SYNC:
                        javaObject = self.delegateObject.read(TaskMode.SYNC, javaBuffer)
                    if tasktype is TaskType.TASK:
                        javaObject = self.delegateObject.read(TaskMode.TASK, javaBuffer)
                    return Task(delegateObject=javaObject, fileReadBuffer=javaBuffer)        
                else:
                    bp = BadParameter("Correct call is read(size,data), "\
                                        +"read(size) or read() ")  
                    return Task(error = bp)
            except org.ogf.saga.error.SagaException, e:
                raise self.convertException(e)
#DOCUMENT: SagaIOException deviation
 
        
    def write(self, buf, size = -1, tasktype=TaskType.NORMAL):
        #in buffer buf, in int size_in = -1, out int size_out
        """
        Write a data buffer to stream
        @summary: write a data buffer to stream
        @param size: number of units of data in the buffer
        @type size: int
        @param buf: buffer containing data that will be sent out via socket
        @type buf: L{Buffer} or string
        @param tasktype: return the normal return values or a Task object in a 
            final, RUNNING or NEW state. By default, tasktype is L{TaskType.NORMAL}
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
        if tasktype is not TaskType.NORMAL and tasktype is not TaskType.SYNC \
        and tasktype is not TaskType.ASYNC  and tasktype is not TaskType.TASK:
            raise BadParameter, "Parameter tasktype is not one of the TaskType values, but " + str(tasktype)
        if tasktype == TaskType.NORMAL:
            if type (size)is not int:
                raise BadParameter, "Parameter size is not an int. Type: " \
                    + str(type(size))
            if buf.__class__ is not Buffer and type(buf) != str:
                raise BadParameter, "Parameter buf is not a Buffer or string.\
                         Class: " + str(buf.__class__)
            if type(buf) == str:
                import java.lang.String
                byte_array = java.lang.String(buf).getBytes()
                buf = Buffer(delegateObject=BufferFactory.createBuffer(byte_array))
 
            if size is -1:
                return self.delegateObject.write(buf.delegateObject)
            else:
                return self.delegateObject.write(buf.delegateObject, size)

        try:
            if type (size)is not int:
                bp = BadParameter( "Parameter size is not an int. Type: " \
                    + str(type(size)))
                return Task(error=bp)
            if buf.__class__ is not Buffer and type(buf) != str:
                bp = BadParameter("Parameter buf is not a Buffer. Class: " \
                    + str(buf.__class__))
                return Task(error=bp)
            if size < -1:
                bp = BadParameter( "Parameter size < 0")
                return Task(error=bp)
            
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

        except org.ogf.saga.error.SagaException, e:
                raise self.convertException(e)

    def __set_Bufsize(self, value):
        self.delegateObject.set_attribute("Bufsize", value)
        
    def __get_Bufsize(self):
        return self.delegateObject.get_attribute("Bufsize")  

    Bufsize = property(__get_Bufsize, __set_Bufsize,
            doc="""The Bufsize attribute. \n@type: int""")

    def __set_Timeout(self, value):
        self.delegateObject.set_attribute("Timeout", value)
        
    def __get_Timeout(self):
        return self.delegateObject.get_attribute("Timeout")  

    Timeout = property(__get_Timeout, __set_Timeout,
            doc="""The Timeout attribute. \n@type: int""")


    def __set_Blocking(self, value):
        self.delegateObject.set_attribute("Blocking", value)
        
    def __get_Blocking(self):
        return self.delegateObject.get_attribute("Blocking")  

    Blocking = property(__get_Blocking, __set_Blocking,
            doc="""The Blocking attribute. \n@type: bool""")

    def __set_Compression(self, value):
        self.delegateObject.set_attribute("Compression", value)
        
    def __get_Compression(self):
        return self.delegateObject.get_attribute("Compression")  

    Compression = property(__get_Compression, __set_Compression, 
                           doc="""The Compression attribute. \n@type: bool""")

    def __set_Nodelay(self, value):
        self.delegateObject.set_attribute("Nodelay", value)
        
    def __get_Nodelay(self):
        return self.delegateObject.get_attribute("Nodelay")  
    
    Nodelay = property(__get_Nodelay, __set_Nodelay,
            doc="""The Nodelay attribute. \n@type: bool""")


    def __set_Reliable(self, value):
        self.delegateObject.set_attribute("Reliable", value)
        
    def __get_Reliable(self):
        return self.delegateObject.get_attribute("Reliable")  

    Reliable = property(__get_Reliable, __set_Reliable, 
            doc="""The Reliable attribute. \n@type: bool""")
     
  