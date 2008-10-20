# Package: saga 
# Module: object 
# Description: The module which specifies the base SAGA object class and ObjectType class
# Specification and documentation can be found in section 3.2, page 47-52 of the GFD-R-P.90 document
# Author: P.F.A. van Zoolingen, Computer Systems Section, Faculty of Exact Science (FEW), Vrije Universiteit, Amsterdam, The Netherlands.

class ObjectType(object):
    """
    ObjectType allows for inspection of SAGA object instances.
    This, in turn, allows to treat large numbers of SAGA object instances in containers, 
    without the need to create separate container types for each specific SAGA object type.
    @version: 1.0

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
    at the application level as well as allowing for inspection of objects type and its
    associated session.
    @version: 1.0
    
    """
    delegateObject = None
        
    def get_id(self):
        """
        Query the object ID.
        @summary: Query the object ID.
        @return: uuid for the object
        @rtype: string 
        """
        try:
            return self.delegateObject.getId()
        except java.lang.Exception, e:
           raise self.convertException(e)
      
    def get_type(self):
        """
        Query the object type.
        @summary: Query the object type.
        @return: type of the object as an int from ObjectType
        @rtype: int
        """
        from saga.error import NotImplemented
        raise NotImplemented, "get_type() is not yet implemented in this object"
        
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
            javaSession = self.delegateObject.getSession()
            session = Session(sessionObject=javaSession)
            return session
        except java.lang.Exception, e:
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
            context = Context(delegateObject=javaClone)
            return context
        except java.lang.Exception, e:
            raise self.convertException(e)
        
    def convertException(self, e):
        object = None
        message = ""
        from org.ogf.saga.error.AlreadyExistsException import AlreadyExistsException
        from org.ogf.saga.error.AuthenticationFailedException import AuthenticationFailedException
        from org.ogf.saga.error.AuthorizationFailedException import AuthorizationFailedException
        from org.ogf.saga.error.BadParameterException import BadParameterException
        from org.ogf.saga.error.DoesNotExistException import DoesNotExistException
        from org.ogf.saga.error.IncorrectStateException import IncorrectStateException
        from org.ogf.saga.error.IncorrectURLException import IncorrectURLException
        from org.ogf.saga.error.NoSuccessException import NoSuccessException
        from org.ogf.saga.error.NotImplementedException import NotImplementedException
        from org.ogf.saga.error.PermissionDeniedException import PermissionDeniedException
        from org.ogf.saga.error.SagaException import SagaException
        from org.ogf.saga.error.SagaIOException import SagaIOException
        from org.ogf.saga.error.TimeoutException import TimeoutException


        try:
            object = e.getObject()
            print "convertException: java Exception had an attached sagaObject. Object is NOT yet translated to a python object"
            object = None
            #TODO: convert sagaObject naar Object -> referentie naar self
        except org.ogf.saga.error.DoesNotExistException, exception:
            object = None
        message = e.getMessage()
               
        if isinstance(e, org.ogf.saga.error.AlreadyExistsException):
            error = AlreadyExists(message, object)
        elif isinstance(e, org.ogf.saga.error.AuthenticationFailedException):
            error = AuthenticationFailed(message, object)
        elif isinstance(e, org.ogf.saga.error.AuthorizationFailedException):
            error = AuthorizationFailed(message, object)
        elif isinstance(e, org.ogf.saga.error.BadParameterException):
            error = BadParameter(message, object)
        elif isinstance(e, org.ogf.saga.error.DoesNotExistException):
            error = DoesNotExist(message, object)
        elif isinstance(e, org.ogf.saga.error.IncorrectStateException):
            IncorrectState(message, object)
        elif isinstance(e, org.ogf.saga.error.IncorrectURLException):
            error = IncorrectURL(message, object)
        elif isinstance(e, org.ogf.saga.error.NoSuccessException):
            error = NoSuccess(message, object)
        elif isinstance(e, org.ogf.saga.error.NotImplementedException):
            error = NotImplemented(message, object)
        elif isinstance(e, org.ogf.saga.error.PermissionDeniedException):
            error = PermissionDenied(message, object)
        elif isinstance(e, org.ogf.saga.error.SagaIOException):
            error = sagaIO(message, object)
        elif isinstance(e, org.ogf.saga.error.TimeoutException):
            error = Timeout(message, object)
        else:
            error = NoSuccess(message, object)
        return error
    
#DOCUMENT: Document SagaIOException -> NoSucces IOError, multiple inheritence 