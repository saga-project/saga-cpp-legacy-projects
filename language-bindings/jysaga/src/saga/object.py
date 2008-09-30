# Package: saga
# Module: object 
# Description: The module which specifies the base SAGA object class and ObjectType class
# Specification and documentation can be found in section 3.2, page 47-52 of the GFD-R-P.90 document
# Author: P.F.A. van Zoolingen, Computer Systems Section, Faculty of Exact Science (FEW), Vrije Universiteit, Amsterdam, The Netherlands.

from error import NotImplemented

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
        
    def get_id(self):
        raise NotImplemented, "get_id() is not implemented in this object"
      
    def get_type(self):
        raise NotImplemented, "get_type() is not implemented in this object"
        
    def get_session(self):
        raise NotImplemented, "get_session() is not implemented in this object"
    
    def clone(self):
        raise NotImplemented, "clone() is not implemented in this object"
    
    def convertException(self, e):
        object = None
        message = ""
        try:
            object = e.getObject()
            print "convertException: java Exception had an attached sagaObject. Object is NOT yet translated to a python object"
            object = None
            #TODO: convert sagaObject naar Object -> referentie naar self
        except java.lang.Exception, exception:
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
            error = NoSuccess(message, object)
            #DOCUMENT: Document SagaIOException -> NoSucces IOError, multiple inheritence
        elif isinstance(e, org.ogf.saga.error.TimeoutException):
            error = Timeout(message, object)
        else:
            error = NoSuccess(message, object)
        return error
 