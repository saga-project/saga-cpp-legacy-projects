# Package: pysaga
# Module: object 
# Description: The module which specifies the base SAGA object class and 
#    ObjectType class
# Specification and documentation can be found in section 3.2, page 47-52 of 
#    the GFD-R-P.90 document
# Author: P.F.A. van Zoolingen, Computer Systems Section, Faculty of 
#    Exact Science (FEW), Vrije Universiteit, Amsterdam, The Netherlands.

from error import NotImplemented

class ObjectType(object):
    """
    ObjectType allows for inspection of SAGA object instances.
    This, in turn, allows to treat large numbers of SAGA object instances in 
    containers, without the need to create separate container types for each 
    specific SAGA object type.
    
    @version: 1.0, designed for Python 2.x

    """
    EXCEPTION         =  1
    URL               =  2
    BUFFER            =  3
    SESSION           =  4
    CONTEXT           =  5
    TASK              =  6
    TASKCONTAINER     =  7
    METRIC            =  8
    NSENTRY           =  9
    NSDIRECTORY       = 10
    IOVEC             = 11
    FILE              = 12
    DIRECTORY         = 13
    LOGICALFILE       = 14
    LOGICALDIRECTORY  = 15
    JOBDESCRIPTION    = 16
    JOBSERVICE        = 17
    JOB               = 18
    JOBSELF           = 19
    STREAMSERVICE     = 20
    STREAM            = 21
    PARAMETER         = 22
    RPC               = 23
  
class Object(object):
    """
    The SAGA object provides methods which are essential for all SAGA
    objects. It provides a unique ID which helps maintain a list of SAGA objects
    at the application level as well as allowing for inspection of objects type 
    and its associated session.
    
    @version: 1.0, designed for Python 2.x
    
    """
        
    def get_id(self):
        """Query the object ID.
        
        @summary: Query the object ID.
        @return: uuid for the object
        @rtype: string 
        
        """
        raise NotImplemented("This method is not yet implemented")
      
    def get_type(self):
        """Query the object type.
        
        @summary: Query the object type.
        @return: type of the object as an int from ObjectType
        @rtype: int
        
        """
        raise NotImplemented("This method is not yet implemented")
      
    def get_session(self):
        """Query the objects session.
        
        @summary: Query the objects session.
        @return: session of the object
        @rtype: L{Session}
        @PreCondition: the object was created in a session, either
            explicitly or implicitly.
        @PostCondition: the returned session is shallow copied.
        @raise DoesNotExist:
        @Note: if no specific session was attached to the object at creation 
            time, the default SAGA session is returned.
        @note: some objects do not have sessions attached, such as 
            JobDescription, Task, Metric, and the Session object itself. For 
            such objects, the method raises a 'DoesNotExist' exception.
        
        """
        raise NotImplemented("This method is not yet implemented")
    
    def clone(self):
        """@summary: Deep copy the object
        
        @return: the deep copied object
        @rtype: L{Object}
        @PostCondition: apart from session and callbacks, no other state is 
            shared between the original object and it's copy.
        @raise NoSuccess:
        @Note: that method is overloaded by all classes which implement 
            Object, and returns a deep copy of the respective class type.
        @see: section 2 of the GFD-R-P.90 document for deep copy semantics.

        """
        raise NotImplemented("This method is not yet implemented")

    
    id = property(get_id, 
            doc="""The object ID.
                @type: string""")
    
    type= property(get_type,
            doc="""The object type.
                @type: int""")
    
    session= property(get_session,
            doc="""The L{Session} of the object.
                @type:L{Session}""")
    
    