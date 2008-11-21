# Package: saga
# Module: buffer 
# Description: The module which specifies the buffer used in saga
# Specification and documentation can be found in section 3.4, page 59-74 of the GFD-R-P.90 document
# Author: P.F.A. van Zoolingen, Computer Systems Section, Faculty of Exact Science (FEW), Vrije Universiteit, Amsterdam, The Netherlands.

from object import ObjectType, Object
from error import NotImplemented, BadParameter,IncorrectState
import jarray
import array.array

from org.ogf.saga.buffer import BufferFactory, Buffer

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

#use jython's JArray to create a byte[]. but needs to 
#update the application buffer then after each mutation....

class Buffer(Object):
    """
    The saga.buffer.Buffer class encapsulates a sequence of data to be used for
    I/O operations, that allows for uniform I/O syntax and semantics over the various SAGA API packages.

    """
    delegateObject = None
    managedByImp = None
    array = None
    applicationBuf = None
    closed = False

#DOCUMENT: Tweak for java specific arrays.
#DOCUMENT: get data only through get_data with application managed buffer
#TODO: if application managed buffer: update internal buffer between calls using the buffer
    def __init__(self, size = -1, data = None, **impl):
        # in array<byte> data, in int size,      out buffer obj or in int size = -1, out buffer obj

        """
        Initialize an I/O buffer.
        
            - B{Call format: Buffer( size, data )}
                - B{Precondition:}
                    - size must be >= 0
                - B{Postcondition:}
                    - the buffer memory is managed by the application.
            
            - B{Call format: Buffer( size )}
                - B{Postcondition:}
                    - if "data" is not specified and size > 0, the buffer memory is allocated by the implementation.
                    - if "data" is not specified, the buffer memory is managed by the implementation.
        
        @summary: Initialize an I/O buffer.
        
        @param size: size of data buffer to be used
        @type size: int
        @param data: buffer to be used
        @type data: char array or a list

        @raise NotImplemented:
        @raise BadParameter: if the implementation cannot handle the given data parameter or the given size
        @raise NoSuccess:
        @see: notes about memory management in GFD-R-P.90 document.
        """
 
        if "delegateObject" in impl:
            if not isinstance(impl["delegateObject"], org.ogf.saga.buffer.Buffer):
                raise BadParameter("Parameter impl[\"delegateObject\"] is not a org.ogf.saga.buffer.Buffer. Type: " + str(impl["delegateObject"].__class__))
            self.delegateObject = impl["delegateObject"]
            return
        if type(size) is not int:
                raise Badparameter, "Parameter size is not an int. Type:", str(type(size))
        if size < -1:
            raise BadParameter, "Parameter size is < 0"
        if type(data) is not array.array and type(data) is not list and data is not None:
            raise BadParameter, "Parameter data is not an list or a char array. Type: " + str(type(data)) 
        if type(data) is array.array and data.typecode != 'c':
            raise BadParameter, "Parameter data is an array of the wrongtype. Typecode: ",  data.typecode   
        try:
            if size != -1 and data is None:
                self.delegateObject = BufferFactory.createBuffer(size)
                self.managedByImp = True
            
            elif size != -1 and data is not None: 
                self.array = jarray.zeros(size, 'b')
                self.delegateObject =  BufferFactory.createBuffer(self.array)
                self.managedByImp = False
                self.applicationBuf = data

            elif size == -1 and data is None:
                self.delegateObject = BufferFactory.createBuffer()
                self.managedByImp = True

            elif size == -1 and data is not None:
                size = len(data)
                self.array = jarray.zeros(size, 'b')
                self.delegateObject =  BufferFactory.createBuffer(self.array)
                self.managedByImp = False
                self.applicationBuf = data
            else:
                raise BadParameter, "Parameters can not be processed. size:" + size + " " + str(type(size)) + " " + data + " " + str(type(data))          
        except org.ogf.saga.error.SagaException, e:
                raise self.convertException(e)
        
        
    def __del__(self):
        """
        Destroy a buffer.
        @note: if the instance was not closed before, __del__ performs a close() on
            the instance, and all notes to close() apply.
        """
        if self.closed is False:
            self.close(0)
        Object.__del__(self)
        
    def set_size(self, size = -1):
       """
       Set size of buffer.
       @summary: Set size of buffer.
       @param size: value for size
       @type size: int
       @PostCondition: the buffer memory is managed by the implementation.
       """ 
       if self.closed is True :
           raise IncorrectState, "Buffer object is already closed()"
       if type(size) is not int:
           raise BadParameter, "Parameter size is not an int. Type: " + str(type(size))
       if size < -1:
           raise BadParameter, "Parameter size < 0"
       try:
           if size == -1:
               self.delegateObject.setSize()
           else:
               self.delegateObject.setSize(size)
           self.managedByImp = True
           array = None
           applicationBuf = None
       except org.ogf.saga.error.SagaException, e:
           raise self.convertException(e)


    def get_size(self):
        """
        Retrieve the current value for size.
        @summary: Retrieve the current value for size.
        @return: value of size
        @rtype: int
        @Raise NotImplemented:
        @raise IncorrectState:
        @Note: if the buffer was created with negative size with the second call format of the __init__(), 
            or the size was set to a negative value with set_size(), this method 
            returns '-1' if the buffer was not yet used for an I/O operation.
        @note: if the buffer was used for a successfull I/O operation where data have been read into the
            buffer, the call returns the size of the memory which has been allocated by the
            implementation during that read operation
        """
        if self.closed is True :
           raise IncorrectState, "Buffer object is already closed()"
        try:
           return self.delegateObject.getSize()
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
        if self.closed == True :
           raise IncorrectState, "Buffer object is already closed()"
        if type(data) is not array.array and type(data) is not list:
            raise BadParameter, "Parameter data is not an list or a char array. Type: " + str(type(size)) 
        if type(data) is array.array and data.typecode != 'c':
            raise BadParameter, "Parameter data is an array of the wrongtype. Typecode:" + data.typecode
        if size < 1 and size != -1:
            raise BadParameter, "Parameter size is < 1"       
        try:
            length = size
            if length == -1:
                if len(data) > 0:
                    #DOCUMENT: this is extention to saga spec.
                    length = len(data)
                else:
                    raise BadParameter("len(data) is <= 0, and size is not specified")
            self.array = jarray.zeros(length, 'b')            
            self.delegateObject.setData(self.array)
            self.managedByImp = False
            self.applicationBuf = data
        except org.ogf.saga.error.SagaException, e:
           raise self.convertException(e)
 
    def update_data(self):
        if len(self.array) <= len(self.applicationBuf):
            for i in range(len(self.array)):
                if self.array[i] < 0:
                    self.applicationBuf[i] = chr(self.array[i]+256)
                else:
                    self.applicationBuf[i] = chr(self.array[i])
        else:  #self.array > self.applicationBuf
            for i in range(len(self.applicationBuf)):
                if self.array[i] < 0:                           
                    self.applicationBuf[i] = chr(self.array[i]+256)
                else:
                    self.applicationBuf[i] = chr(self.array[i]) 
            for i in range(len(self.applicationBuf), len(self.array) ):
                if self.array[i] < 0:                           
                    self.applicationBuf.append(chr(self.array[i]+256))
                else:
                    self.applicationBuf.append(chr(self.array[i]))                            
    
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
        if self.closed is True :
           raise IncorrectState, "Buffer object is already closed()"
        try:
            if self.managedByImp is True:
                return self.delegateObject.getData().tostring()
            else:
                if type(self.applicationBuf) is list or type(self.applicationBuf) is array:
                    self.update_data()
                    return self.applicationBuf
                else:
                    raise NoSuccess, "self.applicationBuf is not a array or a list. Internal inconsistincy."
        except org.ogf.saga.error.SagaException, e:
           raise self.convertException(e)

