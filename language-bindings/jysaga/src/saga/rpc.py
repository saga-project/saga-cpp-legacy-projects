# Package: saga
# Module: rpc 
# Description: The module which specifies classes which handle remote procedure
#    calls.
# Specification and documentation can be found in section 4.5, page 302-312 of 
#    the GFD-R-P.90 document
# Author: P.F.A. van Zoolingen, Computer Systems Section, Faculty of 
#    Exact Science (FEW), Vrije Universiteit, Amsterdam, The Netherlands.

from saga.buffer import Buffer
from saga.object import Object, ObjectType
from saga.permissions import Permissions
from saga.task import Async, Task, TaskType
from saga.session import Session

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

import org.ogf.saga.rpc.IOMode

class IOMode(object):
    """
    IOMode specifies the modus of the saga.rpc.Parameter instances
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
    The Parameter class inherits the saga.buffer.Buffer class, and adds one 
    additional state attribute: IOMode, which is read-only. With that addition, 
    the new class can conveniently be used to define input, inout and output 
    parameters for RPC calls.

    @summary: Parameter can be used to define input, inout and output parameters 
        for RPC calls.
    """
    
    def __init__(self, size, mode, data = None):
        """
        Initialize an parameter instance.
        
            - B{Call format: Parameter( size, mode, data )}
                - B{Precondition:}
                    - size must be >= 0, mode must be one of the IOMode variables
                - B{Postcondition:}
                    - the memory is managed by the application.
            
            - B{Call format: Parameter( size, mode )}
                - B{Postcondition:}
                    - if "data" is not specified and size > 0, the memory is 
                        allocated by the implementation.
                    - if "data" is not specified, the memory is managed by the 
                        implementation.

        @summary: Initialize an parameter instance.
        @param size: size of data to be used
        @type size: int
        @param mode: type of parameter
        @type mode: a value from IOMode
        @param data: data buffer to be used
        @type data: char array or a list
        @raise NotImplemented:
        @raise BadParameter:
        @raise NoSuccess:
        @Note: all notes from the saga.buffer.Buffer.__init__() apply.
        """
        super(Parameter, self).__init__()
        
        
    def set_io_mode(self, mode):
        """
        Purpose: Set io mode
        @summary: set io mode
        @param mode: value for io mode
        @type mode: one of the values from IOMode
        """
        if mode is not IOMode.IN and mode is not IOMode.OUT and\
            mode is not IOMode.INOUT:
            raise BadParameter("Parameter mode is not a value from IOMode"\
                +" but " + str(mode) +" "+ str(mode__class__))

        try:
            if mode == IOMode.IN:
                self.delegateObject.setIOMode(org.ogf.saga.rpc.IOMode.IN)
            if mode == IOMode.OUT:
                self.delegateObject.setIOMode(org.ogf.saga.rpc.IOMode.OUT)
            if mode == IOMode.INOUT:
                self.delegateObject.setIOMode(org.ogf.saga.rpc.IOMode.INOUT)        
        except org.ogf.saga.error.SagaException, e:
            raise self.convertException(e)   

   
    def get_io_mode(self):
        """
        Retrieve the current value for io mode
        @summary: Retrieve the current value for io mode
        @return: value of io mode
        @rtype: one of the values from IOMode
        """
 
        try:
            mode = self.delegateObject.getIOMode()
            if mode == org.ogf.saga.rpc.IOMode.IN:
                return IOMode.IN
            if mode == org.ogf.saga.rpc.IOMode.OUT:
                return IOMode.OUT
            if mode == org.ogf.saga.rpc.IOMode.INOUT:
                return IOMode.INOUT        
        except org.ogf.saga.error.SagaException, e:
            raise self.convertException(e) 
   
class RPC(Object, Permissions, Async ):
    """
    This class represents a remote function handle, which can be called 
    (repeatedly), and returns the result of the respective remote procedure 
    invocation.
    """
     
     
    def __init__(self, funcname, session = Session() ):
        """
        Initializes a remote function handle
        @summary: Initializes a remote function handle
        @param session: saga session to use
        @type session: L{Session<saga.session.Session>} object
        @param funcname: name of remote method to initialize
        @type funcname: L{URL} 
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
        super(RPC, self).__init__()

    def __del__(self):
        """
        @postcondition: the instance is closed.
        @Note: if the instance was not closed before, the destructor performs a 
            close() on the instance, and all notes to close() apply.
        """
        Async.__del__()
        Permissions.__del__()
        Object.__del__() 
        try:
            self.close()
        except:
            pass
        supe
    
    def call(self, parameters, tasktype=TaskType.NORMAL):
        """
        Call the remote procedure.
        @summary: Call the remote procedure.
        @param parameters: argument/result values for call
        @type parameters: list of Parameters
        @param tasktype: return the normal return values or a Task object in a 
            final, RUNNING or NEW state. By default, type is L{TaskType.NORMAL}
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
        @note: the notes about memory management from the L{saga.buffer.Buffer} 
            class apply.

        """
        if tasktype is not TaskType.NORMAL and tasktype is not TypeTask.SYNC \
        and tasktype is not TaskType.ASYNC  and tasktype is not TypeTask.TASK:
            raise BadParameter, "Parameter tasktype is not one of the TypeTask"\
                +" values, but "+ str(tasttype)+"("+ str(tasktype.__class__)+")"
                
                       
    def close(self, timeout = 0.0, tasktype=TaskType.NORMAL):
        """
        Closes the rpc handle instance.
        @summary: Closes the rpc handle instance.
        @param timeout: seconds to wait
        @type timeout: float
        @param tasktype: return the normal return values or a Task object in a 
            final, RUNNING or NEW state. By default, type is L{TaskType.NORMAL}
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
        if tasktype is not TaskType.NORMAL and tasktype is not TypeTask.SYNC \
        and tasktype is not TaskType.ASYNC  and tasktype is not TypeTask.TASK:
            raise BadParameter, "Parameter tasktype is not one of the TypeTask"\
                +" values, but "+ str(tasttype)+"("+ str(tasktype.__class__)+")"
                
