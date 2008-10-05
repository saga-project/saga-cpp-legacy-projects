# Package: saga
# Module: context 
# Description: The module which specifies the context classes used in saga
# Specification and documentation can be found in section 3.6, page 81-86 of the GFD-R-P.90 document
# Author: P.F.A. van Zoolingen, Computer Systems Section, Faculty of Exact Science (FEW), Vrije Universiteit, Amsterdam, The Netherlands.
#GFD-R-P.90 section 3.6, page 81,  package saga.context

from object import Object, ObjectType
from attributes import Attributes
from error import NotImplemented
from session import Session
from org.ogf.saga.context import ContextFactory, Context

class Context(Object, Attributes):
    contextObject = None
    """
    The saga.context.Context class provides the functionality of a security information container.

        - B{Attributes:} 
            - B{Type}:
                - name: Type
                - desc: type of context
                - mode: ReadWrite
                - type: String
                - value: naming conventions as described in GFD-R-P.90 apply
            - B{Server}:
                - name: Server
                - desc: server which manages the context
                - mode: ReadWrite
                - type: String
                - value: -
                - note: a typical example would be the contact information for a MyProxy server, such as
                  'myproxy.remote.net:7512', for a 'myproxy' type context.
            - B{CertRepository}:
                - name: CertRepository
                -    desc: location of certificates and CA signatures
                -    mode: ReadWrite
                -    type: String
                -    value: -
                -    note: a typical example for a globus type context
                       would be "/etc/grid-security/certificates/".
            - B{UserProxy}:
                -    name: UserProxy
                -    desc: location of an existing certificate proxy to be used
                -    mode: ReadWrite
                -    type: String
                -    value: -
                -    note: a typical example for a globus type context would be "/tmp/x509up_u<uid>".
            - B{UserCert}:
                -    name: UserCert
                -    desc: location of a user certificate to use
                -    mode: ReadWrite
                -    type: String
                -    value: -
                -    note: a typical example for a globus type context would be "$HOME/.globus/usercert.pem".
            - B{UserKey}:
                -    name: UserKey
                -    desc: location of a user key to use
                -    mode: ReadWrite
                -    type: String
                -    value: -
                -    note: a typical example for a globus type context would be "$HOME/.globus/userkey.pem".
            - B{UserID}:
                -    name:  UserID
                -    desc:  user id or user name to use
                -    mode:  ReadWrite
                -    type:  String
                -    value: -
                -    note: a typical example for a ftp type context would be "anonymous".
            - B{UserPass}:
                -    name:  UserPass
                -    desc:  password to use
                -    mode:  ReadWrite
                -    type:  String
                -    value: -
                -    note: a typical example for a ftp type context would be "anonymous@localhost".
            - B{UserVO}:
                -    name:  UserVO
                -    desc:  the VO the context belongs to
                -    mode:  ReadWrite
                -    type:  String
                -    value: -
                -    note: a typical example for a globus type context  would be "O=dutchgrid".
            - B{LifeTime}:
                -    name:  LifeTime
                -    desc:  time up to which this context is valid
                -    mode:  ReadWrite
                -    type:  Int
                -    value: -1
                -    note: time and date specified in number of seconds since epoch, 
                          a value of -1 indicates an infinite lifetime.
            - B{RemoteID}:
                -    name:  RemoteID
                -    desc:  user ID for an remote user, who is identified by this context.
                -    mode: ReadOnly
                -    type: String
                -    value: -
                -    note: a typical example for a globus type context would be
                      "/O=dutchgrid/O=users/O=vu/OU=cs/CN=Joe Doe".
            - B{RemoteHost}
                -     name: RemoteHost
                -     desc: the hostname where the connection origininates which is identified by this context.
                -     mode: ReadOnly
                -     type: String
                -     value: -
                -     name: RemotePort
                -     desc: the port used for the connection which is identified by this context.
                -     mode: ReadOnly
                -     type: String
                -     value: -
    """

    def __init__(self, name="", **impl):
        """
        Initialize a security context.
        @summary: Initialize a security context.
        @param name: initial type of context
        @type name: string
        @raise NotImplemented:
        @raise IncorrectState:
        @raise Timeout:
        @raise NoSuccess:
        @note: if name is given (i.e. non-empty), then the __init__ internally calls set_defaults().
            The notes to set_defaults apply.
        @see: the notes about lifetime management in Section 2 of the GFD-R-P.90 document
        """
        if contextObject in impl:
            if type(impl["contextObject"]) is not org.ogf.saga.url.URL:
                raise BadParameter, "Parameter impl[\"contextObject\"] is not a org.ogf.saga.context.Context. Type: " + str(type(impl["contextObject"]))
            self.contextObject = impl["contextObject"]
        try:
            if name is "" or type(name) is not string:
                self.contextObject = ContextFactory.createContext();
            else:
                self.contextObject = ContextFactory.createContext(name);
        except java.lang.Exception, e:
            raise self.convertException(e)

    def set_defaults(self):
        #contextObject.setDefaults();
        """
        Set default values for specified context type.
        @summary: Set default values for specified context type.
        @PostCondition: the context is valid, and can be used for authorization.
        @raise NotImplemented:
        @raise IncorrectState:
        @raise Timeout:
        @raise NoSuccess:
        @note: the method avaluates the value of the 'Type' attribute, and of all other non-empty
              attributes, and, based on that information, tries to set sensible default values for all
              previously empty attributes.
        @note: if the 'Type' attribute has an empty value, an 'IncorrectState' exception is raised.
        @note: this method can be called more than once on a context instance.
        @note: if the implementation cannot create valid default values based on the available
              information, an 'NoSuccess' exception is raised, and a detailed error message is given,
              describing why no default values could be set.

        """
        try:
            self.contextObject.setDefaults()
        except java.lang.Exception, e:
            raise self.convertException(e)
        
    def get_id(self):
        """
        Query the object ID.
        @summary: Query the object ID.
        @return: uuid for the object
        @rtype: string 
        """
        try:
            return contextObject.getId()
        except java.lang.Exception, e:
           raise self.convertException(e)
      
    def get_type(self):
        """
        Query the object type.
        @summary: Query the object type.
        @return: type of the object as an int from ObjectType
        @rtype: int
        """
        return ObjectType.CONTEXT
        
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
        try:
            javaSession = self.contextObject.getSession()
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
            javaContext = self.contextObject.clone()
            context = Context(contextObject=javaContext)
            return context
        except java.lang.Exception, e:
            raise self.convertException(e)        

    def set_attribute(self, key, value):
        """
        Set an attribute to a value.
        @summary: Set an attribute to a value.
        @param key: attribute key
        @param value: value to set the attribute to
        @type key: string
        @type value: string
        @permission: Write
        @raise NotImplemented:
        @raise BadParameter:
        @raise DoesNotExist:
        @raise IncorrectState:
        @raise PermissionDenied:
        @raise AuthorizationFailed:
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @note: an empty string means to set an empty value (the attribute is not removed).
        @note: the attribute is created, if it does not exist
        @note: a 'PermissionDenied' exception is raised if the
             attribute to be changed is ReadOnly.
        @note: only some SAGA objects allow to create new
             attributes - others allow only access to
             predefined attributes. If a non-existing
             attribute is queried on such objects, a
             'DoesNotExist' exception is raised
        @note: changes of attributes may reflect changes of
             endpoint entity properties. As such,
             authorization and/or authentication may fail
             for settings such attributes, for some
             backends. In that case, the respective
             'AuthenticationFailed', 'AuthorizationFailed',
             and 'PermissionDenied' exceptions are raised.
             For example, an implementation may forbid to
             change the saga.stream.Stream 'Bufsize' attribute.
        @note: if an attribute is not well formatted, or
             outside of some allowed range, a 'BadParameter'
             exception with a descriptive error message is
             raised.
        @note: if the operation is attempted on a vector
             attribute, an 'IncorrectState' exception is
             raised.
        @note: setting of attributes may time out, or may fail
             for other reasons - which causes a 'Timeout' or
             'NoSuccess' exception, respectively.

        """
        if type(key) is not str or type(value) is not str:
            raise BadParameter, "Parameter key (" + str(type(key)) +") or value (" + str(type(value)) + ") is not a string."
        try:
            contextObject.setAttribute(key, value)
        except java.lang.Exception, e:
           raise self.convertException(e)
          
    def get_attribute(self, key):
        #return value
        """
        Get an attribute value.
        @summary: Get an attribute value.
        @param key: attribute key
        @type key: string
        @return: value: value of the attribute
        @rtype: string
        @permission: Query
        @raise NotImplemented:
        @raise  DoesNotExist:
        @raise IncorrectState:
        @raise PermissionDenied:
        @raise AuthorizationFailed:
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @note: queries of attributes may imply queries of endpoint entity properties. As such,
              authorization and/or authentication may fail for querying such attributes, for some
              backends. In that case, the respective 'AuthenticationFailed', 'AuthorizationFailed',
              and 'PermissionDenied' exceptions are raised. For example, an implementation may forbid to
              read the saga.stream.Stream 'Bufsize' attribute.
        @note: reading an attribute value for an attribute which is not in the current set of attributes
              causes a 'DoesNotExist' exception.
        @note: if the operation is attempted on a vector attribute, an 'IncorrectState' exception is raised.
        @note: getting attribute values may time out, or may fail for other reasons - which causes a
              'Timeout' or 'NoSuccess' exception, respectively.

        """
        if type(key) is not str:
            raise BadParameter, "Parameter key (" + str(type(key)) + ") is not a string."
        try:
            return contextObject.getAttribute(key)
        except java.lang.Exception, e:
           raise self.convertException(e)
           
    def set_vector_attribute(self, key, values):
        """
        Set an attribute to an list of values.
        @summary: Set an attribute to an array of values.
        @param key:                  attribute key
        @param values:               array of attribute values
        @type key: string
        @type values: list
        @permission: Write
        @raise NotImplemented:
        @raise BadParameter:
        @raise DoesNotExist:
        @raise IncorrectState:
        @raise PermissionDenied:
        @raise AuthorizationFailed:
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @note: the notes to the set_attribute() method apply.
        @note: if the operation is attempted on a scalar attribute, an 'IncorrectState' exception is raised.

        """