#type data:<type 'array'>
#file.read(10) van file (abcdefghijklmnopqrstuvwxyz)
#array('b',[97, 98, 99, 100, 101, 102, 103, 104, 105, 106]) 
# b = signed char http://docs.python.org/lib/module-array.html
 
#TODO: Use java-byte/C++-signed char, or unsigned char??
       
    def close(self, timeout = -0.0):
        #in  float timeout = -0.0
        """
        Closes the object.
        @summary: Closes the object.
        @param timeout: seconds to wait
        @type timeout: float
        @PostCondition: any operation on the object other than close() or __del__ will cause
                        an 'IncorrectState' exception.
        @raise NotImplemented:
        @note: if the current data memory is managed by the implementation, it is freed.
        @note: if the current data memory is managed by the application, it is not accessed anymore by the
               implementation after this method returns.
        @note: if close() is implicitly called in  __del__(), it will never raise an exception.
        @see: for resource deallocation semantics and timeout semantics, see  Section 2 of the GFD-R-P.90 document
        """
        if type(timeout) is not float and type(timeout) is not int:
            raise BadParameter, "Parameter timout is wrong type. Type: " + str(type(timeout))
        try:
            if timeout > 0:
                self.delegateObject.close()
                self.closed = True
            else:
                self.delegateObject.close(timeout)
                self.closed = True 
        except org.ogf.saga.error.SagaException, e:
           raise self.convertException(e)
       
    def get_type(self):
        """
        Query the object type.
        @summary: Query the object type.
        @return: type of the object as an int from ObjectType
        @rtype: int
        """
        return ObjectType.BUFFER
        
    
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
            temp = Buffer(delegateObject = javaClone)
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
