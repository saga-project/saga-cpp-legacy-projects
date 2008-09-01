# Page 302
#  package saga.rpc
from buffer import Buffer
from object import Object, ObjectType

class IOMode(object):
    """
    IOMode specifies the modus of the saga.rpc.Parameter instances
    """
    
    IN    = 1 
    """The parameter is an input parameter: its initial value will be evaluated, and its data buffer will not be changed during the invocation of call()."""
    OUT   = 2 
    """The parameter is an output parameter: its initial value will not be evaluated, and its data buffer will likely be changed during the invocation of call()."""
    INOUT = 3
    """The parameter is input and output parameter: its initial value will not evaluated, and its data buffer will likely be changed during the invocation of call()."""
    
class Parameter(Buffer):
    """
    The parameter class inherits the saga.buffer.Buffer class, and adds one additional
    state attribute: IOMode, which is read-only. With that addition, the new class
    can conveniently be used to define input, inout and output parameters for RPC calls.
    """
    
    def __init__(self, size, mode, data):
        #in array<byte> data = "", in int size = 0, in io_mode mode = In, out buffer obj
        """
        Create an parameter instance
        @summary: Create an parameter instance

        @param size: size of data to be used
        @type size: int
        @param mode: type of parameter
        @type mode: a value from IOMode
        @param data: data buffer to be used
        @type data: B{TO DO}
        @raise NotImplemented:
        @raise BadParameter:
        @raise NoSuccess:
        @Note: all notes from the saga.buffer.Buffer.__init__() apply.
        """
        super(Parameter, self).__init__()
        
        
    def set_io_mode(self, mode):
        #in io_mode mode
        """
        Purpose: Set io mode
        @summary: set io mode
        @param mode: value for io mode
        @type mode: one of the values from IOMode
        """
   
    def get_io_mode(self):
        #out io_mode mode
        """
        Retrieve the current value for io mode
        @summary: Retrieve the current value for io mode
        @return: value of io mode
        @rtype: one of the values from IOMode
        """
   
   
class RPC(Object, Permissions):
    """
    This class represents a remote function handle, which can be called (repeatedly),
    and returns the result of the respective remote procedure invocation.

    """
     
     
    def __init__(self, session, url):
        #in session s, in saga::url url = "", out rpc obj
        """
        Initializes a remote function handle
        @summary: Initializes a remote function handle
        @param session: saga session to use
        @type session: L{Session<saga.session.Session>} object
        @param url: name of remote method to initialize (funcname?)
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
        @note: if url is not given, or is empty (the default), the implementation will choose an  appropriate default value.
        @note: according to the GridRPC specification, the constructor may or may not contact the RPC
              server; absence of an exception does not imply that following RPC calls will succeed, or that
              a remote function handle is in fact available.
        @note: the following mapping MUST be applied from GridRPC errors to SAGA exceptions:
            - GRPC_SERVER_NOT_FOUND   : BadParameter
            - GRPC_FUNCTION_NOT_FOUND : DoesNotExist
            - GRPC_RPC_REFUSED        : AuthorizationFailed
            - GRPC_OTHER_ERROR_CODE   : NoSuccess
        @note: non-GridRPC based implementations SHOULD ensure upon object construction that the remote handle
              is available, for consistency with the semantics on other SAGA object constructors.

        """
        super(RPC, self).__init__()
        
#DESTRUCTOR (in     rpc               obj           );
#DESTRUCTOR Purpose: destroy the object Format: DESTRUCTOR (in rpc obj)
#Inputs:   obj:                  the object to destroy
#PostCond: - the instance is closed.
#Notes:    - if the instance was not closed before, the destructor performs a close() on the instance, and all notes to close() apply.

    #rpc method invocation
    def call(self, parameters):
        #inout array<parameter>  parameters
        """
        Call the remote procedure
        @summary: Call the remote procedure
        Format:   call         (inout array<parameter> param);
        @param parameters: argument/result values for call
        @type parameters: list
        @PreCondition: the instance is open.
        @PostCondition: the instance is avaiable for another call() invocation, even if the present call did not
            yet finish, in the asynchronous case.
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
        @note: according to the GridRPC specification, the RPC server might not be contacted before
            invoking call(). For this reason, all notes to the object constructor apply to the call()
            method as well.
        @note: if an implementation finds inconsistent information in the parameter vector, a
            BadParameter exception is thrown.
        @note: arbitrary backend failures (e.g. semantic  failures in the provided parameter stack, or
                 any errors occuring during the execution of  the remote procedure) MUST be mapped to a
                 "NoSuccess" exception, with a descriptive error message. That way, error semantics of
                 the SAGA implementation and of the RPC function implementation are strictly distinguished.
        @note: the notes about memory management from the L{saga.buffer.Buffer} class apply.

        """
        
    #handle management
    def close(self, timeout = 0.0):
        #in float timeout = 0.0
        """
        Closes the rpc handle instance
        @summary: Closes the rpc handle instance
        Format:   close              (in float timeout = 0.0);
        @param timeout: seconds to wait
        @type timeout: float
        @PostCondition: the instance is closed.
        @raise NotImplemented:
        @raise IncorrectState:
        @raise NoSuccess:
        @Note: any subsequent method call on the object MUST raise an "IncorrectState" exception (apart from DESTRUCTOR and close()).
        @note: if close() is implicitely called in the DESTRUCTOR, it will never throw an exception.
        @note: close() can be called multiple times, with no side effects.
        @see: for resource deallocation semantics, see Section 2 of the GFD-R-P.90 document
        @see: for timeout semantics, see Section 2 of the GFD-R-P.90 document

        """
