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
from saga.task import Async, TaskType, Task
from saga.session import Session
from saga.error import NotImplemented
from saga.url import URL

import array.array
import jarray.array
import jarray.zeros
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
import org.ogf.saga.rpc.Parameter
from org.ogf.saga.rpc import RPCFactory
from org.ogf.saga.task import TaskMode

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
    
class Parameter(Buffer, Object):
    """
    The Parameter class inherits the saga.buffer.Buffer class, and adds one 
    additional state attribute: IOMode, which is read-only. With that addition, 
    the new class can conveniently be used to define input, inout and output 
    parameters for RPC calls.

    @summary: Parameter can be used to define input, inout and output parameters 
        for RPC calls.
    """
    
    #TODO: buffer methods.
    def __init__(self, data=None, size=-1, mode=IOMode.IN, **impl):
        """
        Initialize an parameter instance.
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
        @Note: all notes from the saga.buffer.Buffer.__init__() apply.
        """
        self.delegateObject = None
        self.managedByImp = None
        self.implementation_data = None
        #self.application_data = None
        if "delegateObject" in impl:
            if not isinstance(impl["delegateObject"], org.ogf.saga.rpc.Parameter):
                raise BadParameter("Parameter impl[\"delegateObject\"] is not a "\
                                   +"org.ogf.saga.rpc.Parameter. Type: " \
                                   + str(impl["delegateObject"].__class__))
            self.delegateObject = impl["delegateObject"]
            return
        if type(size) is not int:
                raise Badparameter, "Parameter size is not an int. Type:", \
                    str(type(size))
        if size < -1:
            raise BadParameter, "Parameter size is < 0"
        if mode is not IOMode.IN and mode is not IOMode.OUT and\
            mode is not IOMode.INOUT:
            raise BadParameter("Parameter mode is not a value from IOMode"\
                +" but " + str(mode) +" "+ str(mode__class__))

        try:
            if mode == IOMode.OUT:
                mode_parameter = org.ogf.saga.rpc.IOMode.OUT
            elif mode == IOMode.INOUT:
                mode_parameter = org.ogf.saga.rpc.IOMode.INOUT
            else: # mode == IOMode.IN:
                mode_parameter = org.ogf.saga.rpc.IOMode.IN

            self.delegateObject = RPCFactory.createParameter(mode_parameter)
            if data != None:
                self.set_data(data, size)

        except org.ogf.saga.error.SagaException, e:
                raise self.convertException(e)                
 
        
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
 
    
    def set_data(self, data, size=-1):
        #in array<byte>  data, in int size
        """
        Set new buffer data.
        @summary: Set new buffer data.
        @param data: data to be used in buffer
        @type data: char array or a list
        @param size: size of given data
        @type size: int
        @PostCondition: the buffer memory is managed by the application.
        @Raise NotImplemented:
        @raise BadParameter:
        @raise IncorrectState:
        @Note: the method is semantically equivalent to
               destroying the buffer, and re-creating it with
               the first __init__ call format with the given size.
        @note: the notes for __del__ and the first __init__ call format apply.
        """
        #PARAMETER CHECKS
        if size < 1 and size != -1:
            raise BadParameter, "Parameter size is < 1" 
        if type(size) is not int:
            raise BadParameter, "Parameter size is not an int. Type:"\
                +str(size.__class__)

        #IF DATA IS NONE
        if data == None:
            try:            
                self.delegateObject.setData(data)
                return
            except org.ogf.saga.error.SagaException, e:
                raise self.convertException(e)

        #IF DATA IS ARRAY('c')
        elif type(data) is array.array:
            if data.typecode == 'c':
                self.implementation_data = jarray.zeros(len(data), 'b')
            else:
                raise BadParameter, "Typecode "+data.typecode+" not supported"    
            
            try: 
                for i in range(len(self.implementation_data)):
                    if data[i] > chr(127) and data[i] < chr(256):
                        self.implementation_data[i] = ord(data[i]-256)
                    else:
                        self.implementation_data[i] = ord(data[i])             
            except OverflowError:
                raise BadParameter("Parameter data contained contained outside"\
                                   +" domain chr(0) and chr(255)")
            try:
                self.delegateObject.setData(self.implementation_data)
