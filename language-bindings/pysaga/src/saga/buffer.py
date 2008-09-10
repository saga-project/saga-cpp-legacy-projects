#page 62
#package saga.buffer
from object import ObjectType, Object
from error import NotImplemented

#use jython's JArray to create a byte[]. but needs to 
#update the application buffer then after each mutation....

class Buffer(Object):
    """
    The saga.buffer.Buffer class encapsulates a sequence of data to be used for
    I/O operations, that allows for uniform I/O syntax and semantics over the various SAGA API packages.

    """

    def __init__(self, size = -1, data = None):
        # in array<byte> data, in int size,      out buffer obj or in int size = -1, out buffer obj

        """
        Create an I/O buffer
        
            - B{Call format: Buffer( size, data )}
                - B{Precondition:}
                    - size must be >= 0
                - B{Postcondition:}
                    - the buffer memory is managed by the application.
            
            - B{Call format: Buffer( size )}
                - B{Postcondition:}
                    - if "data" is not specified and size > 0, the buffer memory is allocated by the implementation.
                    - if "data" is not specified, the buffer memory is managed by the implementation.
        
        @summary: Create an I/O buffer
        
        @param size: size of data buffer to be used
        @type size: int
        @param data: buffer to be used
        @type data: char array or a list

        @raise NotImplemented:
        @raise BadParameter: if the implementation cannot handle the given data pointer or the given size
        @raise NoSuccess:
        @see: notes about memory management in GFD-R-P.90 document.
        """
        super(Buffer,self).__init__()
        
    #destructor -> """Destroy a buffer  if the instance was not closed before, the 
    # DESTRUCTOR performs a close() on the instance, and all notes to close() apply.

        
    def set_size( size = -1):
       """
       Set size of buffer
       @summary: Set size of buffer
       @param size: value for size
       @type size: int
       @PostCondition: the buffer memory is managed by the implementation.
       """ 

    def get_size(self):
        """
        Retrieve the current value for size
        @summary: Retrieve the current value for size
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
        size = 0;
        return size
    
    def set_data(self, data, size=-1):
        #in array<byte>  data, in int size
        """
        Set new buffer data
        @summary: Set new buffer data
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
               the first CONSTRUCTOR with the given size.
        @note: the notes for the DESTRUCTOR and the first CONSTRUCTOR apply.
        """
    
    def get_data(self):
        #out array<byte> data
        """
        Retrieve the buffer data
        @summary: Retrieve the buffer data
        @return: buffer data to retrieve. Type depends on what type was used to create the Buffer object
        @rtype: char array or list
        @Raise NotImplemented:
        @raise DoesNotExist:
        @raise IncorrectState:
        @Note: see notes about memory management
        @note: if the buffer was created as implementation
                    managed (size = -1), but no I/O operation has
                    yet been successfully performed on the buffer,
                    a 'DoesNotExist' exception is thrown.
        """
        data = ""
        return data
        
    def close(self, timeout = -0.0):
        #in  float timeout = -0.0
        """
        Closes the object
        @summary: Closes the object
        @param timeout: seconds to wait
        @type timeout: float
        @PostCondition: any operation on the object other than close() or the DESTRUCTOR will cause
                        an 'IncorrectState' exception.
        @raise NotImplemented:
        @note: if the current data memory is managed by the implementation, it is freed.
        @note: if the current data memory is managed by the application, it is not accessed anymore by the
               implementation after this method returns.
        @note: if close() is implicitly called in  __del__(), it will never throw an exception.
        @see: for resource deallocation semantics and timeout semantics, see  Section 2 of the GFD-R-P.90 document
        """

        #Add Object Methods
