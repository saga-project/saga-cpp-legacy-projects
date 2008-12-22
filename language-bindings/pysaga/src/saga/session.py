# Package: pysaga
# Module: session 
# Description: The module which specifies the session used in SAGA
# Specification and documentation can be found in section 3.5, 
#    page 75-80 of the GFD-R-P.90 document
# Author: P.F.A. van Zoolingen, Computer Systems Section, Faculty of 
#    Exact Science (FEW), Vrije Universiteit, Amsterdam, The Netherlands.

from object import Object, ObjectType
from attributes import Attributes
from error import NotImplemented

class Session(Object):
    """
    
    The session object provides the functionality of a session, which isolates 
    independent sets of SAGA objects from each other. Sessions also support the
    management of security information
    
    @version: 1.0, designed for Python 2.x    
    
    """
        
    def __init__(self, default=True):
        """Initialize the object.
        
        @summary: Initialize the object.
        @param default: indicates this object should be the default session 
        @type default: bool
        @raise NotImplemented:
        @raise NoSuccess:
        @Note: the created session has no context instances attached.
        @note: if 'default' is specified as 'True', the constructor returns a 
            shallow copy of the default session, with all the default contexts 
            attached. The application can then change the properties of the 
            default session, which is continued to be implicetly used on the 
            creation of all SAGA objects, unless specified otherwise.
        
        """
        
    def add_context(self, context):
        """Attach a security context to a session.
        
        @summary: Attach a security context to a session.
        @param context: Security context to add
        @type context: L{Context}
        @PostCondition: the added context is deep copied, and no state is shared.
        @postcondition: any object within that session can use the context, 
            even if it was created before add_context() was called.
        @raise NotImplemented:
        @Note: if the session already has a context attached which has exactly 
            the same set of attribute values as the parameter context, no action 
            is taken.

        """
        raise NotImplemented("This method is not yet implemented")

    def remove_context(self, context):
        """Detach a security context from a session.
        
        @summary: Detach a security context from a session.
        @param context: Security context to remove
        @type context: L{Context}
        @raise NotImplemented:
        @raise DoesNotExist:
        @PreCondition: a context with completely identical attributes is 
            available in the session.
        @PostCondition: that context is removed from the session, and can from 
            now on not be used by any object in that session, even if it was 
            created before remove_context() was called.
        @Note: this methods removes the context on the session which has exactly 
            the same set of parameter values as the parameter context.
        @note: a 'DoesNotExist' exception is raised if no context exist on the 
            session which has the same attributes as the parameter context.

        """
        raise NotImplemented("This method is not yet implemented")
        
    def list_contexts(self):
        """Retrieve all contexts attached to a session.
        
        @summary: Retrieve all contexts attached to a session.
        @return: contexts of this session
        @rtype: list
        @raise NotImplemented:
        @Note: a empty list is returned if no context is attached.
        @note: contexts may get added to a session by default, hence the 
            returned list may be non-empty even if add_context() was never 
            called before.
        @note: a context might still be in use even if not included in the 
            returned list. See notes about context life time in the GFD-R-P.90 
            document.

        """
        raise NotImplemented("This method is not yet implemented")
    
    contexts = property(list_contexts,
            doc="""Contexts attached to a session
                @type: list""")
    
    