#                self.application_data = data
            except org.ogf.saga.error.SagaException, e:
                raise self.convertException(e)            

        #IF DATA IS LIST
        elif type(data) is list:
            if len(data) == 0:
                    self.implementation_data = None            
            elif type(data[0]) is int:
                if(max(data)==1 or max(data)==0)and(min(data)==1 or min(data)==0):
                   try:
                       self.implementation_data = jarray.array(data,'z')
                   except TypeError, e:
                       raise BadParameter,"Parameter data contains mulitple"\
                                      +" types instead of only True or False" 
                else:
                    try:
                        self.implementation_data = jarray.array(data,'i')
                    except TypeError, e:
                        raise BadParameter,"Parameter data contains mulitple"\
                                      +" types instead of only int"
            elif type(data[0]) is long:
                try:
                    self.implementation_data = jarray.array(data,'l')
                except TypeError, e:
                    raise BadParameter,"Parameter data contains mulitple"\
                                      +" types instead of only long"
            elif type(data[0]) is float:
                try:
                    self.implementation_data = jarray.array(data,'f')
                except TypeError, e:
                    raise BadParameter,"Parameter data contains mulitple"\
                                      +" types instead of only float"
            elif type(data[0]) is str:
                len_OK = True
                try:
                    for i in range(len(data)):
                        if len(data[i]) > 1:
                            len_OK = False
                            break 
                except:
                    raise BadParameter,"Parameter data contains mulitple "\
                                      +"types instead of only str or characters"
                if len_OK:
                    try:
                        self.implementation_data = jarray.array(data,'c')
                    except TypeError, e:
                        raise BadParameter,"Parameter data contains mulitple"\
                                      +" types instead of only characters" 
                else:
                    import java.lang.String                   
                    try:
                        self.implementation_data = jarray.array(data,java.lang.String)
                    except TypeError, e:
                        raise BadParameter,"Parameter data contains mulitple"\
                                      +" types instead of only str"
            else:
                return BadParameter("Type"+ str(data[0].__class__)+\
                                    " not supported)")
            try:
                self.delegateObject.setData(self.implementation_data)
#                self.application_data = data
            except org.ogf.saga.error.SagaException, e:
                raise self.convertException(e)  
        
        #IF DATA IS A NORMAL TYPE                         
        elif type(data) is int or type(data) is long or type(data) is float\
                or type(data) is str:
                try: 
                    self.delegateObject.setData(data)
                    self.implementation_data = data
#                    self.application_data = data
                except org.ogf.saga.error.SagaException, e:
                    raise self.convertException(e) 
        #IF DATA IS A BOOL (does not exist in jython yet)
        elif type(data) is bool:
                try: 
                    self.delegateObject.setData(data)
                    self.implementation_data
#                    self.application_data = data
                except org.ogf.saga.error.SagaException, e:
                    raise self.convertException(e)                 
        else: #ANY OTHER TYPE: UNKNOWN CONSEQUENCES
                 try: 
                    self.delegateObject.setData(data)
                    self.implementation_data = data
#                    self.application_data = data
                 except org.ogf.saga.error.SagaException, e:
                    raise self.convertException(e)                                                                                       
    
                             
    def get_data(self):
        #out array<byte> data
        """
        Retrieve the buffer data.
        @summary: Retrieve the buffer data.
        @return: buffer data to retrieve. Type depends on what type was used to create the Buffer object
        @rtype: char array or list
        @Raise NotImplemented:
        @raise DoesNotExist:
        @raise IncorrectState:
        @Note: see notes about memory management in the GFD-R-P.90 document
        @note: if the buffer was created as implementation
                    managed (size == -1), but no I/O operation has
                    yet been successfully performed on the buffer,
                    a 'DoesNotExist' exception is raised.
        """
        temp = None
        try:
            temp = self.delegateObject.getData()
        except org.ogf.saga.error.SagaException, e:
            raise self.convertException(e)

        if temp == None:
            return None
 
        if type(temp) is int or type(temp) is long or type(temp) is float\
                or type(temp) is str: 
            return temp
        
        arr = jarray.zeros(1, 'c')
        if temp.__class__ == arr.__class__:
            if len(temp) == 0:
                return []
 
            if type(temp[0]) == int:
                #temp.typecode == 'b':
                retval = []
                for i in range(len(temp)):
                    if temp[i] < 0:
                        retval.append( chr(self.implementation_data[i]+256) )
                    else:
                        retval.append( chr(self.implementation_data[i]) )
                return retval
            
            if temp.typecode == 'z' or temp.typecode == 'i' or \
               temp.typecode == 'i' or temp.typecode == 'l' or \
               temp.typecode == 'f' or temp.typecode == 'c':
                return list(temp)

            import java.lang.String
            if temp.typecode == 'java.lang.String':
                retval = []
                for i in range(len(temp)):
                    retval.append( temp[i].toString() ) 
                return retval             

    def close(self):
        raise NotImplemented("close() is not implemented in Parameter")
    
    def get_size(self):
        raise NotImplemented("get_size() is not implemented in Parameter")
 
    def set_size(self, size=-1):
        raise NotImplemented("set_size() is not implemented in Parameter")


    def get_type(self):
        """
        Query the object type.
        @summary: Query the object type.
        @return: type of the object as an int from ObjectType
        @rtype: int
        """
        return ObjectType.PARAMETER

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
        from saga.error import NotImplemented
        raise NotImplemented("clone() is not implemented")
        
