# Package: saga
# Module: attributes 
# Description: The module which specifies the classes concerning the setting 
#    and getting of attributes in saga
# Specification and documentation can be found in section 3.8, page 101-113 of 
#    the GFD-R-P.90 document
# Author: P.F.A. van Zoolingen, Computer Systems Section, Faculty of 
#    Exact Science (FEW), Vrije Universiteit, Amsterdam, The Netherlands.

from error import NotImplemented
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

class Attributes(object):
    """
    There are various places in the SAGA API where attributes need to be 
    associated with objects, for instance for job descriptions and metrics. The 
    Attributes class provides a common interface for storing and retrieving 
    attributes.
    
    @undocumented: __get_attributes
    """
    delegateObject = None
    
    def set_attribute(self, key, value):
        """
        Set an attribute to a value.
        @summary: Set an attribute to a value.
        @param key: attribute key
        @param value: value to set the attribute to
        @type key: string
        @type value: string
        @permission:    Write
        @raise NotImplemented:
        @raise BadParameter:
        @raise DoesNotExist:
        @raise  IncorrectState:
        @raise PermissionDenied:
        @raise AuthorizationFailed:
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @note: an empty string means to set an empty value (the attribute is 
            not removed).
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
            raise BadParameter, "Parameter key (" + str(type(key)) \
                +") or value (" + str(type(value)) + ") is not a string."
        try:
            self.delegateObject.setAttribute(key, value)
        except org.ogf.saga.error.SagaException, e:
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
        @permission:    Query
        @raise NotImplemented:
        @raise  DoesNotExist:
        @raise IncorrectState:
        @raise PermissionDenied:
        @raise AuthorizationFailed:
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @note: queries of attributes may imply queries of endpoint entity 
            properties. As such, authorization and/or authentication may fail 
            for querying such attributes, for some backends. In that case, the 
            respective 'AuthenticationFailed', 'AuthorizationFailed', and 
            'PermissionDenied' exceptions are raised. For example, an 
            implementation may forbid to read the saga.stream.Stream 'Bufsize' 
            attribute.
        @note: reading an attribute value for an attribute which is not in the 
            current set of attributes causes a 'DoesNotExist' exception.
        @note: if the operation is attempted on a vector attribute, an 
            'IncorrectState' exception is raised.
        @note: getting attribute values may time out, or may fail for other 
            reasons - which causes a 'Timeout' or 'NoSuccess' exception, 
            respectively.

        """
        if type(key) is not str:
            raise BadParameter, "Parameter key (" + str(type(key)) \
                + ") is not a string."
        try:
            return self.delegateObject.getAttribute(key)
        except org.ogf.saga.error.SagaException, e:
           raise self.convertException(e)
            
    def set_vector_attribute(self, key, values):
        """
        Set an attribute to a list of values.
        @summary: Set an attribute to a list of string values.
        @param key: attribute key
        @param values:  list of attribute values
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
        @note: if the operation is attempted on a scalar attribute, an 
            'IncorrectState' exception is raised.

        """
        if type(key) is not str:
            raise BadParameter, "Parameter key (" + str(type(key)) \
                +") is not a string."
        if type(values) is not list and type(values) is not tuple:
            raise BadParameter, "Parameter values (" + str(type(key)) \
                +") is not a list."        
        try:
            import jarray.array
            import java.lang.String
            jythonArray = jarray.array(values, java.lang.String)
            self.delegateObject.setVectorAttribute( key, jythonArray)
        except org.ogf.saga.error.SagaException, e:
           raise self.convertException(e)        
       
    def get_vector_attribute(self, key):
        #return a list of values
        """
        Get the list of values associated with an attribute.
        @summary: Get the list of values associated with an attribute.
        @param key: attribute key
        @type key: string
        @return: list of values of the attribute.
        @rtype: list
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
        @note: if the operation is attempted on a scalar attribute, an 
            'IncorrectState' exception is raised.

        """
        if type(key) is not str:
            raise BadParameter, "Parameter key (" + str(type(key)) \
                +") is not a string."
        try:
            javaArray = self.delegateObject.getVectorAttribute(key)
            return list(javaArray)
        except org.ogf.saga.error.SagaException, e:
           raise self.convertException(e)
       
    def remove_attribute(self, key):
        """
        Removes an attribute.
        @summary: Removes an attribute.
        @param key: attribute to be removed
        @type key: string
        @postcondition: the attribute is not available anymore.
        @permission: Write
        @raise NotImplemented:
        @raise DoesNotExist:
        @raise PermissionDenied:
        @raise AuthorizationFailed:
        @raise  AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @note: a vector attribute can also be removed with this method
        @note: only some SAGA objects allow to remove attributes.
        @note: a ReadOnly attribute cannot be removed - any attempt to do so 
            raises a 'PermissionDenied' exception.
        @note: if a non-existing attribute is removed, a 'DoesNotExist' 
            exception is raised.
        @note: exceptions have the same semantics as defined for the 
            set_attribute() method description.
        """
        if type(key) is not str:
            raise BadParameter, "Parameter key (" + str(type(key)) \
                +") is not a string."        
        try:
            self.delegateObject.removeAttribute(key)
        except org.ogf.saga.error.SagaException, e:
            raise self.convertException(e)   
    
    def list_attributes(self):
        #return list of keys out
        """
        Get the list of attribute keys.
        @summary: Get the list of attribute keys.
        @return: existing attribute keys
        @rtype: list
        @permission: Query
        @raise NotImplemented:
        @raise PermissionDenied:
        @raise AuthorizationFailed:
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @note: exceptions have the same semantics as defined
            for the get_attribute() method description.
        @note: if no attributes are defined for the object, an empty list is 
            returned.

        """
        try:
            javaArray = self.delegateObject.listAttributes()
            return list(javaArray)
        except org.ogf.saga.error.SagaException, e:
            raise self.convertException(e) 
        
    def find_attributes(self,  pattern):
        #return keys_list
        """
        Find matching attributes.
        @summary: Find matching attributes.
        @param pattern: list of string search patterns
        @type pattern: list
        @return: matching attribute keys
        @rtype: list
        @permission: Query
        @raise NotImplemented:
        @raise BadParameter:
        @raise PermissionDenied:
        @raise AuthorizationFailed:
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @note: the pattern must be formatted as described earlier, otherwise a 
            'BadParameter' exception is raised.
        @note: exceptions have the same semantics as defined for the 
            get_attribute() method description.

        """
        if type(pattern) is not list and type(pattern) is not tuple:
            from saga.error import BadParameter
            raise BadParameter( "Parameter pattern (" + str(type(pattern)) \
                                +") is not a list.")
        tempString = None
        javaArray = None
        import jarray.array
        import java.lang.String
                
        try:
            if len(pattern) is 0:
                temp = java.lang.String("")
                tempArray = jarray.array(temp, java.lang.String)                
                javaArray = self.delegateObject.findAttributes(tempArray)
            elif len(pattern) is 1:
                temp = java.lang.String(pattern[0])
                tempArray = jarray.array([temp], java.lang.String)
                javaArray = self.delegateObject.findAttributes(tempArray)
            else:
                tempArray = jarray.array(pattern, java.lang.String) 
                javaArray = self.delegateObject.findAttributes(tempArray)
        except org.ogf.saga.error.SagaException, e:
            raise self.convertException(e)    
        return list(javaArray)   
