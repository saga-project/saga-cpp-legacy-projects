# Package: pysaga
# Module: rpc 
# Description: The module which specifies classes which handle remote procedure
#    calls.
# Specification and documentation can be found in section 4.5, page 302-312 of 
#    the GFD-R-P.90 document
# Author: P.F.A. van Zoolingen, Computer Systems Section, Faculty of 
#    Exact Science (FEW), Vrije Universiteit, Amsterdam, The Netherlands.

from buffer import Buffer
from object import Object, ObjectType
from permissions import Permissions
from task import Async, TaskType, Task
from session import Session


class IOMode(object):
    """
    IOMode specifies the modus of the L{Parameter} instances
    
    @version: 1.0, designed for Python 2.x    
    
    """
    
    IN    = 1 
    """
    @summary: The parameter is an input parameter: its initial value will be 
        evaluated, and its data buffer will not be changed during the invocation 
        of call().
        """
    
    OUT   = 2 
    """
    @summary: The parameter is an output parameter: its initial value will not 
        be evaluated, and its data buffer will likely be changed during the 
        invocation of call().
    """
    
    INOUT = 3
    """
    @summary: The parameter is input and output parameter: its initial value 
        will not evaluated, and its data buffer will likely be changed during 
        the invocation of call().
    """
    
class Parameter(Buffer):
    """
    The Parameter class inherits the L{Buffer} class, and adds one 
    additional state attribute: IOMode, which is read-only. With that addition, 
    the new class can conveniently be used to define input, inout and output 
    parameters for RPC calls.

    @summary: Parameter can be used to define input, inout and output parameters 
        for RPC calls.
    @version: 1.0, designed for Python 2.x

    """
    
    def __init__(self, data=None, size=-1, mode=IOMode.IN):
        """Initialize an parameter instance.
        
        @summary: Initialize an parameter instance.
        @param size: size of data to be used
        @type size: int
        @param mode: type of parameter
        @type mode: a value from IOMode
        @param data: data buffer to be used or other (see note)
        @type data: char array or list, or other (see note)
        @raise NotImplemented:
        @raise BadParameter:
        @raise NoSuccess:
        @note: If Parameter is initialized as Parameter(), it becomes an
            implementation managed IN Parameter, which data has yet to be set.
        @note: If data is an array or list of chars it works identical to Buffer
            or Iovec and creates an application managed buffer
        @note: For IN and INOUT Parameters, it is also possible to use normal 
            types, such as int, char, long, bool, float, str, or lists of them.
            Arrays of chars ('c') can be used if bytes are expected. Which 
            types, how and if they are interpreted is dependent of the 
            underlying implementation.
        @note: For OUT Parameters with size -1 it is possible to retreive
            normal types, such as int, long, bool, float, str, or lists or of 
            them, through the get_data() method after the call(). Which types 
            will be returned is dependent of the underlying implementation.
        @note: The mode value has to be initialized for each parameter, and size 
            and buffer values have to be initialized for each In and InOut 
            Parameter. For OUT parameters, size may have the value -1 in which 
            case the buffer is be un-allocated, and is to be created (e.g. 
            allocated) by the SAGA implementation upon arrival of the result 
            data, with a size sufficient to hold all result data. The size value 
            is to be set by the implementation to the allocated buffer size. 
        @note: When an Out or InOut Parameter uses a pre-allocated buffer, any 
            data exceeding the buffer size are discarded. The application is 
            responsible for specifying correct buffer sizes for pre-allocated 
            buffers; otherwise the behaviour is undefined.
        @Note: all notes from the L{Buffer.__init__()} apply.
        
        """
        
    def set_io_mode(self, mode):
        """Purpose: Set io mode.
        
        @summary: set io mode.
        @param mode: value for io mode
        @type mode: one of the values from IOMode
        
        """
        raise NotImplemented("This method is not yet implemented")
   
    def get_io_mode(self):
        """Retrieve the current value for io mode.
        
        @summary: Retrieve the current value for io mode.
        @return: value of io mode
        @rtype: one of the values from IOMode
        
        """
        raise NotImplemented("This method is not yet implemented")
        
        
    mode = property(get_io_mode, set_io_mode, doc="""The io mode
                                                    @type: int""")


   