#        try:
#            javaClone = self.delegateObject.clone()
#            temp = Buffer(delegateObject = javaClone)
#            temp.managedByImp = self.managedByImp
#            if self.implementation_data is None:
#                temp.array = None
#            else:
#                temp.array = jarray.array(self.implementation_data, 'b')
#            #TODO: check clone and buffer behaviour -> Java Data copying? Set data?
#            temp.applicationBuf = self.application_data
#            temp.closed = self.closed
#            return temp
#        except org.ogf.saga.error.SagaException, e:
#            raise self.convertException(e)

    def get_session(self):
        """
        Query the objects session.
        @summary: Query the objects session.
        @return: session of the object
        @rtype: L{Session}
        @PreCondition: the object was created in a session, either
            explicitly or implicitly.
        @PostCondition: the returned session is shallow copied.
        @raise DoesNotExist:
        @Note: if no specific session was attached to the object at creation time, 
            the default SAGA session is returned.
        @note: some objects do not have sessions attached, such as JobDescription, Task, Metric, and the
            Session object itself. For such objects, the method raises a 'DoesNotExist' exception.
        """
        from saga.error import NotImplemented
        raise NotImplemented("get_session() is not implemented")

#        from saga.session import Session
#        try:
#            tempSession = self.delegateObject.getSession()
#            session = Session(sessionObject=tempSession)
#            return session
#        except org.ogf.saga.error.SagaException, e:
#            raise self.convertException(e)
 
    def get_id(self):
        """
        Query the object ID.
        @summary: Query the object ID.
        @return: uuid for the object
        @rtype: string 
        """
        from saga.error import NotImplemented
        raise NotImplemented("get_id is not implemented")
#        try:
#            return self.delegateObject.getId()
#        except org.ogf.saga.error.SagaException, e:
#           raise self.convertException(e) 


    mode = property(get_io_mode, set_io_mode, doc="""The io mode
                                                    @type: int""")
    data = property(get_data,set_data, doc="""The data
                                            @type: see set_data()""")
    size = property(get_size,set_size,
            doc="""The size of the buffer
                @type: int""")    
    
    