#TODO add other methods
        if type(key) is not str or type(value) is not str:
            raise BadParameter, "Parameter key (" + str(type(key)) +") or value (" + str(type(value)) + ") is not a string."
        try:
            contextObject.setAttribute(key, value)
        except java.lang.Exception, e:
           raise self.convertException(e)        
        raise NotImplemented, "set_vector_attribute() is not implemented in this object"
    
    def get_vector_attribute(self, key):
        #return a list of values
        """
        Get the tuple of values associated with an attribute.
        @summary: Get the tuple of values associated with an attribute.
        @param key: attribute key
        @type key: string
        @return: tuple of values of the attribute.
        @rtype: tuple
        @permission: Query
        @raise NotImplemented:
        @raise DoesNotExist:
        @raise IncorrectState:
        @raise PermissionDenied:
        @raise AuthorizationFailed:
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @note: the notes to the get_attribute() method apply.
        @note: if the operation is attempted on a scalar attribute, an 'IncorrectState' exception is raised.

        """
        raise NotImplemented, "get_vector_attribute() is not implemented in this object"
    
    def remove_attribute(self, key):
        """
        Removes an attribute.
        @summary: Removes an attribute.
        @param    key:                  attribute to be removed
        @type key: string
        @postcondition: - the attribute is not available anymore.
        @permission:    Write
        @raise NotImplemented:
        @raise DoesNotExist:
        @raise PermissionDenied:
        @raise AuthorizationFailed:
        @raise  AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @note: a vector attribute can also be removed with this method
        @note: only some SAGA objects allow to remove attributes.
        @note: a ReadOnly attribute cannot be removed - any attempt to do so raises a 'PermissionDenied' exception.
        @note: if a non-existing attribute is removed, a 'DoesNotExist' exception is raised.
        @note: exceptions have the same semantics as defined for the set_attribute() method description.
        """

        raise NotImplemented, "remove_attribute() is not implemented in this object"
    
    def list_attributes(self):
        #return list of keys out
        """
        Get the list of attribute keys.
        @summary: Get the list of attribute keys.
        @return: existing attribute keys
        @rtype: tuple
        @permission: Query
        @raise NotImplemented:
        @raise PermissionDenied:
        @raise AuthorizationFailed:
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @note: exceptions have the same semantics as defined
            for the get_attribute() method description.
        @note: if no attributes are defined for the object, an empty tuple is returned.

        """
        raise NotImplemented, "list_attributes() is not implemented in this object"
    
    def find_attributes(self,  pattern):
        #return keys_list
        """
        Find matching attributes.
        @summary: Find matching attributes.
        @param pattern: list of string search patterns
        @type pattern: list
        @return: matching attribute keys
        @rtype: tuple
        @permission: Query
        @raise NotImplemented:
        @raise BadParameter:
        @raise PermissionDenied:
        @raise AuthorizationFailed:
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @note: the pattern must be formatted as described earlier, otherwise a 'BadParameter' exception is raised.
        @note: exceptions have the same semantics as defined for the get_attribute() method description.

        """
        raise NotImplemented, "find_attributes() is not implemented in this object"
    
    def attribute_exists(self, key):
        """
        Check the attribute's existence.
        @summary: Check the attribute's existence.
        @param key: attribute key
        @type key: string
        @return: does the attribute exist
        @rtype: bool
        @permission:    Query
        @raise NotImplemented:
        @raise PermissionDenied:
        @raise  AuthorizationFailed:
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @note: This method returns True if the attribute identified by the key exists.
        @note:exceptions have the same semantics as defined for the get_attribute() method description,
            apart from the fact that a 'DoesNotExist' exception is never raised.

        """
        raise NotImplemented, "attribute_exists() is not implemented in this object"
    
    def attribute_is_readonly(self, key):
        """
        Check if the attribute is read only.
        @summary: Check if the attribute is read only.
        @param    key:                  attribute key
        @type key: string
        @return: indicator if the attribute read only
        @rtype: bool
        @permission:    Query
        @raise NotImplemented:
        @raise  DoesNotExist:
        @raise PermissionDenied:
        @raise AuthorizationFailed:
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @note: This method returns True if the attribute identified by the key exists, and can be read
              by get_attribute() or get_vector attribute(), but cannot be changed by set_attribute() and
              set_vector_attribute().
        @note: exceptions have the same semantics as defined for the get_attribute() method description.

        """
        raise NotImplemented, "attribute_is_readonly() is not implemented in this object"
        
    def attribute_is_writable(self, key):
        """
        Check if the attribute is writable.
        @summary: Check if the attribute is writable.
        @param key: attribute key
        @type key: string
        @return: indicator if the attribute is writable
        @rtype: bool
        @permission:    Query
        @raise NotImplemented:
        @raise DoesNotExist:
        @raise PermissionDenied:
        @raise AuthorizationFailed:
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @note: This method returns True if the attribute identified by the key exists, and can be
            changed by set_attribute() or set_vector_attribute().
        @note: exceptions have the same semantics as defined for the get_attribute() method description.

        """
        raise NotImplemented, "attribute_is_writeable() is not implemented in this object"
    
    def attribute_is_removable (self, key):
        """
        Check if the attribute is removable.
        @summary: Check if the attribute is removable.
        @param key: attribute key
        @type key: string
        @return: indicator if the attribute is removable
        @rtype: bool
        @permission:    Query
        @raise NotImplemented:
        @raise  DoesNotExist:
        @raise PermissionDenied:
        @raise AuthorizationFailed:
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @note: This method returns True if the attribute identified by the key exists, and can be
                 removed by remove_attribute().
        @note: exceptions have the same semantics as defined for the get_attribute() method description.

        """
        raise NotImplemented, "attribute_is_removeable() is not implemented in this object"
    
    def attribute_is_vector(self, key):
        """
        Check whether the attribute is a vector or a scalar.
        @summary: Check whether the attribute is a vector or a scalar.
        @param key: attribute key
        @type key: string
        @return: indicator if the  attribute is scalar (False) or vector (True)
        @rtype: bool
        @permission:    Query
        @raise NotImplemented:
        @raise DoesNotExist:
        @raise PermissionDenied:
        @raise AuthorizationFailed:
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @note: This method returns True if the attribute identified by key is a vector attribute.
        @note: exceptions have the same semantics as defined for the get_attribute() method description.

        """
        raise NotImplemented, "attribute_is_vector() is not implemented in this object"