#TODO: String[] instead of ... 
    
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
        @note: This method returns True if the attribute identified by the key 
            exists.
        @note:exceptions have the same semantics as defined for the 
            get_attribute() method description, apart from the fact that a 
            'DoesNotExist' exception is never raised.

        """
        if type(key) is not str:
            raise BadParameter, "Parameter key (" + str(type(key)) \
                +") is not a string."       
        try:
            retval = self.delegateObject.existsAttribute(key)
            if retval == True:
                return True
            else:
                return False
        except org.ogf.saga.error.DoesNotExistException, d:
            return False
        except org.ogf.saga.error.SagaException, e:
           raise self.convertException(e)   

#TODO: No attributeExists() in java 
#TODO: check workaround 
       
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
        @note: This method returns True if the attribute identified by the key 
            exists, and can be read by get_attribute() or get_vector 
            attribute(), but cannot be changed by set_attribute() and
            set_vector_attribute().
        @note: exceptions have the same semantics as defined for the 
            get_attribute() method description.

        """
        if type(key) is not str:
            raise BadParameter, "Parameter key (" + str(type(key)) \
                +") is not a string."
        try:
            retval = self.delegateObject.isReadOnlyAttribute(key)
            if retval == 1:
                return True
            else:
                return False
        except org.ogf.saga.error.SagaException, e:
           raise self.convertException(e)  
        
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
        @note: This method returns True if the attribute identified by the key 
            exists, and can be changed by set_attribute() or 
            set_vector_attribute().
        @note: exceptions have the same semantics as defined for the 
            get_attribute() method description.

        """
        if type(key) is not str:
            raise BadParameter, "Parameter key (" + str(type(key)) \
                +") is not a string."
        try:
            retval = self.delegateObject.isWritableAttribute(key)
            if retval == 1:
                return True
            else:
                return False
        except org.ogf.saga.error.SagaException, e:
           raise self.convertException(e) 
    
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
        @note: This method returns True if the attribute identified by the key 
            exists, and can be removed by remove_attribute().
        @note: exceptions have the same semantics as defined for 
            the get_attribute() method description.

        """
        if type(key) is not str:
            raise BadParameter, "Parameter key (" + str(type(key)) \
                +") is not a string."
        try:
            retval = self.delegateObject.isRemovableAttribute(key)
            if retval == 1:
                return True
            else:
                return False
        except org.ogf.saga.error.SagaException, e:
           raise self.convertException(e) 
       
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
        @note: This method returns True if the attribute identified by key is a 
            vector attribute.
        @note: exceptions have the same semantics as defined for 
            the get_attribute() method description.

        """
        if type(key) is not str:
            raise BadParameter, "Parameter key (" + str(type(key)) \
                +") is not a string."
        try:
            retval = self.delegateObject.isVectorAttribute(key)
            if retval == 1:
                return True
            else:
                return False
        except org.ogf.saga.error.SagaException, e:
           raise self.convertException(e) 
       

        
    def __get_attributes(self):
        attributes=self.list_attributes()
        temp = {}
        for i in attributes:
            if self.attribute_is_vector(i):
                temp[i]=self.get_vector_attribute(i)
            else:
                temp[i]=self.get_attribute(i)
        return temp
    
    attributes = property(__get_attributes,
          doc="""The attributes in dictionary form.\n@type: dict""")