class RPC(Object, Permissions, Async ):
    """
    This class represents a remote function handle, which can be called 
    (repeatedly), and returns the result of the respective remote procedure 
    invocation.
    """
     
     
    def __init__(self, funcname, session=Session(), tasktype=TaskType.NORMAL, **impl):
        """
        Initializes a remote function handle
        @summary: Initializes a remote function handle
        @param session: saga session to use
        @type session: L{Session<saga.session.Session>} object
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
        if "delegateObject" in impl:
            if not isinstance(impl["delegateObject"], org.ogf.saga.rpc.RPC):
                raise BadParameter("Parameter impl[\"delegateObject\"] is not a "\
                                   +"org.ogf.saga.rpc.RPC. Type: " \
                                   + str(impl["delegateObject"].__class__))
            self.delegateObject = impl["delegateObject"]
            return
        if not isinstance(funcname, URL):
            raise BadParameter("Parameter funcname is not an URL")
        if not isinstance(session, Session):
            raise BadParameter("Parameter session is not a Session")        

        try:
            self.delegateObject = RPCFactory.createRPC(session.delegateObject, funcname.delegateObject)
        except org.ogf.saga.error.SagaException, e:
            raise self.convertException(e)
#TODO: gridrpc:// or http:// or any://

    def __del__(self):
        """
        Destroys the RPC object.
        @summary: Destroys the RPC object.
        @postcondition: the instance is closed.
        @Note: if the instance was not closed before, the destructor performs a 
            close() on the instance, and all notes to close() apply.
        """
        Async.__del__()
        Permissions.__del__()
        Object.__del__() 
        self.close()
    
    def call(self, parameters, tasktype=TaskType.NORMAL):
        """
        Call the remote procedure.
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
        @note: the notes about memory management from the L{saga.buffer.Buffer} 
            class apply.

        """
        if tasktype is not TaskType.NORMAL and tasktype is not TaskType.SYNC \
        and tasktype is not TaskType.ASYNC  and tasktype is not TaskType.TASK:
            raise BadParameter, "Parameter tasktype is not one of the TaskType"\
                +" values, but "+ str(tasttype)+"("+ str(tasktype.__class__)+")"
        
        #Normal call()
        if tasktype == TaskType.NORMAL: 
            temp = jarray.zeros(len(parameters), org.ogf.saga.rpc.Parameter)

            for i in range(len(parameters)):
                try:
                    temp[i] = parameters[i].delegateObject
                except:
                    raise BadParameter("Parameter parameters contains more"\
                                       "types than Parameter. "+i+": "\
                                        +str(parameters[i].__class__))
            try:           
                self.delegateObject.call( temp )
            except org.ogf.saga.error.SagaException, e:
                raise self.convertException(e) 
        
        #Asynchronous call()
        else:
            temp = jarray.zeros(len(parameters), org.ogf.saga.rpc.Parameter)

            for i in range(len(parameters)):
                try:
                    temp[i] = parameters[i].delegateObject
                except:
                    bp = BadParameter("Parameter parameters contains more"\
                                       "types than Parameter. "+i+": "\
                                        +str(parameters[i].__class__))            
            try:
                task = None
                if tasktype is TaskType.ASYNC:
                    task = self.delegateObject.call(TaskMode.ASYNC, temp)
                if tasktype is TaskType.SYNC:
                    task = self.delegateObject.call(TaskMode.SYNC, temp)
                if tasktype is TaskType.TASK:
                    task = self.delegateObject.call(TaskMode.TASK, temp)
                return Task(delegateObject = task)        
            except org.ogf.saga.error.SagaException, e:
                raise self.convertException(e)            

    def close(self, timeout = 0.0, tasktype=TaskType.NORMAL):
        """
        Closes the rpc handle instance.
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
        if tasktype is not TaskType.NORMAL and tasktype is not TaskType.SYNC \
        and tasktype is not TaskType.ASYNC  and tasktype is not TaskType.TASK:
            raise BadParameter, "Parameter tasktype is not one of the TaskType"\
                +" values, but "+ str(tasttype)+"("+ str(tasktype.__class__)+")"


        #Normal close()
        if tasktype == TaskType.NORMAL:
            if type(timeout) is not float and type(timeout) is not int:
                raise BadParameter, "Parameter timeout is not a float, but " +\
                                str(timeout.__class__)+")"
            
            try:
                if timeout == 0.0:
                    self.delegateObject.close()
                else:
                    self.delegateObject.close(timeout)
            except org.ogf.saga.error.SagaException, e:
                raise self.convertException(e)
 
        #Asynchronous close()
        else:
            if type(timeout) is not float and type(timeout) is not int:
                bp = BadParameter( "Parameter timeout is not a float, but " +\
                                str(timeout.__class__)+")")
                return Task(error = bp)
            try:
                task = None
                if tasktype is TaskType.ASYNC:
                    if timeout == 0.0:
                        task = self.delegateObject.close(TaskMode.ASYNC)
                    else:
                        task = self.delegateObject.close(TaskMode.ASYNC,timeout)
                if tasktype is TaskType.SYNC:
                    if timeout == 0.0:
                        task = self.delegateObject.close(TaskMode.SYNC)
                    else:
                        task = self.delegateObject.close(TaskMode.SYNC,timeout)                
                if tasktype is TaskType.TASK:
                    if timeout == 0.0:
                        task = self.delegateObject.close(TaskMode.TASK)
                    else:
                        task = self.delegateObject.close(TaskMode.TASK,timeout) 
                return Task(delegateObject = task)        
            except org.ogf.saga.error.SagaException, e:
                raise self.convertException(e)
 
    def get_id(self):
        """
        Query the object ID.
        @summary: Query the object ID.
        @return: uuid for the object
        @rtype: string 
        """
        try:
            return self.delegateObject.getId()
        except org.ogf.saga.error.SagaException, e:
           raise self.convertException(e)
      
    def get_type(self):
        """
        Query the object type.
        @summary: Query the object type.
        @return: type of the object as an int from ObjectType
        @rtype: int
        """
        return ObjectType.RPC
      
    def get_session(self):
        """
        Query the objects session.
        @summary: Query the objects session.
        @return: session of the object
        @rtype: L{Session}
        @PreCondition: the object was created in a session, either
            explicitly or implicitly.
        @PostCondition: the returned session is shallow copied.
        @raise DoesNotExist:
        @Note: if no specific session was attached to the object at creation time, 
            the default SAGA session is returned.
        @note: some objects do not have sessions attached, such as JobDescription, Task, Metric, and the
            Session object itself. For such objects, the method raises a 'DoesNotExist' exception.
        """
        from saga.session import Session
        try:
            tempSession = self.delegateObject.getSession()
            session = Session(sessionObject=tempSession)
            return session
        except org.ogf.saga.error.SagaException, e:
            raise self.convertException(e)
    
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
            clone = RPC(funcname="", delegateObject=javaClone)
            return clone
        except org.ogf.saga.error.SagaException, e:
            raise self.convertException(e)               
        
        