class RPC(Object, Permissions, Async):
    """
    This class represents a remote function handle, which can be called 
    (repeatedly), and returns the result of the respective remote procedure 
    invocation.

    @version: 1.0, designed for Python 2.x

    """
     
     
    def __init__(self, funcname, session=Session(), tasktype=TaskType.NORMAL):
        """Initializes a remote function handle
        
        @summary: Initializes a remote function handle
        @param session: SAGA session to use
        @type session: L{Session} object
        @param funcname: name of remote method to initialize
        @type funcname: L{URL} 
        @param tasktype: return a normal RPC object or a Task object that 
            creates a RPC object in a final, RUNNING or NEW state. By default, 
            tasktype is L{TaskType.NORMAL}
        @type tasktype: value from L{TaskType}        
        @PostCondition: the instance is open.
        @Permission: Query
        @raises NotImplemented:
        @raise IncorrectURL:
        @raise BadParameter:
        @raise DoesNotExist:
        @raise PermissionDenied:
        @raise AuthorizationFailed:
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @note: if url is not given, or is empty (the default), the 
            implementation will choose an  appropriate default value.
        @note: according to the GridRPC specification, the constructor may or 
            may not contact the RPC server; absence of an exception does not 
            imply that following RPC calls will succeed, or that a remote 
            function handle is in fact available.
        @note: the following mapping is be applied from GridRPC errors to SAGA 
          exceptions:
            - GRPC_SERVER_NOT_FOUND   : BadParameter
            - GRPC_FUNCTION_NOT_FOUND : DoesNotExist
            - GRPC_RPC_REFUSED        : AuthorizationFailed
            - GRPC_OTHER_ERROR_CODE   : NoSuccess
        @note: non-GridRPC based implementations SHOULD ensure upon object 
            construction that the remote handle is available, for consistency 
            with the semantics on other SAGA object constructors.

        """

    def __del__(self):
        """Destroys the RPC object.
        
        @summary: Destroys the RPC object.
        @postcondition: the instance is closed.
        @Note: if the instance was not closed before, the destructor performs a 
            close() on the instance, and all notes to close() apply.
            
        """
    
    def call(self, parameters, tasktype=TaskType.NORMAL):
        """Call the remote procedure.
        
        @summary: Call the remote procedure.
        @param parameters: argument/result values for call
        @type parameters: list of Parameters
        @param tasktype: return the normal return values or a Task object in a 
            final, RUNNING or NEW state. By default, tasktype 
            is L{TaskType.NORMAL}
        @type tasktype: value from L{TaskType}
        @PreCondition: the instance is open.
        @PostCondition: the instance is avaiable for another call() invocation, 
            even if the present call did not yet finish, in the asynchronous 
            case.
        @Permission: Exec
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
        @note: according to the GridRPC specification, the RPC server might not 
            be contacted before invoking call(). For this reason, all notes to 
            the object constructor apply to the call() method as well.
        @note: if an implementation finds inconsistent information in the 
            Parameter list, a BadParameter exception is raised.
        @note: arbitrary backend failures (e.g. semantic  failures in the 
            provided parameter stack, or any errors occuring during the 
            execution of  the remote procedure) are mapped to a "NoSuccess" 
            exception, with a descriptive error message. That way, error 
            semantics of the SAGA implementation and of the RPC function 
            implementation are strictly distinguished.
        @note: the notes about memory management from the L{Buffer} 
            class apply.

        """
        raise NotImplemented("This method is not yet implemented")
        
    def close(self, timeout=0.0, tasktype=TaskType.NORMAL):
        """Closes the rpc handle instance.
        
        @summary: Closes the rpc handle instance.
        @param timeout: seconds to wait
        @type timeout: float
        @param tasktype: return the normal return values or a Task object in a 
            final, RUNNING or NEW state. By default, tasktype 
            is L{TaskType.NORMAL}
        @type tasktype: value from L{TaskType}
        @PostCondition: the instance is closed.
        @raise NotImplemented:
        @raise IncorrectState:
        @raise NoSuccess:
        @Note: any subsequent method call on the object raises an 
            "IncorrectState" exception (apart from DESTRUCTOR and close()).
        @note: if close() is implicitely called in the DESTRUCTOR, it will never 
            raise an exception.
        @note: close() can be called multiple times, with no side effects.
        @see: for resource deallocation semantics, see Section 2 of the 
            GFD-R-P.90 document
        @see: for timeout semantics, see Section 2 of the GFD-R-P.90 document

        """
        raise NotImplemented("This method is not yet implemented")
    
    